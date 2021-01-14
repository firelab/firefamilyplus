// wordpdoc.cpp : implementation of the CRichDoc class
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
#include "fireplus.h"
#include <afxrich.h>
#include "richdoc.h"
#include "richview.h"
#include "direct.h"

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
// CRichDoc
IMPLEMENT_DYNCREATE(CRichDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(CRichDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(CRichDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichDoc construction/destruction

CRichDoc::CRichDoc()
{
	//m_nNewDocType = -1;
}

BOOL CRichDoc::OnNewDocument()
{
	if (!CRichEditDoc::OnNewDocument())
		return FALSE;

 	return TRUE;
}

void CRichDoc::ReportSaveLoadException(LPCTSTR lpszPathName,
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

BOOL CRichDoc::OnSaveDocument(LPCTSTR lpszPathName)
{

	//chdir(theApp.userDir);
	if (!CRichEditDoc::OnSaveDocument(lpszPathName))
		return FALSE;
	if(isTemp)
	{
		isTemp = false;
		if(strcmpi(lpszPathName, tName) != 0)
			unlink(tName);
	}
	return TRUE;
		
}

BOOL CRichDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{

	isTemp = false;
	tName = lpszPathName;
	m_bRTF = false;
	if (!CRichEditDoc::OnOpenDocument(lpszPathName))
		return FALSE;
	return TRUE;
}

void CRichDoc::Serialize(CArchive& ar)
{
	COleMessageFilter* pFilter = AfxOleGetMessageFilter();
	ASSERT(pFilter != NULL);
	pFilter->EnableBusyDialog(FALSE);
	CRichEditDoc::Serialize(ar);
	pFilter->EnableBusyDialog(TRUE);
}


CRichView* CRichDoc::GetView()
{
	POSITION pos = GetFirstViewPosition();
	return (CRichView* )GetNextView( pos );
}
CRichEditCntrItem* CRichDoc::CreateClientItem(REOBJECT* preo) const
{
	// cast away constness of this
	return NULL;//new CWordPadCntrItem(preo, (CRichDoc*)this);
}

/////////////////////////////////////////////////////////////////////////////
// CRichDoc Operations
BOOL CRichDoc::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	return CRichEditDoc::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CRichDoc::OnCloseDocument() 
{
	char temp[256];
	bool itemp = isTemp;
	strcpy(temp, tName);
	CRichEditDoc::OnCloseDocument();
	if(itemp)
		unlink(temp);
}

void CRichDoc::PreCloseFrame(CFrameWnd* pFrameArg)
{
	CRichEditDoc::PreCloseFrame(pFrameArg);
	//SaveState(m_nDocType);
}

CFile* CRichDoc::GetFile(LPCTSTR pszPathName, UINT nOpenFlags, CFileException* pException)
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
// CRichDoc diagnostics

#ifdef _DEBUG
void CRichDoc::AssertValid() const
{
	CRichEditDoc::AssertValid();
}

void CRichDoc::Dump(CDumpContext& dc) const
{
	CRichEditDoc::Dump(dc);
}
#endif //_DEBUG


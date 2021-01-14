// FiresDoc.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "FiresDoc.h"


// CFiresDoc

IMPLEMENT_DYNCREATE(CFiresDoc, CDocument)

CFiresDoc::CFiresDoc()
{
	records = NULL;
}

BOOL CFiresDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CFiresDoc::~CFiresDoc()
{
	if(records)
	{
		records->Close();
		delete records;
	}
}


BEGIN_MESSAGE_MAP(CFiresDoc, CDocument)
END_MESSAGE_MAP()


// CFiresDoc diagnostics

#ifdef _DEBUG
void CFiresDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CFiresDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CFiresDoc serialization

void CFiresDoc::Serialize(CArchive& ar)
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
#endif


// CFiresDoc commands

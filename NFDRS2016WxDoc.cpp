// NFDRS2016WxDoc.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "NFDRS2016WxDoc.h"


// CNFDRS2016WxDoc

IMPLEMENT_DYNCREATE(CNFDRS2016WxDoc, CDocument)

CNFDRS2016WxDoc::CNFDRS2016WxDoc()
{
	records = NULL;
}

BOOL CNFDRS2016WxDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CNFDRS2016WxDoc::~CNFDRS2016WxDoc()
{
	if (records)
	{
		records->Close();
		delete records;
	}
}


BEGIN_MESSAGE_MAP(CNFDRS2016WxDoc, CDocument)
END_MESSAGE_MAP()


// CNFDRS2016WxDoc diagnostics

#ifdef _DEBUG
void CNFDRS2016WxDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CNFDRS2016WxDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CNFDRS2016WxDoc serialization

void CNFDRS2016WxDoc::Serialize(CArchive& ar)
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


// CNFDRS2016WxDoc commands

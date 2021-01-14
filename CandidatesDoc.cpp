// CandidatesDoc.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "CandidatesDoc.h"


// CCandidatesDoc

IMPLEMENT_DYNCREATE(CCandidatesDoc, CDocument)

CCandidatesDoc::CCandidatesDoc()
{
	records = NULL;
	m_FireDay = m_LargeFireDay = m_MultiFireDay = 1;

}

BOOL CCandidatesDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CCandidatesDoc::~CCandidatesDoc()
{
	if(records)
	{
		records->Close();
		delete records;
	}
}


BEGIN_MESSAGE_MAP(CCandidatesDoc, CDocument)
END_MESSAGE_MAP()


// CCandidatesDoc diagnostics

#ifdef _DEBUG
void CCandidatesDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CCandidatesDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CCandidatesDoc serialization

void CCandidatesDoc::Serialize(CArchive& ar)
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


// CCandidatesDoc commands

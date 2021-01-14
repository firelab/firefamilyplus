// ClimDoc.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "ClimDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClimDoc

IMPLEMENT_DYNCREATE(CClimDoc, CDocument)

CClimDoc::CClimDoc()
{
}

BOOL CClimDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CClimDoc::~CClimDoc()
{
}


BEGIN_MESSAGE_MAP(CClimDoc, CDocument)
	//{{AFX_MSG_MAP(CClimDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClimDoc diagnostics

#ifdef _DEBUG
void CClimDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CClimDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CClimDoc serialization

void CClimDoc::Serialize(CArchive& ar)
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
// CClimDoc commands

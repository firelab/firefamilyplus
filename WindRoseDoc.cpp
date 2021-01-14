// WindRoseDoc.cpp : implementation file
//
 
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "wxSet.h"
#include "WindRoseDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWindRoseDoc

IMPLEMENT_DYNCREATE(CWindRoseDoc, CDocument)

CWindRoseDoc::CWindRoseDoc()
{
	recs = 0;
	days = 0;
	days16 = 0;
	maxPerc = 0.0;
    dateStr = "";
	staStr = "";
	perStr = "";
	hourStr = "";
	extraStr = "";
    
	
}

BOOL CWindRoseDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CWindRoseDoc::~CWindRoseDoc()
{
	
	
}


BEGIN_MESSAGE_MAP(CWindRoseDoc, CDocument)
	//{{AFX_MSG_MAP(CWindRoseDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindRoseDoc diagnostics

#ifdef _DEBUG
void CWindRoseDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWindRoseDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



/////////////////////////////////////////////////////////////////////////////
// CWindRoseDoc commands

void CWindRoseDoc::DeleteContents() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	

	CDocument::DeleteContents();
}

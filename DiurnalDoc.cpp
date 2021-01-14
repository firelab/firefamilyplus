// DiurnalDoc.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DiurnalDoc.h"
#include "DiurnalFormView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiurnalDoc

IMPLEMENT_DYNCREATE(CDiurnalDoc, CDocument)

CDiurnalDoc::CDiurnalDoc()
{
	da = NULL;
	dateTime = COleDateTime::GetCurrentTime();
	stamp = "";
}

BOOL CDiurnalDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CDiurnalDoc::~CDiurnalDoc()
{
	if(da)
		delete da;
}


BEGIN_MESSAGE_MAP(CDiurnalDoc, CDocument)
	//{{AFX_MSG_MAP(CDiurnalDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiurnalDoc diagnostics

#ifdef _DEBUG
void CDiurnalDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDiurnalDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDiurnalDoc serialization

void CDiurnalDoc::Serialize(CArchive& ar)
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
// CDiurnalDoc commands

/*void CDiurnalDoc::OnFileSaveAs() 
{

   POSITION pos = GetFirstViewPosition();
   while (pos != NULL)
   {
      CView* pView = GetNextView(pos);
      if(pView->IsKindOf( RUNTIME_CLASS( CDiurnalFormView ) ) )
	  {
			((CDiurnalFormView *)pView)->SaveTheDamnThing();
			break;
	  }
   }   		
}*/

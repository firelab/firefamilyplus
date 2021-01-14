// WeatherDoc.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "WeatherDoc.h"


// CWeatherDoc

IMPLEMENT_DYNCREATE(CWeatherDoc, CDocument)

CWeatherDoc::CWeatherDoc()
{
	records = NULL;
}

BOOL CWeatherDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CWeatherDoc::~CWeatherDoc()
{
	if(records)
	{
		records->Close();
		delete records;
	}
}


BEGIN_MESSAGE_MAP(CWeatherDoc, CDocument)
END_MESSAGE_MAP()


// CWeatherDoc diagnostics

#ifdef _DEBUG
void CWeatherDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CWeatherDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CWeatherDoc serialization

void CWeatherDoc::Serialize(CArchive& ar)
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


// CWeatherDoc commands

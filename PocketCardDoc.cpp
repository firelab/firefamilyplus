// PocketCardDoc.cpp : implementation file
//
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "wxSet.h"
#include "PocketCardDoc.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "climateSet.h"
#include "SIGStationSet.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPocketCardDoc

IMPLEMENT_DYNCREATE(CPocketCardDoc, CDocument)

CPocketCardDoc::CPocketCardDoc()
{
	m_pAnalysis = NULL;
}

BOOL CPocketCardDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CPocketCardDoc::~CPocketCardDoc()
{
	if(m_pAnalysis)
		delete m_pAnalysis;
	/*if(periods)
	{
		for(int j = 0; j < MAXVARIDS; j++)
		{
			if(periods[j])
			{
				for(int i = 0; i < numPeriods; i++)
					delete periods[j][i];
				delete[] periods[j];
			}
		}
		if(numPeriods > 0)
			delete[] periods;
	}
	periods = NULL;
	if(ytdPeriods)
	{
		for(int j = 0; j < MAXVARIDS; j++)
		{
			if(ytdPeriods[j])
			{
				for(int i = 0; i < numPeriods; i++)
					delete ytdPeriods[j][i];
				delete[] ytdPeriods[j];
			}
		}
		if(numPeriods > 0)
			delete[] ytdPeriods;
	}
	ytdPeriods = NULL;*/
}


BEGIN_MESSAGE_MAP(CPocketCardDoc, CDocument)
	//{{AFX_MSG_MAP(CPocketCardDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPocketCardDoc diagnostics

#ifdef _DEBUG
void CPocketCardDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CPocketCardDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPocketCardDoc serialization

void CPocketCardDoc::Serialize(CArchive& ar)
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
// CPocketCardDoc commands

void CPocketCardDoc::DeleteContents() 
{
	// TODO: Add your specialized code here and/or call the base class
	CDocument::DeleteContents();
}

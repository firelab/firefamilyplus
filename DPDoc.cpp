// DPDoc.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DPDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDPDoc

IMPLEMENT_DYNCREATE(CDPDoc, CDocument)

CDPDoc::CDPDoc()
{
	fires = NULL;
	nDP = 0;
	for(int i = 0; i < MAXDPS; i++)
	{
		lowers[i] = 0.0;
	}
	nd = nfd = nlfd = nmfd = 0;
}

BOOL CDPDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CDPDoc::~CDPDoc()
{
	if(fires)
		delete fires;
}


BEGIN_MESSAGE_MAP(CDPDoc, CDocument)
	//{{AFX_MSG_MAP(CDPDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDPDoc diagnostics

#ifdef _DEBUG
void CDPDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDPDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDPDoc serialization

void CDPDoc::Serialize(CArchive& ar)
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
// CDPDoc commands

void CDPDoc::CalcClasses()
{
	double maxVal = 0, nextVal;
	nd = nfd = nlfd = nmfd = 0;
	long d = 0, loc = 0;
	if(nDP > 0)
	{
		if(!goUp)
		{
			if(fires->allVals)
			{
				while(fires->allVals[d] < lowers[0] && d < fires->nv)
					d++;
				nd = fires->nv - d;
			}
			d = 0;
			if(fires->fdVals)
			{
				while(fires->fdVals[d] < lowers[0] && d < fires->nfd)
					d++;
				nfd = fires->nfd - d;
			}
			d = 0;
			if(fires->lfdVals)
			{
				while(fires->lfdVals[d] < lowers[0] && d < fires->nlfd)
					d++;
				nlfd = fires->nlfd - d;
			}
			d = 0;
			if(fires->mfdVals)
			{
				while(fires->mfdVals[d] < lowers[0] && d < fires->nmfd)
					d++;
				nmfd = fires->nmfd - d;
			}
		}
		else
		{
			d = fires->nv - 1;
			if(fires->allVals)
			{
				while(fires->allVals[d] > lowers[0] && d > 0)
					d--;
				nd = d + 1;
			}
			d = fires->nfd - 1;
			if(fires->fdVals)
			{
				while(fires->fdVals[d] > lowers[0] && d < 0)
					d--;
				nfd = d + 1;
			}
			d = fires->nlfd - 1;
			if(fires->lfdVals)
			{
				while(fires->lfdVals[d] > lowers[0] && d < 0)
					d--;
				nlfd = d + 1;
			}
			d = fires->nmfd - 1;
			if(fires->mfdVals)
			{
				while(fires->mfdVals[d] > lowers[0] && d < 0)
					d--;
				nmfd = d + 1;
			}
		}
	}
	if(fires->nv > 0)
		maxVal = (goUp == true) ? fires->allVals[0] : fires->allVals[fires->nv - 1];
		//maxVal = fires->allVals[fires->nv - 1];
	if(!goUp)
	{
		for(int i = 0; i < nDP; i++)
		{
			if(i < nDP - 1)
				nextVal = lowers[i + 1];
			else
				nextVal = maxVal;
			cd[i] = cfd[i] = clfd[i] = cmfd[i] = loc = 0;
			if(fires->allVals)
			{
				while(fires->allVals[loc] < lowers[i] && loc < fires->nv)
					loc++;
				while((fires->allVals[loc] < nextVal || i == nDP - 1) && loc < fires->nv)
				{
					cd[i]++;
					loc++;
				}
			}
			loc = 0;
			if(fires->fdVals)
			{
				while(fires->fdVals[loc] < lowers[i] && loc < fires->nfd)
					loc++;
				while((fires->fdVals[loc] < nextVal || i == nDP - 1) && loc < fires->nfd)
				{
					cfd[i]++;
					loc++;
				}
			}
			loc = 0;
			if(fires->lfdVals)
			{
				while(fires->lfdVals[loc] < lowers[i] && loc < fires->nlfd)
					loc++;
				while((fires->lfdVals[loc] < nextVal || i == nDP - 1) && loc < fires->nlfd)
				{
					clfd[i]++;
					loc++;
				}
			}
			loc = 0;
			if(fires->mfdVals)
			{
				while(fires->mfdVals[loc] < lowers[i] && loc < fires->nmfd)
					loc++;
				while((fires->mfdVals[loc] < nextVal || i == nDP - 1) && loc < fires->nmfd)
				{
					cmfd[i]++;
					loc++;
				}
			}
		}
	}
	else
	{
		//for(int i = nDP - 1; i >= 0; i--)
		for(int i = 0; i < nDP; i++)
		{
			if(i < nDP - 1)
				nextVal = lowers[i + 1];
			else
				nextVal = maxVal;
			cd[i] = cfd[i] = clfd[i] = cmfd[i] = loc = 0;
			loc = nd - 1;
			if(fires->allVals)
			{
				while(loc >= 0 && fires->allVals[loc] > lowers[i])
					loc--;
				while(loc >= 0 && (fires->allVals[loc] > nextVal || i == nDP - 1))
				{
					cd[i]++;
					loc--;
				}
			}
			loc = nfd - 1;
			if(fires->fdVals)
			{
				while(loc >= 0 && fires->fdVals[loc] > lowers[i])
					loc--;
				while(loc >= 0 && (fires->fdVals[loc] > nextVal || i == nDP - 1))
				{
					cfd[i]++;
					loc--;
				}
			}
			loc = nlfd - 1;
			if(fires->lfdVals)
			{
				while(loc >= 0 && fires->lfdVals[loc] > lowers[i])
					loc--;
				while(loc >= 0 && (fires->lfdVals[loc] > nextVal || i == nDP - 1))
				{
					clfd[i]++;
					loc--;
				}
			}
			loc = nmfd - 1;
			if(fires->mfdVals)
			{
				while(loc >= 0 && fires->mfdVals[loc] > lowers[i])
					loc--;
				while(loc >= 0 && (fires->mfdVals[loc] > nextVal || i == nDP - 1))
				{
					cmfd[i]++;
					loc--;
				}
			}
		}
	}

}


void CDPDoc::SetDefaults()
{
	if(fires->nv < 5)
		return;
	//setup initial classes
	goUp = false;
	if((fires->varID >= 5 && fires->varID <= 8) || (fires->varID >= 17 && fires->varID <= 23))
		goUp = true;
	double pcnt;
	int loc;
	if(goUp)
		pcnt = 0.03;
	else
		pcnt = 0.97;
	loc = (int)max(0.0, ((double)fires->nv) * pcnt - 1);
	lowers[4] = fires->allVals[loc];
	if(goUp)
		pcnt = 0.1;
	else
		pcnt = 0.9;
	loc = (int)max(0.0, ((double)fires->nv) * pcnt - 1);
	lowers[3] = fires->allVals[loc];
	if(goUp)
	{
		loc = (int)max(0.0, ((double)fires->nv) * 0.4 - 1);
		lowers[2] = fires->allVals[loc]; //lowers[3] + (fires->allVals[fires->nv - 1] - lowers[3]) / 2.0;
		loc = (int)max(0.0, ((double)fires->nv) * 0.6 - 1);
		lowers[1] = fires->allVals[loc]; //lowers[2] + (fires->allVals[fires->nv - 1] - lowers[2]) / 2.0;
		lowers[0] = fires->allVals[fires->nv - 1];
	}
	else
	{
		loc = (int)max(0.0, ((double)fires->nv) * 0.6 - 1);
		lowers[2] = fires->allVals[loc]; //lowers[2] = lowers[3] / 2.0;
		loc = (int)max(0.0, ((double)fires->nv) * 0.4 - 1);
		lowers[1] = fires->allVals[loc]; //		lowers[1] = lowers[2] / 2.0;
		lowers[0] = 0.0;
	}
	nDP = 5;
	CalcClasses();
}

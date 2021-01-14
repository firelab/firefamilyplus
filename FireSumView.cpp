// FireSumView.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "fireplusset.h"
#include "FireSumView.h"
#include "FireSummary.h"
//#include "FireSumDoc.h"
#include <math.h>
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFireSumView
extern char *monthNames[];
extern CFireplusApp theApp;

IMPLEMENT_DYNCREATE(CFireSumView, CView)

CFireSumView::CFireSumView()
{
}

CFireSumView::~CFireSumView()
{
}


BEGIN_MESSAGE_MAP(CFireSumView, CView)
	//{{AFX_MSG_MAP(CFireSumView)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireSumView drawing

void CFireSumView::OnDraw(CDC* pDC)
{
	CRect rect;
	if(!pDC->IsPrinting())
	{
		GetClientRect(rect);
		rect.NormalizeRect();
		if(rect.Width() == 0 || rect.Height() == 0)
			return;
	}
	else
	{
		pDC->GetBoundsRect(&rect, DCB_RESET);
	}
	DrawToDCRegion(pDC, rect);
/*	CRect rect;
	if(!pDC->IsPrinting())
	{
		GetClientRect(rect);
		rect.NormalizeRect();
	}
	else
	{
		pDC->GetBoundsRect(&rect, DCB_RESET);
		//rect.NormalizeRect();
	}
	if(rect.Width() == 0 || rect.Height() == 0)
		return;
	DrawToDCRegion(pDC, rect);*/
}

void CFireSumView::DrawToDCRegion(CDC *pDC, CRect rect)
{
	//CFireSumDoc *pDoc = (CFireSumDoc *)GetDocument();
	CString str;
	// TODO: add draw code here
	if(pDC->IsPrinting())
	{
		int lx, ly;
		ly = pDC->GetDeviceCaps(VERTRES);
		lx = pDC->GetDeviceCaps(HORZRES);
		rect.left = 0;
		rect.top = 0;
		rect.right = lx;
		rect.bottom = ly;
	}
	CRect faRect(rect.left, rect.top, 
			rect.left + 2 * rect.Width() / 3, rect.top + 3 * rect.Height() / 5), 
		mRect(rect.left + 2 * rect.Width() / 3, rect.top, 
			rect.right, rect.top + 3 * rect.Height() / 5 ), 
		scRect(rect.left, rect.top + 3 * rect.Height() / 5,
			rect.left + rect.Width() / 3, rect.bottom), 
		ccRect(rect.left + rect.Width() / 3, rect.top + 3 * rect.Height() / 5,
			rect.left + 2 * rect.Width() / 3, rect.bottom), 
		fdRect(rect.left  + 2 * rect.Width() / 3, rect.top + 3 * rect.Height() / 5,
			rect.right, rect.bottom);
    CPen penBlack, lgrayPen, redPen, bluePen, *oldPen;  // Construct it, then initialize
    if(!penBlack.CreatePen( PS_SOLID, 1, RGB(0,0,0)) 
		|| !bluePen.CreatePen( PS_SOLID, 1, RGB(0,0,200))    
		|| !redPen.CreatePen( PS_SOLID, 1, RGB(200,0,0))    
		|| !lgrayPen.CreatePen( PS_SOLID, 1, RGB(200,200,200)))    
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Resources too low for creating drawing pens.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CFont headerFont, vFont, *oldFont;
	if(!headerFont.CreateFont(faRect.Height() / 16, faRect.Width() / 70, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica") 
		|| !vFont.CreateFont(faRect.Height() / 16, faRect.Width() / 70, 900, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica"))
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create graph fonts.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CBrush redBrush, blueHatchBrush, whiteBrush, *oldBrush;
	if(!redBrush.CreateSolidBrush(RGB(200,0,0))
		|| !whiteBrush.CreateSolidBrush(0x00FFFFFF)
		|| !blueHatchBrush.CreateHatchBrush(HS_FDIAGONAL, RGB(0,0,200)))
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create brushes.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	oldPen = pDC->SelectObject(&penBlack);
	oldFont = pDC->SelectObject(&headerFont);
	oldBrush = pDC->SelectObject(&whiteBrush);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);

	pDC->Rectangle(faRect);
	pDC->Rectangle(mRect);
	pDC->Rectangle(scRect);
	pDC->Rectangle(ccRect);
	pDC->Rectangle(fdRect);
	//now draw the 5 bargraphs to the 5 rects
	int x0, y0, x1, y1, x, y, bar;
	//Fires and Acres by Year
	x0 = faRect.left + 2 * faRect.Width() / 15;
	x1 = faRect.right - 2 * faRect.Width() / 15;
	y0 = faRect.bottom - 3 * faRect.Height() / 16;
	y1 = faRect.top + 3 * faRect.Height() / 16;
	pDC->SelectObject(&penBlack);
	pDC->MoveTo(x0, y1);
	pDC->LineTo(x0, y0);
	pDC->LineTo(x1, y0);
	pDC->LineTo(x1, y1);
	pDC->SelectObject(&headerFont);
	pDC->TextOut(faRect.left + faRect.Width() / 2, faRect.bottom - faRect.Height() / 40,
		"Total Fires and Acres by Year");
	CString tStr;
	tStr.Format("Fires (%ld)", totFires);
	pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
	pDC->TextOut(faRect.left + (x0 - faRect.left) / 20, y1 - (y1 - faRect.top) / 2, tStr);
	//pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->TextOut(faRect.left + (x0 - faRect.left) / 20, y1 - (y1 - faRect.top) / 4, "(Solid)");
	tStr.Format("Acres (%.0f)", totAcres);
	pDC->SetTextAlign(TA_BASELINE | TA_RIGHT);
	pDC->TextOut(faRect.right - (faRect.right - x1) / 20, y1 - (y1 - faRect.top) / 2, tStr);
	//pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->TextOut(faRect.right - (faRect.right - x1) / 20 , y1 - (y1 - faRect.top) / 4, "(Hatch)");
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	bar = (x1 - x0) / (m_pFiresSummary->nYears * 3);
	long fIdeal;
	double aIdeal;
	IdealFromYMax(&firesMax, &acresMax, &fIdeal, &aIdeal);
	y = y0;
	x = 0;
	pDC->SetTextAlign(TA_TOP | TA_RIGHT);
	pDC->SelectObject(&penBlack);
	while( y >= y1 )
	{
		pDC->MoveTo(x0 - (x0 - faRect.left)/10, y);
		pDC->LineTo(x0, y);
		str.Format("%ld ", x * fIdeal);
		CSize sz = pDC->GetTextExtent(str);
		pDC->TextOut(x0 - (x0 - rect.left)/10, y  - sz.cy / 2, str);
		x++;
		y = y0 - (int)(( ((double)x * fIdeal) / (double)firesMax) * (double)(y0 - y1));
	}
	y = y0;
	x = 0;
	pDC->SetTextAlign(TA_TOP | TA_LEFT);
	pDC->SelectObject(&penBlack);
	while( y >= y1 )
	{
		pDC->MoveTo(x1 + (faRect.right - x1)/10 - 1, y);
		pDC->LineTo(x1, y);
		str.Format("%.0f ", x * aIdeal);
		CSize sz = pDC->GetTextExtent(str);
		pDC->TextOut(x1 + (faRect.right - x1)/10, y  - sz.cy / 2, str);
		x++;
		y = y0 - (int)(( ((double)x * aIdeal) / acresMax) * (double)(y0 - y1));
	}
	x = x0 - bar;// * 2;
	//int skip = m_pFiresSummary->nYears / 5 + 1;
	pDC->SelectObject(&headerFont);
	CSize lSize = pDC->GetTextExtent("2000");
	int i;
	for(i = 0; i < m_pFiresSummary->nYears; i++)
	{
		x += 3 * bar;
		if(m_pFiresSummary->fires[i] > 0)
		{
			pDC->SelectObject(&redBrush);
			pDC->SelectObject(&redPen);
			y = y0 - (int)((double)m_pFiresSummary->fires[i] / (double)firesMax * (double)(y0 - y1));
			pDC->Rectangle(x - bar, y, x, y0);
		}
		if(m_pFiresSummary->acres[i] > 0.0)
		{
			pDC->SelectObject(&blueHatchBrush);
			pDC->SelectObject(&bluePen);
			y = y0 - (int)((double)m_pFiresSummary->acres[i] / (double)acresMax * (double)(y0 - y1));
			pDC->Rectangle(x, y, x + bar, y0);
		}
		pDC->SelectObject(&penBlack);
		pDC->MoveTo(x, y0);
		pDC->LineTo(x, y0 + lSize.cy / 2);
		if(i % yrSkip == 0)
		{
			pDC->SetTextAlign(TA_TOP | TA_CENTER);
			pDC->SelectObject(&headerFont);
			str.Format("%4d", m_pFiresSummary->startYear + i);
			pDC->TextOut(x, y0 + lSize.cy / 2, str);
		}
	}

	//%fires by month
	x0 = mRect.left + 3 * mRect.Width() / 16;
	x1 = mRect.right - mRect.Width() / 16;
	y0 = mRect.bottom - 3 * mRect.Height() / 16;
	y1 = mRect.top + 3 * mRect.Height() / 32;
	bar = (x1 - x0) / 24;
	pDC->SelectObject(&penBlack);
	pDC->MoveTo(x0, y1);
	pDC->LineTo(x0, y0);
	pDC->LineTo(x1, y0);
	pDC->SelectObject(&lgrayPen);
	pDC->SetTextAlign(TA_TOP | TA_RIGHT);
	for(i = 1; i <= 10; i++)
	{
		y = y0 - (int)(( ((double)(i * 10)) / 100.0) * (double)(y0 - y1));
		pDC->MoveTo(x0 + 1, y);
		pDC->LineTo(x1, y);
		if(i % 2 == 0)
		{
			str.Format("%d", i * 10);
			CSize sz = pDC->GetTextExtent(str);
			pDC->TextOut(x0 - (x0 - mRect.left) / 10, y - sz.cy / 2, str);
		}
	}
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SelectObject(&headerFont);
	pDC->TextOut(x0 + (x1 - x0) / 2, mRect.bottom - mRect.Height() / 40,
		"Month");
	pDC->SelectObject(&vFont);
	pDC->TextOut(mRect.left + (x0 - mRect.left) / 3 , y1 + (y0 - y1) / 2, "Percent");
	x = x0 - bar;
	pDC->SelectObject(&redPen);
	pDC->SelectObject(&redBrush);
	for(i = 0; i < 12; i++)
	{
		x += 2 * bar;
		if(m_pFiresSummary->monthFires[i] > 0)
		{
			y = y0 - (int)((double)m_pFiresSummary->monthFires[i] / (double)m_pFiresSummary->monthFires[12] * (double)(y0 - y1));
			pDC->Rectangle(x - bar / 2, y, x + bar / 2, y0);
			pDC->SelectObject(&vFont);
			str.Format("%ld", m_pFiresSummary->monthFires[i]);
			pDC->SetTextAlign(TA_TOP | TA_LEFT);
			CSize sz = pDC->GetTextExtent(str);
			if(y - sz.cx < mRect.top)
				y = mRect.top + sz.cx + 1; 
			pDC->TextOut(x - sz.cy / 2, y, str);
		}
		pDC->SetTextAlign(TA_TOP | TA_CENTER);
		pDC->SelectObject(&headerFont);
		str.Format("%1.1s", monthNames[i]);
		pDC->TextOut(x, y0, str);

	}

	//%fires by sizeclass
	x0 = scRect.left + 3 * scRect.Width() / 16;
	x1 = scRect.right - scRect.Width() / 16;
	y0 = scRect.bottom - 3 * scRect.Height() / 16;
	y1 = scRect.top + 3 * scRect.Height() / 32;
	pDC->SelectObject(&penBlack);
	pDC->MoveTo(x0, y1);
	pDC->LineTo(x0, y0);
	pDC->LineTo(x1, y0);
	pDC->SelectObject(&lgrayPen);
	pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_TOP | TA_RIGHT);
	for(i = 1; i <= 10; i++)
	{
		y = y0 - (int)(( ((double)(i * 10)) / 100.0) * (double)(y0 - y1));
		pDC->MoveTo(x0 + 1, y);
		pDC->LineTo(x1, y);
		if(i % 2 == 0)
		{
			str.Format("%d", i * 10);
			CSize sz = pDC->GetTextExtent(str);
			pDC->TextOut(x0 - (x0 - scRect.left) / 10, y - sz.cy / 2, str);
		}
	}
	pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->TextOut(scRect.left + scRect.Width() / 2, scRect.bottom - scRect.Height() / 40,
		"Size Class");
	pDC->SelectObject(&vFont);
	pDC->TextOut(scRect.left + (x0 - scRect.left) / 3 , y1 + (y0 - y1) / 2, "Percent");
	bar = (x1 - x0) / (NSIZECLASSES * 2);
	x = x0 - bar;
	pDC->SelectObject(&redPen);
	pDC->SelectObject(&redBrush);
	for(i = 0; i < NSIZECLASSES; i++)
	{
		x += 2 * bar;
		if(m_pFiresSummary->sizeClasses[i] > 0)
		{
			y = y0 - (int)((double)m_pFiresSummary->sizeClasses[i] / (double)m_pFiresSummary->sizeClasses[NSIZECLASSES] * (double)(y0 - y1));
			pDC->Rectangle(x - bar / 2, y, x + bar / 2, y0);
			pDC->SelectObject(&vFont);
			str.Format("%ld", m_pFiresSummary->sizeClasses[i]);
			pDC->SetTextAlign(TA_TOP | TA_LEFT);
			CSize sz = pDC->GetTextExtent(str);
			if(y - sz.cx < scRect.top)
				y = scRect.top + sz.cx + 1; 
			pDC->TextOut(x - sz.cy / 2, y, str);
		}
		pDC->SetTextAlign(TA_TOP | TA_CENTER);
		pDC->SelectObject(&headerFont);
		str.Format("%c", i + 'A');
		pDC->TextOut(x, y0, str);

	}

	//%fires by cause class
	x0 = ccRect.left + 3 * ccRect.Width() / 16;
	x1 = ccRect.right - ccRect.Width() / 16;
	y0 = ccRect.bottom - 3 * ccRect.Height() / 16;
	y1 = ccRect.top + 3 * ccRect.Height() / 32;
	pDC->SelectObject(&penBlack);
	pDC->MoveTo(x0, y1);
	pDC->LineTo(x0, y0);
	pDC->LineTo(x1, y0);
	pDC->SelectObject(&lgrayPen);
	pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_TOP | TA_RIGHT);
	for(i = 1; i <= 10; i++)
	{
		y = y0 - (int)(( ((double)(i * 10)) / 100.0) * (double)(y0 - y1));
		pDC->MoveTo(x0 + 1, y);
		pDC->LineTo(x1, y);
		if(i % 2 == 0)
		{
			str.Format("%d", i * 10);
			CSize sz = pDC->GetTextExtent(str);
			pDC->TextOut(x0 - (x0 - ccRect.left) / 10, y - sz.cy / 2, str);
		}
	}
	pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->TextOut(ccRect.left + ccRect.Width() / 2, ccRect.bottom - ccRect.Height() / 40,
		"Cause Class");
	pDC->SelectObject(&vFont);
	pDC->TextOut(ccRect.left + (x0 - ccRect.left) / 3 , y1 + (y0 - y1) / 2, "Percent");
	bar = (x1 - x0) / (NCAUSECLASSES * 2);
	x = x0 - bar;
	pDC->SelectObject(&redPen);
	pDC->SelectObject(&redBrush);
	for(i = 0; i < NCAUSECLASSES; i++)
	{
		x += 2 * bar;
		if(m_pFiresSummary->causeClasses[i] > 0)
		{
			y = y0 - (int)((double)m_pFiresSummary->causeClasses[i] / (double)m_pFiresSummary->causeClasses[NCAUSECLASSES] * (double)(y0 - y1));
			pDC->Rectangle(x - bar / 2, y, x + bar / 2, y0);
			pDC->SelectObject(&vFont);
			str.Format("%ld", m_pFiresSummary->causeClasses[i]);
			pDC->SetTextAlign(TA_TOP | TA_LEFT);
			CSize sz = pDC->GetTextExtent(str);
			if(y - sz.cx < ccRect.top)
				y = ccRect.top + sz.cx + 1; 
			pDC->TextOut(x - sz.cy / 2, y, str);
		}
		pDC->SetTextAlign(TA_TOP | TA_CENTER);
		pDC->SelectObject(&headerFont);
		str.Format("%d", i + 1);
		pDC->TextOut(x, y0, str);
	}

	//%fire days by #fires per day
	x0 = fdRect.left + 3 * fdRect.Width() / 16;
	x1 = fdRect.right - fdRect.Width() / 16;
	y0 = fdRect.bottom - 3 * fdRect.Height() / 16;
	y1 = fdRect.top + 3 * fdRect.Height() / 32;
	pDC->SelectObject(&penBlack);
	pDC->MoveTo(x0, y1);
	pDC->LineTo(x0, y0);
	pDC->LineTo(x1, y0);
	pDC->SelectObject(&lgrayPen);
	pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_TOP | TA_RIGHT);
	for(i = 1; i <= 10; i++)
	{
		y = y0 - (int)(( ((double)(i * 10)) / 100.0) * (double)(y0 - y1));
		pDC->MoveTo(x0 + 1, y);
		pDC->LineTo(x1, y);
		if(i % 2 == 0)
		{
			str.Format("%d", i * 10);
			CSize sz = pDC->GetTextExtent(str);
			pDC->TextOut(x0 - (x0 - fdRect.left) / 10, y - sz.cy / 2, str);
		}
	}
	str.Format("Fires/F-Day(F-Days=%ld)", m_pFiresSummary->nPerDay[NDAYVALS]); 
	pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->TextOut(fdRect.left + fdRect.Width() / 2, fdRect.bottom - fdRect.Height() / 40,
		str);
	pDC->SelectObject(&vFont);
	pDC->TextOut(fdRect.left + (x0 - fdRect.left) / 3 , y1 + (y0 - y1) / 2, "Percent");
	bar = (x1 - x0) / (NDAYVALS * 2);
	x = x0 - bar;
	pDC->SelectObject(&redPen);
	pDC->SelectObject(&redBrush);
	for(i = 0; i < NDAYVALS; i++)
	{
		x += 2 * bar;
		if(m_pFiresSummary->nPerDay[i] > 0)
		{
			y = y0 - (int)((double)m_pFiresSummary->nPerDay[i] / (double)m_pFiresSummary->nPerDay[NDAYVALS] * (double)(y0 - y1));
			pDC->Rectangle(x - bar / 2, y, x + bar / 2, y0);
			pDC->SelectObject(&vFont);
			str.Format("%ld", m_pFiresSummary->nPerDay[i]);
			pDC->SetTextAlign(TA_TOP | TA_LEFT);
			CSize sz = pDC->GetTextExtent(str);
			if(y - sz.cx < fdRect.top)
				y = fdRect.top + sz.cx + 1; 
			pDC->TextOut(x - sz.cy / 2, y, str);
		}
		pDC->SetTextAlign(TA_TOP | TA_CENTER);
		pDC->SelectObject(&headerFont);
		if( i == NDAYVALS - 1)
			str.Format("%d+", i + 1);
		else
			str.Format("%d", i + 1);
		pDC->TextOut(x, y0, str);
	}

	//output query unit names
	y = rect.top;
	x = faRect.left + (faRect.Width() / 2);
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	pDC->SelectObject(&headerFont);
	for(i = 0; i < m_pFiresSummary->nStrings; i++)
	{
		if(m_pFiresSummary->qStrings[i].GetLength() > 0)
		{
			pDC->TextOut(x, y, m_pFiresSummary->qStrings[i]);
			CSize sz = pDC->GetTextExtent(m_pFiresSummary->qStrings[i]);
			y += sz.cy + 1;
		}
	}
	//restore GDI objects
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldFont);
	pDC->SelectObject(oldBrush);
}

void CFireSumView::IdealFromYMax(long *fMax, double *aMax, long *fIdeal, double *aIdeal)
{
	if(*aMax <= 0)
		*aIdeal = 1.0;
	else
	{
		int power = (int)log10(*aMax / 4.0 );
		if(power > 0)
		{
			double step = floor( (*aMax / 4.0) / pow(10.0, power) );
			*aIdeal = floor( step * pow(10.0, power ));
		}
		else
			*aIdeal = max(*aMax / 4.0, 1);
	}
	if(*fMax <= 0)
		*fIdeal = 1;
	else
	{
		int power = (int)log10( ((double)*fMax) / 4.0 );
		if(power > 0)
		{
			double step = floor( ((double)*fMax / 4.0) / pow(10.0, power) );
			*fIdeal = (int)floor( step * pow(10.0, power ));
		}
		else
			*fIdeal = (int)max(*fMax / 4.0, 1);
	}
//	*aMax = 5 * *aIdeal;
//	*fMax = 5 * *fIdeal;
}
/////////////////////////////////////////////////////////////////////////////
// CFireSumView diagnostics

#ifdef _DEBUG
void CFireSumView::AssertValid() const
{
	CView::AssertValid();
}

void CFireSumView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFireSumView message handlers

void CFireSumView::OnFilePrint() 
{
	// TODO: Add your command handler code here
	/*PRINTDLG pd;
	theApp.GetPrinterDeviceDefaults(&pd);
	CPrintDialog pdlg(false);
	pdlg.m_pd.hDevNames = pd.hDevNames;
	pdlg.m_pd.hDevMode = pd.hDevMode;
	if(pdlg.DoModal() == IDOK)
	{
		theApp.SelectPrinter(pdlg.m_pd.hDevNames, pdlg.m_pd.hDevMode);
		CDC pDC;
		theApp.CreatePrinterDC(pDC);
		if(pDC)
		{
			DOCINFO docinfo;
			docinfo.cbSize = sizeof(DOCINFO);
			docinfo.lpszDocName = "FireFamily Plus Pocket Card";
			docinfo.lpszOutput = NULL;
			pDC.StartDoc(&docinfo);
			pDC.StartPage();
			int lx, ly;
			ly = pDC.GetDeviceCaps(VERTRES);
			lx = pDC.GetDeviceCaps(HORZRES);
			CRect bnd(1, 1,
				lx, ly);
			DrawToDCRegion(&pDC, bnd);
			pDC.EndPage();
			pDC.EndDoc();
		}
		HDC tDC = pDC.Detach();
		DeleteDC(tDC);
	}*/
	CView::OnFilePrint();
	
}

void CFireSumView::OnFileSaveAs() 
{
	chdir(theApp.userDir);
	CFileDialog fd(false, theApp.GetGraphicsExt(), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		theApp.GetGraphicsFileStr(), this);
	int saveWidth = 800, saveHeight = 600;
	if(fd.DoModal() == IDOK)
	{
		CDC *whdc = GetDC();
		CDC memhdc;
		memhdc.CreateCompatibleDC(NULL);
		HBITMAP picture = CreateCompatibleBitmap(whdc->m_hDC, saveWidth, saveHeight);
		HGDIOBJ Old = SelectObject(memhdc.m_hDC, picture);
		CRect rect(0, 0, saveWidth, saveHeight);
		DrawToDCRegion(&memhdc, rect);
		char fName[256];
		strcpy(fName, fd.GetPathName());
		SaveBitmapFile(memhdc.m_hDC, picture, fName); 
		SelectObject(memhdc.m_hDC, Old);
		ReleaseDC(whdc);
		memhdc.DeleteDC();
		DeleteObject(picture);
		theApp.SetUserDir(fd.GetPathName());
		theApp.SetGraphicsExt(fd.GetFileExt());
	}
	chdir(theApp.dbDir);//theApp.workDir);
}

void CFireSumView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
//	CFireSumDoc *pDoc = (CFireSumDoc *)GetDocument();
	yrSkip = 1;
	if(m_pFiresSummary->nYears <= 5)
		yrSkip = 1;
	else if(m_pFiresSummary->nYears <= 10)
		yrSkip = 2;
	else if(m_pFiresSummary->nYears <= 20)
		yrSkip = 4;
	else if(m_pFiresSummary->nYears <= 40)
		yrSkip = 5;
	//else if(m_pFiresSummary->nYears < 40)
	//	yrSkip = 4;
	else
		yrSkip = m_pFiresSummary->nYears / 5 + 1;
	totFires = firesMax = 0;
	totAcres = acresMax = 0;
	for(int i = 0; i < m_pFiresSummary->nYears; i++)
	{
		firesMax = max(firesMax, m_pFiresSummary->fires[i]);
		acresMax = max(acresMax, m_pFiresSummary->acres[i]);
		totFires += m_pFiresSummary->fires[i];
		totAcres += m_pFiresSummary->acres[i];
	}
	if(acresMax == 0.0)
		acresMax = 1.0;
	if(firesMax == 0)
		firesMax = 1;
}

BOOL CFireSumView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	
	return DoPreparePrinting(pInfo);
	//return CView::OnPreparePrinting(pInfo);
}

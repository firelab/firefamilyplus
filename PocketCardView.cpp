// PocketCardView.cpp : implementation file
//
 
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "wxSet.h"
#include "PocketCardDoc.h"
#include "PocketCardView.h"
#include <math.h>
#include <direct.h>
#include<shlwapi.h>
#include "FireDay.h"
#include "PeriodDoc.h"
#include <math.h>
#include "climateSet.h"
#include "SIGStationSet.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"
#include ".\pocketcardview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPocketCardView
extern CFireplusApp theApp;

IMPLEMENT_DYNCREATE(CPocketCardView, CScrollView)

CPocketCardView::CPocketCardView()
{
	extremeIsHigh = true;
}

CPocketCardView::~CPocketCardView()
{
}

//char *months[12] =
//{ "Jan", "Feb", "Mar", "Apr", "May", "June", "July", "Aug", "Sept", "Oct", "Nov", "Dec" };
char *months[12] =
{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

BEGIN_MESSAGE_MAP(CPocketCardView, CScrollView)
	//{{AFX_MSG_MAP(CPocketCardView)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
	//ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPocketCardView drawing
void CPocketCardView::PreDrawCalcs()
{
	m_nObs = 0;
	
	//if(!theApp.pCardDIB)
	//count years
	//calculate 97th percentile
	m_years = 0;
	Stats stats;
	//CPocketCardDoc* pDoc = (CPocketCardDoc*)GetDocument();
	//m_pAnalysis = pDoc->
	bool *yrsIn = new bool[m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][0]->years];
	for(int i = 0; i < m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][0]->years; i++)
		yrsIn[i] = false;
//	bool hasYear = false;
	for(int k = 0; k < m_pAnalysis->numPeriods; k++)
	{
		for(int y = 0; y < m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->years; y++)
		{
			for(int v = 0; v < m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->yStats[y].N(); v++)
			{
				stats.Accumulate(m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->yStats[y].vals[v]);
				//if(m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->N
			}
			if(m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->yStats[y].N() > 0 && !yrsIn[y])
			{
				yrsIn[y] = true;
				m_years++;
			}
		}
	}
	stats.Complete();
	delete[] yrsIn;
	pcntile97 = stats.Percentile(m_pAnalysis->m_pocketOpts.cp);
	m_nObs = stats.N();
	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = 900;
	sizeTotal.cy = 650;
	SetScrollSizes(MM_TEXT, sizeTotal);
	string1 = string2 = string3 = string4 = "";
	//now fill strings dependant on variable
	switch(m_pAnalysis->m_pocketOpts.varID + 1)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 23:
	case 24:
	case 25:
		string1.Format("%s gives day-to-day fluctuations.", m_pAnalysis->m_pocketOpts.m_IndexString);
		break;
	case 12:
		string1.Format("%s gives day-to-day fluctuations", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, and wind.");
		string4.Format("Wind is part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 13:
		string1.Format("%s gives seasonal trends", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity,");
		string3.Format("daily temperature & rh ranges, and precip duration.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 14:
		string1.Format("%s gives day-to-day fluctuations", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, wind,");
		string3.Format("daily temperature & rh ranges, and precip duration.");
		string4.Format("Wind is part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 15:
		string1.Format("%s gives seasonal trends", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature and precip amount.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 16:
		string1.Format("%s gives day-to-day fluctuations", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, SOW, and wind.");
		string4.Format("Wind is part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 17:
		string1.Format("%s gives day-to-day fluctuations", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, and SOW.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 18:
		string1.Format("%s gives day-to-day fluctuations", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, and SOW.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 19:
		string1.Format("%s gives seasonal trends", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from daily temp & rh ranges, and precip duration.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 20:
		string1.Format("%s gives seasonal trends", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from daily temp & rh ranges, and precip duration.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 21:
		string1.Format("%s gives seasonal trends", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from daily temp & rh ranges, and precip duration.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 22:
		string1.Format("%s gives seasonal trends", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from daily temp & rh ranges, and precip duration.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	/*case 23:
		string1.Format("%s gives seasonal trends", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from daily temp & rh ranges, and precip duration.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;*/
	case 26:
		string1.Format("%s gives day-to-day fluctuations", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, wind, and precip amount.");
		string4.Format("Wind is part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 27:
		string1.Format("%s gives seasonal trends", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, and precip amount.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 28:
		string1.Format("%s gives seasonal trends", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature and precip amount.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 29:
		string1.Format("%s gives day-to-day fluctuations", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, wind, and precip amount.");
		string4.Format("Wind is part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 30:
		string1.Format("%s gives seasonal trends", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, and precip amount.");
		string4.Format("Wind is NOT part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 31:
		string1.Format("%s gives day-to-day fluctuations", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, wind, and precip amount.");
		string4.Format("Wind is part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 32:
		string1.Format("%s gives day-to-day fluctuations", m_pAnalysis->m_pocketOpts.m_IndexString);
		string2.Format("calculated from temperature, humidity, wind, and precip amount.");
		string4.Format("Wind is part of %s calculation.", m_pAnalysis->m_pocketOpts.m_IndexShort);
		break;
	case 33:
		string1.Format("%s gives day-to-day fluctuations.", m_pAnalysis->m_pocketOpts.m_IndexString);
		break;
	case 34:
		string1.Format("%s gives day-to-day fluctuations.", m_pAnalysis->m_pocketOpts.m_IndexString);
		break;
	case 35:
		string1.Format("%s gives day-to-day fluctuations.", m_pAnalysis->m_pocketOpts.m_IndexString);
		break;
	case 36:
		string1.Format("%s indicates temperature reading type.", m_pAnalysis->m_pocketOpts.m_IndexString);
		break;

	}
	switch(m_pAnalysis->m_pocketOpts.varID + 1)
	{
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 25:
	case 36:
	case 37:
	case 42:
		extremeIsHigh = false;
		break;
	default:
		extremeIsHigh = true;

	}
}

void CPocketCardView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	PreDrawCalcs();
}

void CPocketCardView::OnDraw(CDC* pDC)
{
	CRect rect;
	if(!pDC->IsPrinting())
	{
		rect.left = 0;
		rect.top = 0;
		rect.bottom = 600;
		rect.right = 850;
	}
	else
	{
		pDC->GetBoundsRect(&rect, DCB_RESET);
		//rect.NormalizeRect();
	}
//	if(rect.Width() == 0 || rect.Height() == 0)
//		return;
	DrawToDCRegion(pDC, rect);
}

void CPocketCardView::DrawToDCRegion(CDC * pDC, CRect rect)
{

	bool inverted = false;
	CPocketCardDoc* pDoc = (CPocketCardDoc*)GetDocument();
	//check for inverted graph types RH's and Fuel Moistures
	if((m_pAnalysis->m_pocketOpts.varID >= 4 && m_pAnalysis->m_pocketOpts.varID <= 7)
		|| (m_pAnalysis->m_pocketOpts.varID >= 16 && m_pAnalysis->m_pocketOpts.varID <= 22))
		inverted = true;
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
	CRect r1(rect.left, rect.top, rect.left + rect.Width() / 2,
			rect.top + rect.Height() / 2),
		r2(rect.left + rect.Width() / 2, rect.top, rect.right,
			rect.top + rect.Height() / 2),
		r3(rect.left, rect.top + rect.Height() / 2, rect.left + rect.Width() / 2,
			rect.bottom),
		r4(rect.left + rect.Width() / 2, rect.top + rect.Height() / 2,
			rect.right, rect.bottom);
	// TODO: add draw code here
	CPen dotPen, redPen, bluePen, blackPen, blackPen2, pcntPen, *pOldPen;
	if(!dotPen.CreatePen( PS_DOT, 1, RGB(0,0,0))
		|| !pcntPen.CreatePen( PS_DASH, 1, RGB(0xEE,0x00,0))
		|| !blackPen.CreatePen( PS_SOLID, 1, RGB(0,0,0))
		|| !blackPen2.CreatePen( PS_SOLID, 2, RGB(0,0,0))
		|| !redPen.CreatePen( PS_SOLID, 1, RGB(0xCC,0,0))
		|| !bluePen.CreatePen( PS_SOLID, 1, RGB(0,0,0xCC)))
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Resources too low for creating pocket card pens.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CFont bigFont, monthFont, normalBoldFont, normalFont, normalVFont;
	if(!bigFont.CreateFont( r1.Height() / 20, r1.Width() / 45, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")
		//|| !monthFont.CreateFont(r1.Height() / 20, r1.Width() / 55, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
		|| !monthFont.CreateFont(r1.Height() / 20, r1.Width() / 65, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")
		|| !normalBoldFont.CreateFont(r1.Height() / 25, r1.Width() / 80, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")
		|| !normalFont.CreateFont(r1.Height() / 25, r1.Width() / 80, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica") 
		|| !normalVFont.CreateFont(r1.Height() / 25, r1.Width() / 80, 900, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica") )
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create pocket card fonts.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CBrush whiteBrush, blackBrush;
	if(!whiteBrush.CreateSolidBrush(0x00FFFFFF) || !blackBrush.CreateSolidBrush(0x00000000))
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create pocket card brushes.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CBrush *pOldBrush = pDC->SelectObject(&whiteBrush);
	pOldPen = pDC->SelectObject(&blackPen);
	pDC->Rectangle(rect);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.top);
	pDC->LineTo(rect.left, rect.top);
 
	//pOldPen = 
	pDC->SelectObject(&dotPen);
	pDC->MoveTo(r1.left, r4.top);
	pDC->LineTo(r4.right, r4.top);
	pDC->MoveTo(r1.right, r1.top);
	pDC->LineTo(r1.right, r4.bottom);
	int legendRight, legendHeight;
	CRect g1(r1.left + r1.Width() / 9, r1.top + r1.Height() / 7, 
			r1.left + r1.Width() / 9 * 8, r1.top + r1.Height() / 7 * 6),
		g2(r3.left + r3.Width() / 9, r3.top + r3.Height() / 7, 
			r3.left + r3.Width() / 9 * 8, r3.top + r3.Height() / 7 * 6),
		slRect(r2.left + r2.Width() / 9, r2.top + r2.Height() / 7 * 2, 
			r2.left + r2.Width() / 9 + r2.Width() / 10, 
			r2.top + r2.Height() / 7 * 2 + r2.Height() / 5),
		ckRect(r4.left + r4.Width() / 9, r4.top + r4.Height() / 7 * 2, 
			r4.left + r4.Width() / 9 + r4.Width() / 40, 
			r4.top + r4.Height() / 7 * 2 + r4.Height() / 25);
	int rasterCaps = pDC->GetDeviceCaps(RASTERCAPS);
	//if (rasterCaps & RC_STRETCHBLT)
	//{
		//output color bitmaps
		CRect ryrect, gyrect, yrect;
		int t1 = g1.top, t2 = g2.top;
		if(m_pAnalysis->m_pocketOpts.ryPcnt > 0)
		{
			ryrect = g1;
			if(!inverted)
			{
				ryrect.bottom = g1.top + g1.Height() * m_pAnalysis->m_pocketOpts.ryPcnt / 100;
				t1 = ryrect.bottom;
				PaintDIB(pDC->m_hDC, ryrect, theApp.pCardRYDIB, theApp.pCardRYRect, NULL);//theApp.pCardRYPal);
			}
			else
			{
				ryrect.top = g1.bottom - g1.Height() * m_pAnalysis->m_pocketOpts.ryPcnt / 100;
				t1 = ryrect.top;
				PaintDIB(pDC->m_hDC, ryrect, theApp.pCardYRDIB, theApp.pCardYRRect, NULL);//theApp.pCardYRPal);
			}
			//t1 = ryrect.bottom;
			ryrect = g2;
			if(!inverted)
			{
				ryrect.bottom = g2.top + g2.Height() * m_pAnalysis->m_pocketOpts.ryPcnt / 100;
				t2 = ryrect.bottom;
				PaintDIB(pDC->m_hDC, ryrect, theApp.pCardRYDIB, theApp.pCardRYRect, NULL);//theApp.pCardRYPal);
			}
			else
			{
				ryrect.top = g2.bottom - g2.Height() * m_pAnalysis->m_pocketOpts.ryPcnt / 100;
				t2 = ryrect.top;
				PaintDIB(pDC->m_hDC, ryrect, theApp.pCardYRDIB, theApp.pCardYRRect, NULL);//theApp.pCardYRPal);
			}
			//t2 = ryrect.bottom;
		}
		if(m_pAnalysis->m_pocketOpts.yPcnt > 0)
		{
			yrect = g1;
			if(!inverted)
			{
				yrect.top = t1;
				yrect.bottom = yrect.top + g1.Height() * m_pAnalysis->m_pocketOpts.yPcnt / 100;
				t1 = yrect.bottom;
			}
			else
			{
				yrect.bottom = t1;
				yrect.top = yrect.bottom - g1.Height() * m_pAnalysis->m_pocketOpts.yPcnt / 100;
				t1 = yrect.top;
			}
			PaintDIB(pDC->m_hDC, yrect, theApp.pCardYDIB, theApp.pCardYRect, NULL);//theApp.pCardYPal);
			yrect = g2;
			if(!inverted)
			{
				yrect.top = t2;
				yrect.bottom = yrect.top + g2.Height() * m_pAnalysis->m_pocketOpts.yPcnt / 100;
				t2 = yrect.bottom;
			}
			else
			{
				yrect.bottom = t2;
				yrect.top = yrect.bottom - g2.Height() * m_pAnalysis->m_pocketOpts.yPcnt / 100;
				t2 = yrect.top;
			}
			PaintDIB(pDC->m_hDC, yrect, theApp.pCardYDIB, theApp.pCardYRect, NULL);//theApp.pCardYPal);
		}
		if(m_pAnalysis->m_pocketOpts.gyPcnt > 0)
		{
			gyrect = g1;
			if(!inverted)
			{
				gyrect.top = t1;
				PaintDIB(pDC->m_hDC, gyrect, theApp.pCardGYDIB, theApp.pCardGYRect, NULL);//theApp.pCardGYPal);
			}
			else
			{
				gyrect.bottom = t1;
				PaintDIB(pDC->m_hDC, gyrect, theApp.pCardYGDIB, theApp.pCardYGRect, NULL);//theApp.pCardYGPal);
			}
			//gyrect.bottom = gyrect.top + g1.Height() * theApp.pcGyVal / 100;
			gyrect = g2;
			if(!inverted)
			{
				gyrect.top = t2;
				PaintDIB(pDC->m_hDC, gyrect, theApp.pCardGYDIB, theApp.pCardGYRect, NULL);//theApp.pCardGYPal);
			}
			else
			{
				gyrect.bottom = t2;
				PaintDIB(pDC->m_hDC, gyrect, theApp.pCardYGDIB, theApp.pCardYGRect, NULL);//theApp.pCardYGPal);
			}
			//gyrect.bottom = gyrect.top + g2.Height() * theApp.pcGyVal / 100;
		}
		//PaintDIB(pDC->m_hDC, g1, theApp.pCardDIB, theApp.pCardRect, theApp.pCardPal);
		//PaintDIB(pDC->m_hDC, g2, theApp.pCardDIB, theApp.pCardRect, theApp.pCardPal);
	// 2014: we removed the stoplight
    //
	//	PaintDIB(pDC->m_hDC, slRect, theApp.stoplightDIB, theApp.stoplightRect, NULL);//theApp.stoplightPal);
//	}
	legendRight = r1.left + r1.Width() / 9 * 8 + r1.Width() / 30;//bm.bmWidth + 50 + 15;
	legendHeight = r1.Height() / 7 * 5;//bm.bmHeight + 50;
	pDC->SelectObject(&blackPen);
	pDC->MoveTo(g1.left, g1.top);
	pDC->LineTo(g1.left, g1.bottom);
	pDC->LineTo(g1.right, g1.bottom);
	pDC->LineTo(g1.right, g1.top);
	pDC->LineTo(g1.left, g1.top);

	pDC->MoveTo(g2.left, g2.top);
	pDC->LineTo(g2.left, g2.bottom);
	pDC->LineTo(g2.right, g2.bottom);
	pDC->LineTo(g2.right, g2.top);
	pDC->LineTo(g2.left, g2.top);

	//now start outputting text blocks
	//start with vertical text on graphs
	CFont *pOldFont = pDC->SelectObject(&normalVFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);
	CString tStr;
	pDC->TextOut(r1.left + r1.Width() / 30, r1.top + r1.Height() / 2, m_pAnalysis->m_pocketOpts.m_IndexString);
	pDC->TextOut(r3.left + r3.Width() / 30, r3.top + r3.Height() / 2, m_pAnalysis->m_pocketOpts.m_IndexString);
	if(extremeIsHigh)
		tStr = "Moderate";
	else
		tStr = "Extreme";
	pDC->TextOut(legendRight, legendHeight, tStr); 
	pDC->TextOut(legendRight, legendHeight + r3.top, tStr); 
	if(extremeIsHigh)
		tStr = "Extreme";
	else
		tStr = "Moderate";
	pDC->TextOut(legendRight, legendHeight / 3, tStr); 
	pDC->TextOut(legendRight, legendHeight / 3 + r3.top, tStr); 

	//now do r1
	pDC->SelectObject(&bigFont);
	tStr.Format("FIRE DANGER -- %s", m_pAnalysis->m_pocketOpts.m_Area);
	int t = r1.top + r1.Height() / 16;
	pDC->TextOut(r1.left + r1.Width() / 2, t, tStr);
	CSize tSize = pDC->GetTextExtent(tStr);
	t += tSize.cy;
	tStr.Format("Maximum, Average, and %.0fth Percentile, based on %d years data", m_pAnalysis->m_pocketOpts.cp * 100.0, m_years);
	pDC->SelectObject(&normalFont);
	tSize = pDC->GetTextExtent(tStr);
	int l = r1.left + r1.Width() / 2 - tSize.cx / 2;
	int b = t + tSize.cy / 4;
	pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
	pDC->SetTextColor(0x00000000);
	tStr = inverted ? "Minimum" : "Maximum";
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	pDC->SelectObject(&blackPen);
	pDC->MoveTo(l, b);
	pDC->LineTo(l + tSize.cx, b);
	l += tSize.cx;
	tStr = ", ";
	pDC->SetTextColor(0x00000000);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	l += tSize.cx;
	tStr = "Average";
	pDC->SetTextColor(0x00CC0000);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	pDC->SelectObject(&bluePen);
	pDC->MoveTo(l, b);
	pDC->LineTo(l + tSize.cx, b);
	l += tSize.cx;
	tStr = ", and ";
	pDC->SetTextColor(0x00000000);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	l += tSize.cx;
	tStr.Format("%.0fth Percentile", m_pAnalysis->m_pocketOpts.cp * 100.0);
	pDC->SetTextColor(0x000000EE);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	pDC->SelectObject(&pcntPen);
	pDC->MoveTo(l, b);
	pDC->LineTo(l + tSize.cx, b);
	l += tSize.cx;
	//years of data
	if(m_years < 10)
	{
		tStr = ", based on ";
		pDC->SetTextColor(0x00000000);
		pDC->TextOut(l, t, tStr);
		tSize = pDC->GetTextExtent(tStr);
		l += tSize.cx;
		tStr.Format("%d", m_years);
		pDC->SetTextColor(0x000000EE);
		pDC->TextOut(l, t, tStr);
		tSize = pDC->GetTextExtent(tStr);
		l += tSize.cx;
		tStr = " years data";
		pDC->SetTextColor(0x00000000);
		pDC->TextOut(l, t, tStr);
	}
	else
	{
		tStr.Format(", based on %d years data", m_years);
		pDC->SetTextColor(0x00000000);
		pDC->TextOut(l, t, tStr);
	}

	//ok, now do r3
	tStr = "Years to Remember:";
	pDC->SelectObject(&bigFont);
	tSize = pDC->GetTextExtent(tStr + " 0000  0000");
	l = r3.left + r3.Width() / 2 - tSize.cx / 2;
	t = r3.top + r3.Height() / 10;
	pDC->SetTextColor(0x00000000);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	l += tSize.cx;

	tSize = pDC->GetTextExtent("  ");
	l += tSize.cx;
	tStr.Format("%d", m_pAnalysis->m_pocketOpts.m_Year1);
	pDC->SetTextColor(0x00CC0000);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	pDC->SelectObject(&bluePen);
	pDC->MoveTo(l, t + tSize.cy / 3);
	pDC->LineTo(l + tSize.cx, t + tSize.cy / 3);
	if(m_pAnalysis->m_pocketOpts.m_Year1 != m_pAnalysis->m_pocketOpts.m_Year2)
	{
		l += tSize.cx;
		tSize = pDC->GetTextExtent("  ");
		l += tSize.cx;
		//tSize = pDC->GetTextExtent(tStr);
		tStr.Format("%d", m_pAnalysis->m_pocketOpts.m_Year2);
		pDC->SetTextColor(0x000000CC);
		pDC->TextOut(l, t, tStr);
		tSize = pDC->GetTextExtent(tStr);
		pDC->SelectObject(&redPen);
		pDC->MoveTo(l, t + tSize.cy / 3);
		pDC->LineTo(l + tSize.cx, t + tSize.cy / 3);
	}
	if(m_pAnalysis->m_pocketOpts.ytdPeriods)
	{//output year
		l += tSize.cx;
		tSize = pDC->GetTextExtent("  ");
		l += tSize.cx;
		//tSize = pDC->GetTextExtent(tStr);
		tStr.Format("%d", m_pAnalysis->m_pocketOpts.ytdPeriods[m_pAnalysis->m_pocketOpts.varID][0]->baseYear);
		pDC->SetTextColor(0x00000000);
		pDC->TextOut(l, t, tStr);
		tSize = pDC->GetTextExtent(tStr);
		pDC->SelectObject(&blackPen2);
		pDC->MoveTo(l, t + tSize.cy / 3);
		pDC->LineTo(l + tSize.cx, t + tSize.cy / 3);
	}
	if(m_pAnalysis->m_pocketOpts.m_FuelModel.GetLength() > 0)
	{
		tStr.Format("Fuel Model: %s", m_pAnalysis->m_pocketOpts.m_FuelModel);
		pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
		pDC->SetTextColor(0x00000000);
		pDC->TextOut(r3.left + r3.Width() / 2, r3.bottom - r3.Height() / 30, tStr);
	}
	//figure extent of values
	int cnt = 0, x, y;
	double ymax = 1, ymin = 0.0;
	int k;
	for(k = 0; k < m_pAnalysis->numPeriods; k++)
	{
		if(m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->N() > 0)
		{
			ymax = max(ymax, m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->max);
			ymin = min(ymin, m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->min);
		}
		if(m_pAnalysis->m_pocketOpts.ytdPeriods)
		{
			if(m_pAnalysis->m_pocketOpts.ytdPeriods[m_pAnalysis->m_pocketOpts.varID][k]->N() > 0)
			{
				ymax = max(ymax, m_pAnalysis->m_pocketOpts.ytdPeriods[m_pAnalysis->m_pocketOpts.varID][k]->max);
				ymin = min(ymin, m_pAnalysis->m_pocketOpts.ytdPeriods[m_pAnalysis->m_pocketOpts.varID][k]->min);
			}
		}
	}
	ymax *= 1.1;
	//now output 97th percentile across graphs..
	pDC->SelectObject(&pcntPen);
	y = (int)(g1.bottom - (pcntile97 - ymin) / (ymax - ymin) * (double)(g1.Height())); 
	pDC->MoveTo(g1.left, y);
	pDC->LineTo(g1.right, y);
	pDC->MoveTo(g2.left, y + r1.Height());
	pDC->LineTo(g2.right, y + r1.Height());
	//now ready to output graphs in r1 and r3, overlaid on bitmaps,
	//and the x and y axis values
	double step;
	if(m_pAnalysis->numPeriods > 1)
		step = (double)(g1.Width()) / (double)(m_pAnalysis->numPeriods - 1);
	else
		step = (g1.Width()) / 2;
	//pDC->SelectObject(&redPen);
	pDC->SelectObject(&blackPen);
	//pDC->MoveTo(g1.left, g1.bottom);
	//int loc = pDoc->overLayYears[i] - m_pAnalysis->periods[varID][0]->baseYear;
	//do max's or min's
	for(k = 0; k < m_pAnalysis->numPeriods; k++)
	{
		if(m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->N() > 0L)
		{
			x = (int)(g1.left + k * step);
			double tval;
			if(inverted)
				tval = m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->min;
			else
				tval = m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->max;
			y = (int)(g1.bottom - (tval - ymin)/ (ymax - ymin) * (double)(g1.Height()));//y0 - y1);
			if(!cnt)
			{
				pDC->MoveTo(x, y);
				cnt++;
			}
			else
				pDC->LineTo(x, y);
		}		
		else
			cnt = 0;
	}
	//do means
	pDC->SelectObject(&bluePen);
	cnt = 0;
	for(k = 0; k < m_pAnalysis->numPeriods; k++)
	{
		if(m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->N() > 0)
		{
			x = (int)(g1.left + k * step);
			y = (int)(g1.bottom - (m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->mean - ymin) / (ymax - ymin)* (double)(g1.Height()));//y0 - y1);
			if(!cnt)
			{
				pDC->MoveTo(x, y);
				cnt++;
			}
			else
				pDC->LineTo(x, y);
		}
		else
			cnt = 0;
	}

	//do first year to remember
	int loc = m_pAnalysis->m_pocketOpts.m_Year1 - m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][0]->baseYear;
	cnt = 0;
	for(k = 0; k < m_pAnalysis->numPeriods; k++)
	{
		if(m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->yStats[loc].N() > 0)
		{
			x = (int)(g2.left + k * step);
			y = (int)(g2.bottom - (m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->yStats[loc].Mean() - ymin) / (ymax - ymin) * (double)(g2.Height()));//y0 - y1);
			if(!cnt)
			{
				pDC->MoveTo(x, y);
				cnt++;
			}
			else
				pDC->LineTo(x, y);
		}		
		else
			cnt = 0;
	}
	//do second year to remember
	if(m_pAnalysis->m_pocketOpts.m_Year1 != m_pAnalysis->m_pocketOpts.m_Year2)
	{
		loc = m_pAnalysis->m_pocketOpts.m_Year2 - m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][0]->baseYear;
		pDC->SelectObject(&redPen);
		cnt = 0;
		for(k = 0; k < m_pAnalysis->numPeriods; k++)
		{
			if(m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->yStats[loc].N() > 0)
			{
				x = (int)(g2.left + k * step);
				y = (int)(g2.bottom - (m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->yStats[loc].Mean() - ymin) / (ymax - ymin) * (double)(g2.Height()));//y0 - y1);
				if(!cnt)
				{
					pDC->MoveTo(x, y);
					cnt++;
				}
				else
					pDC->LineTo(x, y);
			}
			else
				cnt = 0;
		}
	}
	//do year to date line if selected
	if(m_pAnalysis->m_pocketOpts.ytdPeriods)
	{
		loc = m_pAnalysis->m_pocketOpts.m_Year2 - m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][0]->baseYear;
		pDC->SelectObject(&blackPen2);
		cnt = 0;
		for(k = 0; k < m_pAnalysis->numPeriods; k++)
		{
			if(m_pAnalysis->m_pocketOpts.ytdPeriods[m_pAnalysis->m_pocketOpts.varID][k]->N() > 0)
			{
				x = (int)(g2.left + k * step);
				y = (int)(g2.bottom - (m_pAnalysis->m_pocketOpts.ytdPeriods[m_pAnalysis->m_pocketOpts.varID][k]->mean - ymin) / (ymax - ymin) * (double)(g2.Height()));//y0 - y1);
				if(!cnt)
				{
					pDC->MoveTo(x, y);
					cnt++;
				}
				else
					pDC->LineTo(x, y);
			}
			else
				cnt = 0;
		}
	}
	//lines drawn, now output axis values
	pDC->SelectObject(&normalFont);
	pDC->SelectObject(&blackPen);
	pDC->SetTextColor(0x00000000);
	pDC->SetTextAlign(TA_BASELINE | TA_RIGHT);
	double ideal = IdealFromYMax(ymax - ymin);
	y = g1.bottom;
	x = 0;
	int z = 0;
	while((z * ideal) > ymin)
		z--;
	x = z;
	while( y >= g1.top )
	{
		if(ideal < 1.0)
			tStr.Format("%.1f ", x * ideal);
		else if(ideal < 10)
			tStr.Format("%.0f ", x * ideal);
		else
			tStr.Format("%.0f ", x * ideal);
		pDC->MoveTo(g1.left - (g1.left - r1.left)/10, y);
		pDC->LineTo(g1.left, y);
		pDC->TextOut(g1.left/* - (g1.left - r1.left)/10*/, y, tStr);
		pDC->MoveTo(g2.left - (g2.left - r3.left)/10, y + r1.Height());
		pDC->LineTo(g2.left, y + r1.Height());
		pDC->TextOut(g2.left/* - (g2.left - r3.left)/10*/, y + r1.Height(), tStr);
		x++;
		//y = g1.bottom - (int)(( x * ideal / ymax) * (double)(g1.Height()));
		y = g1.bottom - (int)(( (x - z) * ideal / (ymax - ymin)) * (double)(g1.Height()));
	}
	//ok, now both need months
	pDC->SetTextAlign(TA_TOP | TA_LEFT);
	pDC->SelectObject(&monthFont);
	//grrr count months
	int nMonths = 0;
	for(k = 0; k < m_pAnalysis->numPeriods; k++)
	{
		if(k == 0 || m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetDay() == 1)
			nMonths++;
	}
	for(k = 0; k < m_pAnalysis->numPeriods; k++)
	{
		if(k == 0 || m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetDay() == 1)
		{
			if(k == 0 && m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetDay() >= 10)
				pDC->SetTextAlign(TA_TOP | TA_RIGHT);
			tStr.Format("%s", months[m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetMonth() - 1]);
			l = (int)(g1.left + k * step);
			pDC->TextOut(l, g1.bottom + 4, tStr);
			pDC->TextOut(l, g2.bottom + 4, tStr);
			pDC->MoveTo(l, g1.bottom);
			pDC->LineTo(l, g1.bottom + 4);
			pDC->MoveTo(l, g2.bottom);
			pDC->LineTo(l, g2.bottom + 4);
			if(k == 0 && m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetDay() >= 10)
				pDC->SetTextAlign(TA_TOP | TA_LEFT);
		}
	}
	//now put any fires onto bottom graph
	pDC->SelectObject(&bigFont);
	pDC->SetTextAlign(TA_BOTTOM | TA_CENTER);
	if(m_pAnalysis->m_pocketOpts.m_Fire1.GetLength() > 0 )
	{
		for(k = 0; k < m_pAnalysis->numPeriods; k++)
		{
			if(m_pAnalysis->m_pocketOpts.m_Fire1Date.GetMonth() == m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetMonth()
				&& m_pAnalysis->m_pocketOpts.m_Fire1Date.GetDay() >= m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetDay()
				&& m_pAnalysis->m_pocketOpts.m_Fire1Date.GetDay() <= m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->end.GetDay())
				break;
		}
		if(k < m_pAnalysis->numPeriods)
		{
			int starSize = g2.Height() / 32;
			x = (int)(g2.left + k * step);
			y = (int)(g2.bottom - (m_pAnalysis->m_pocketOpts.m_Fire1Val - ymin) / (ymax - ymin)* (double)(g2.Height()));
			Star(pDC, x, y, starSize);
			pDC->TextOut(x, y - starSize, m_pAnalysis->m_pocketOpts.m_Fire1);
		}
	}
	if(m_pAnalysis->m_pocketOpts.m_Fire2.GetLength() > 0 )
	{
		for(k = 0; k < m_pAnalysis->numPeriods; k++)
		{
			if(m_pAnalysis->m_pocketOpts.m_Fire2Date.GetMonth() == m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetMonth()
				&& m_pAnalysis->m_pocketOpts.m_Fire2Date.GetDay() >= m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetDay()
				&& m_pAnalysis->m_pocketOpts.m_Fire2Date.GetDay() <= m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->end.GetDay())
				break;
		}
		if(k < m_pAnalysis->numPeriods)
		{
			int starSize = g2.Height() / 32;
			x = (int)(g2.left + k * step);
			y = (int)(g2.bottom - (m_pAnalysis->m_pocketOpts.m_Fire2Val - ymin) / (ymax - ymin) * (double)(g2.Height()));
			Star(pDC, x, y, starSize);
			pDC->TextOut(x, y - starSize, m_pAnalysis->m_pocketOpts.m_Fire2);
		}
	}
	if(m_pAnalysis->m_pocketOpts.m_Fire3.GetLength() > 0 )
	{
		for(k = 0; k < m_pAnalysis->numPeriods; k++)
		{
			if(m_pAnalysis->m_pocketOpts.m_Fire3Date.GetMonth() == m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetMonth()
				&& m_pAnalysis->m_pocketOpts.m_Fire3Date.GetDay() >= m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->start.GetDay()
				&& m_pAnalysis->m_pocketOpts.m_Fire3Date.GetDay() <= m_pAnalysis->periods[m_pAnalysis->m_pocketOpts.varID][k]->end.GetDay())
				break;
		}
		if(k < m_pAnalysis->numPeriods)
		{
			int starSize = g2.Height() / 32;
			x = (int)(g2.left + k * step);
			y = (int)(g2.bottom - (m_pAnalysis->m_pocketOpts.m_Fire3Val - ymin) / (ymax - ymin)* (double)(g2.Height()));
			Star(pDC, x, y, starSize);
			pDC->TextOut(x, y - starSize, m_pAnalysis->m_pocketOpts.m_Fire3);
		}
	}
	//now do r2
	pDC->SelectObject(&bigFont);
	pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
	tStr = "Fire Danger Area:";
	t = r2.top + r2.Height() / 17;
	pDC->TextOut(r2.left + r2.Width() / 12, t, tStr);
	if(m_pAnalysis->m_pocketOpts.m_Line1.GetLength() > 0 || m_pAnalysis->m_pocketOpts.m_Line2.GetLength() > 0 
		|| m_pAnalysis->m_pocketOpts.m_Line3.GetLength() > 0)
	{
		l = r2.left + r2.Width() / 10;// + tSize.cy
		tSize = pDC->GetTextExtent(tStr);
		t += tSize.cy;// * 3 / 2;
		pDC->SelectObject(&normalFont);
		if(m_pAnalysis->m_pocketOpts.m_Line1.GetLength() > 0)
		{
			tSize = pDC->GetTextExtent(m_pAnalysis->m_pocketOpts.m_Line1);
			t -= tSize.cy / 2;
			pDC->SelectObject(&blackPen);
			pDC->SelectObject(&blackBrush);
			pDC->Ellipse(l - tSize.cy / 4, t - tSize.cy / 4,
				l + tSize.cy / 4, t + tSize.cy / 4);
			pDC->TextOut(l + tSize.cy, t + tSize.cy / 4, m_pAnalysis->m_pocketOpts.m_Line1);
			t += tSize.cy * 3 / 2;
		}
		if(m_pAnalysis->m_pocketOpts.m_Line2.GetLength() > 0)
		{
			tSize = pDC->GetTextExtent(m_pAnalysis->m_pocketOpts.m_Line2);
			t -= tSize.cy / 2;
			pDC->SelectObject(&blackPen);
			pDC->SelectObject(&blackBrush);
			pDC->Ellipse(l - tSize.cy / 4, t - tSize.cy / 4,
				l + tSize.cy / 4, t + tSize.cy / 4);
			pDC->TextOut(l + tSize.cy, t + tSize.cy / 4, m_pAnalysis->m_pocketOpts.m_Line2);
			t += tSize.cy * 3 / 2;
		}
		if(m_pAnalysis->m_pocketOpts.m_Line3.GetLength() > 0)
		{
			tSize = pDC->GetTextExtent(m_pAnalysis->m_pocketOpts.m_Line3);
			t -= tSize.cy / 2;
			pDC->SelectObject(&blackPen);
			pDC->SelectObject(&blackBrush);
			pDC->Ellipse(l - tSize.cy / 4, t - tSize.cy / 4,
				l + tSize.cy / 4, t + tSize.cy / 4);
			pDC->TextOut(l + tSize.cy, t + tSize.cy / 4, m_pAnalysis->m_pocketOpts.m_Line3);
			t += tSize.cy * 3 / 2;
		}
		if(m_pAnalysis->m_pocketOpts.m_NWCGStd == false)
		{
			CString stdStr = "* Does NOT Meet NWCG Wx Station Standards";
			tSize = pDC->GetTextExtent(stdStr);
			t -= tSize.cy / 2;
			pDC->SetTextColor(0x000000EE);
			pDC->TextOut(l + tSize.cy, t + tSize.cy / 4, stdStr);
		}
		else
		{
			CString stdStr = "* Meets NWCG Wx Station Standards";
			tSize = pDC->GetTextExtent(stdStr);
			t -= tSize.cy / 2;
			pDC->SetTextColor(0x00000000);
			pDC->TextOut(l + tSize.cy, t + tSize.cy / 4, stdStr);
		}
	}
	pDC->SetTextColor(0x00000000);
	//output area bitmap if there is one
	if(m_pAnalysis->m_pocketOpts.areaDIB)
	{
		CRect areaRect;
		areaRect.left = r2.left + r2.Width() / 3 * 2;
		areaRect.right = r2.right - 1;
		areaRect.top = r2.top + 1;
		areaRect.bottom = slRect.top;
		double dibAR = (double) m_pAnalysis->m_pocketOpts.areaRect.Height() / (double) m_pAnalysis->m_pocketOpts.areaRect.Width();
		double trgAR = (double) areaRect.Height() / (double) areaRect.Width();
		if(dibAR < trgAR)//adjust target height to maintain dib's aspect ratio
		{
			int H = dibAR * areaRect.Width();
			areaRect.bottom = areaRect.top + H;
		}
		else if(dibAR > trgAR && dibAR > 0)//adjust target width to maintain dib's aspect ratio
		{
			int W = areaRect.Height() / dibAR;
			areaRect.right = areaRect.left + W;
		}
		PaintDIB(pDC->m_hDC, areaRect, m_pAnalysis->m_pocketOpts.areaDIB, m_pAnalysis->m_pocketOpts.areaRect, NULL);//pDoc->areaPal);
	}
	//interpretation section - assume stoplight bitmap was OK
	pDC->SelectObject(&blackPen);
	pDC->SelectObject(&bigFont);
	pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
	tStr = "Fire Danger Interpretation:";
	pDC->TextOut(r2.left + r2.Width() / 12, slRect.top, tStr);
	pDC->SelectObject(&normalFont);
	l = slRect.left ;
	t = slRect.top + slRect.Height() / 4;
	tStr.Format("EXTREME");
	pDC->SetTextColor(0x000000CC);
	pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	//l += tSize.cy + tSize.cy / 7;
	pDC->SetTextColor(0x00000000);
	tStr.Format("-- Use extreme caution");
	pDC->TextOut(l + tSize.cx + tSize.cx / 7, t, tStr);
	t += slRect.Height() / 4;
	tStr.Format("High");
	pDC->SetTextColor(0x0000CCCC);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	pDC->SetTextColor(0x00000000);
	tStr.Format("-- Watch for change");
	pDC->TextOut(l + tSize.cx + tSize.cx / 7, t, tStr);
	t += slRect.Height() / 4;
	tStr.Format("Moderate");
	pDC->SetTextColor(0x0000AA00);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	pDC->SetTextColor(0x00000000);
	tStr.Format("-- Lower Potential, but always be aware");
	pDC->TextOut(l + tSize.cx + tSize.cx / 7, t, tStr);

	l = slRect.left;
	t = slRect.bottom + 2 * tSize.cy;
	tStr.Format("%s", inverted ? "Minimum" : "Maximum");
	//pDC->SetTextColor(0x000000CC);
	pDC->SetTextColor(0x00000000);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	l += tSize.cx + tSize.cx / 7;
	if(m_pAnalysis->m_pocketOpts.periodLength > 1)
		tStr.Format("-- %s %s by %d day period", 
		inverted ? "Lowest" : "Highest", 
		m_pAnalysis->m_pocketOpts.m_IndexString, 
		m_pAnalysis->m_pocketOpts.periodLength);
	else
		tStr.Format("-- %s %s by day", 
		inverted ? "Lowest" : "Highest", 
		m_pAnalysis->m_pocketOpts.m_IndexString);
	pDC->SetTextColor(0x00000000);
	pDC->TextOut(l, t, tStr);
	tStr.Format("for %d - %d", m_pAnalysis->m_pocketOpts.startYear, m_pAnalysis->m_pocketOpts.endYear);
	t += tSize.cy + 1;
	pDC->TextOut(slRect.left, t, tStr);
	t += tSize.cy + 1;
	l = slRect.left;
	tStr.Format("Average");
	pDC->SetTextColor(0x00CC0000);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	l += tSize.cx + tSize.cx / 7;
	tStr.Format("-- shows peak fire season over %d years (%ld observations)",
		m_years, m_nObs);
	pDC->SetTextColor(0x00000000);
	pDC->TextOut(l, t, tStr);
	t += tSize.cy + 1;
	l = slRect.left;
	pDC->SetTextColor(0x000000EE);
	tStr.Format("%.0fth Percentile", m_pAnalysis->m_pocketOpts.cp * 100.0);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	l += tSize.cx + tSize.cx / 7;
	CClimateSet climSet(m_pAnalysis->pDB);
	climSet.m_strFilter.Format("[VarID]=%d", m_pAnalysis->m_pocketOpts.varID+1);
	climSet.Open();
	if (climSet.IsBOF() || climSet.IsEOF())
	{
		tStr.Format("-- %.0f%% of the %ld days from %d - %d",
			(m_pAnalysis->m_pocketOpts.cp >= 0.5) ? (1.0 - m_pAnalysis->m_pocketOpts.cp) * 100.0 : m_pAnalysis->m_pocketOpts.cp * 100.0, m_nObs,
			m_pAnalysis->m_pocketOpts.startYear,
			m_pAnalysis->m_pocketOpts.endYear);
		pDC->SetTextColor(0x00000000);
		pDC->TextOut(l, t, tStr);
		t += tSize.cy + 1;
		if ((m_pAnalysis->m_pocketOpts.varID >= 4 &&    // RH MeanRH MinRH MaxRH
			m_pAnalysis->m_pocketOpts.varID <= 7) ||
			(m_pAnalysis->m_pocketOpts.varID >= 16 &&   // FM1 FM10 FM100 FM1000 FMHerb FMWood
				m_pAnalysis->m_pocketOpts.varID <= 21))
			tStr.Format("had an %s %s %.0f",
				m_pAnalysis->m_pocketOpts.m_IndexString,
				"below",
				pcntile97);
		else
			tStr.Format("had an %s %s %.0f",
				m_pAnalysis->m_pocketOpts.m_IndexString,
				"above",
				pcntile97);
	}
	else
	{
		bool wantLower = false;
		if (climSet.m_CriticalPercentile >= climSet.m_cp2)
			wantLower = true;
		tStr.Format("-- %.0f%% of the %ld days from %d - %d",
			wantLower ? m_pAnalysis->m_pocketOpts.cp * 100.0 : (1.0 - m_pAnalysis->m_pocketOpts.cp) * 100.0, m_nObs,
			m_pAnalysis->m_pocketOpts.startYear,
			m_pAnalysis->m_pocketOpts.endYear);
		pDC->SetTextColor(0x00000000);
		pDC->TextOut(l, t, tStr);
		t += tSize.cy + 1;
		//if (climSet.m_CriticalPercentile >= climSet.m_cp2)//RH, moistures, etc
		//{
			tStr.Format("had an %s %s %.0f",
				m_pAnalysis->m_pocketOpts.m_IndexString,
				wantLower ? "below" : "above",
				pcntile97);
		/* }
		else // normal
		{
			tStr.Format("had an %s %s %.0f",
				m_pAnalysis->m_pocketOpts.m_IndexString,
				"above",
				pcntile97);
		}*/
	}
	pDC->TextOut(slRect.left, t, tStr);
	

	pDC->SelectObject(&bigFont);
	pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
	tStr.Format("Local Thresholds - Watch out:");
	t = r2.top + r2.Height() / 5 * 4;
	l = r2.left + r2.Width() / 12;
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	l += tSize.cx + tSize.cx / 28;
	pDC->SelectObject(&normalFont);
	tStr.Format("Combinations");
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	t += tSize.cy + 1;
	l = slRect.left;
	tStr.Format("of any of these factors can greatly increase fire behavior:");
	pDC->TextOut(l, t, tStr);
	t += tSize.cy + 1;
	pDC->SelectObject(&normalBoldFont);
	pDC->TextOut(l, t, "20' Wind Speed");
	tSize = pDC->GetTextExtent("20' Wind Speed");
	l += tSize.cx;
	pDC->SelectObject(&normalFont);
	tStr.Format(" over %d mph, ", m_pAnalysis->m_pocketOpts.m_WindSpeed);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	l += tSize.cx;
	pDC->SelectObject(&normalBoldFont);
	pDC->TextOut(l, t, "RH ");
	tSize = pDC->GetTextExtent("RH ");
	l += tSize.cx;
	pDC->SelectObject(&normalFont);
	tStr.Format(" less than %d%%, ", m_pAnalysis->m_pocketOpts.m_RH);
	pDC->TextOut(l, t, tStr);

	t += tSize.cy + 1;
	l = slRect.left;
	pDC->SelectObject(&normalBoldFont);
	pDC->TextOut(l, t, "Temperature");
	tSize = pDC->GetTextExtent("Temperature");
	l += tSize.cx;
	pDC->SelectObject(&normalFont);
	tStr.Format(" over %d", m_pAnalysis->m_pocketOpts.m_Temp);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	l += tSize.cx;
	if(m_pAnalysis->m_pocketOpts.extraStr.GetLength() > 0)
	{
		pDC->TextOut(l, t, ", ");
		tSize = pDC->GetTextExtent(", ");
		l += tSize.cx;
		pDC->SelectObject(&normalBoldFont);
		pDC->TextOut(l, t, m_pAnalysis->m_pocketOpts.extraStr);
		tSize = pDC->GetTextExtent(m_pAnalysis->m_pocketOpts.extraStr);
		l += tSize.cx;
		pDC->SelectObject(&normalFont);
		if(m_pAnalysis->m_pocketOpts.extraGreater == 0)
			tStr.Format(" %s %d", "less than", m_pAnalysis->m_pocketOpts.m_LiveFM);
		else if(m_pAnalysis->m_pocketOpts.extraGreater == 1)
			tStr.Format(" %s %d", "over", m_pAnalysis->m_pocketOpts.m_LiveFM);
		else
			tStr.Format(" %s %d", "near", m_pAnalysis->m_pocketOpts.m_LiveFM);
		//tStr.Format(" %s %d", (m_pAnalysis->m_pocketOpts.extraGreater == true) ? "over" : "less than", m_pAnalysis->m_pocketOpts.m_LiveFM);
		pDC->TextOut(l, t, tStr);
	}

	//now do r4
	int strCnt = 0;
	l = r4.left + r4.Width() / 16;
	t = r4.top + r4.Height() / 12;
	tStr.Format("Remember what Fire Danger tells you:");	
	pDC->SelectObject(&bigFont);
	pDC->TextOut(l, t, tStr);
	pDC->SelectObject(&normalFont);
	ckRect.left = l;
	ckRect.top = t + 1;
	ckRect.right = l + r4.Width() / 40;
	ckRect.bottom = t + r4.Height() / 25;
	//if (rasterCaps & RC_STRETCHBLT)
		PaintDIB(pDC->m_hDC, ckRect, theApp.checkDIB, theApp.checkRect, NULL);//theApp.checkPal);
	pDC->TextOut(ckRect.right + 1, ckRect.bottom, string1);
	strCnt++;
	tSize = pDC->GetTextExtent(string1);
	t = ckRect.bottom + tSize.cy + 1;
	if(string2.GetLength() > 0)
	{
		pDC->TextOut(ckRect.right + ckRect.Width(), t, string2);
		strCnt++;
		if(string3.GetLength() > 0)
		{
			t += tSize.cy + 1;
			pDC->TextOut(ckRect.right + ckRect.Width(), t, string3);
			strCnt++;
		}
	}
	ckRect.OffsetRect(0, tSize.cy * strCnt + strCnt);
	if (/*rasterCaps & RC_STRETCHBLT && */(string4.GetLength() > 0))
	{
		PaintDIB(pDC->m_hDC, ckRect, theApp.checkDIB, theApp.checkRect, NULL);//theApp.checkPal);
		pDC->TextOut(ckRect.right + 1, ckRect.bottom, string4);
		tSize = pDC->GetTextExtent(tStr);
		ckRect.OffsetRect(0, tSize.cy + 1);
	}
	//if (rasterCaps & RC_STRETCHBLT)
		PaintDIB(pDC->m_hDC, ckRect, theApp.checkDIB, theApp.checkRect, NULL);//theApp.checkPal);
	tStr.Format("Watch local conditions and variations across");
	pDC->TextOut(ckRect.right + 1, ckRect.bottom, tStr);
	tSize = pDC->GetTextExtent(tStr);
	t = ckRect.bottom + tSize.cy + 1;
	tStr.Format("the landscape -- Fuel, Weather, Topography.");
	pDC->TextOut(ckRect.right + ckRect.Width(), t, tStr);
	ckRect.OffsetRect(0, tSize.cy * 2 + 1);
//	if (rasterCaps & RC_STRETCHBLT)
		PaintDIB(pDC->m_hDC, ckRect, theApp.checkDIB, theApp.checkRect, NULL);//theApp.checkPal);
	tStr.Format("Listen to weather forecasts -- especially WIND.");
	pDC->TextOut(ckRect.right + 1, ckRect.bottom, tStr);

	//past experience block
	t = ckRect.bottom + r4.Height() / 12;
	tStr.Format("Past Experience:");
	pDC->SelectObject(&bigFont);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	t += tSize.cy / 2;
	pDC->SelectObject(&normalFont);
	//use DrawText for past experience Text....
	//CString peStr;
	CRect peRect(l, t, r4.right, r4.bottom);
	pDC->SetTextAlign(TA_TOP | TA_LEFT);

	pDC->DrawText(m_pAnalysis->m_pocketOpts.m_PEText, peRect, DT_LEFT | DT_NOPREFIX | DT_WORDBREAK);
	//dump lines of past experience text
	/*int nLines = GetLineCount(pDoc->m_PEText);
	char tmp[256];
	for(int i = 0; i < nLines; i++)
	{
		GetLine(i, pDoc->m_PEText, tmp);
		pDC->TextOut(l, t, tmp);
		tSize = pDC->GetTextExtent(tStr);
		t += tSize.cy + 1;
		if(t >=r4.bottom)
			break;
	}*/

	//NWCG credit!
	CFont italicFont;
	italicFont.CreateFont(r1.Height() / 25, r1.Width() / 80, 0, 0, FW_NORMAL, 1, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica");
	l = r4.right - r4.Width() / 40;
	t = r4.bottom - 1;
	pDC->SetTextAlign(TA_BOTTOM | TA_RIGHT);
	pDC->SelectObject(&normalBoldFont);
	pDC->TextOut(l, t, "Design by NWCG Fire Danger Working Team");
	l = r4.left + r4.Width() / 40;
	t -= tSize.cy;
	tStr = m_pAnalysis->m_pocketOpts.dateStamp + " ";
	pDC->SetTextAlign(TA_BOTTOM | TA_LEFT);
	pDC->SelectObject(&italicFont);
	pDC->TextOut(l, t, tStr);
	tSize = pDC->GetTextExtent(tStr);
	l += tSize.cx;
	tStr.Format("(%s)", m_pAnalysis->m_pocketOpts.dbName);
	tSize = pDC->GetTextExtent(tStr);
	char strPath[MAX_PATH];
	strcpy_s(strPath, m_pAnalysis->m_pocketOpts.dbName);
	PathCompactPath(pDC->GetSafeHdc(), strPath, r4.right - l - 8);
	tStr.Format("(%s)", strPath);
	pDC->TextOut(l, t, tStr);

	l = r4.left + r4.Width() / 40;
	t -= tSize.cy;
	tStr.Format("Responsible Agency: %s", m_pAnalysis->m_pocketOpts.m_RespAgency);
	pDC->SelectObject(&normalFont);
	pDC->TextOut(l, t, tStr);
	//pDC->SetTextAlign(TA_BOTTOM | TA_RIGHT);
	//pDC->TextOut(l, t, pDoc->dateStamp);
//	add right justified timestamp
	//restore drawing tools to original state
    pDC->SelectObject( pOldPen );    
    pDC->SelectObject( pOldFont );    
    pDC->SelectObject( pOldBrush );    
}

double CPocketCardView::IdealFromYMax(double ymax)
{
	if(ymax < 0.15)
		return 0.01;
	else if(ymax < 0.6)
		return 0.05;
	else if(ymax < 1.25)
		return 0.1;
	else if(ymax < 2.5)
		return 0.25;
	else if(ymax < 5.0)
		return 0.5;
	else if(ymax < 10.0)
		return 1.0;
	else if(ymax < 25.0)
		return 2.0;
	else if(ymax < 50.0)
		return 5.0;
	else if(ymax < 100.0)
		return 10.0;
	else if(ymax < 250.0)
		return 20.0;
	else if(ymax < 500.0)
		return 50.0;
	else if(ymax < 1000.0)
		return 100.0;
	else if(ymax < 2500.0)
		return 200.0;
	else if(ymax < 5000.0)
		return 500.0;
	else if(ymax < 10000.0)
		return 1000.0;
	//way big
	int power = (int)log10( ymax / 10.0 );
	if(power > 0)
	{
		double step = floor( (ymax / 10.0) / pow(10.0, power) );
		return floor( step * pow(10.0, power ));
	}

	return ymax / 10;
}
/////////////////////////////////////////////////////////////////////////////
// CPocketCardView diagnostics

#ifdef _DEBUG
void CPocketCardView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPocketCardView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPocketCardView message handlers

void CPocketCardView::Star(CDC *pDC, int x, int y, int starSize)
{
	int lWid = starSize / 5, s = starSize;
	if(lWid <= 1)
		lWid++;
	if(starSize % 2 != 0)
		s++;
	CPen pen;
	if(!pen.CreatePen( PS_SOLID, lWid, RGB(0,0,0)))
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Resources too low for creating asterisk pen.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CPen *oldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(x - s, y);
	pDC->LineTo(x + s, y);
	pDC->MoveTo(x - s / 2, y - s * 2 / 3);
	pDC->LineTo(x + s / 2, y + s * 2 / 3);
	pDC->MoveTo(x - s / 2, y + s * 2 / 3);
	pDC->LineTo(x + s / 2, y - s * 2 / 3);

	pDC->SelectObject(oldPen);
}

/*void CPocketCardView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CScrollView::OnPrint(pDC, pInfo);
}*/

/*void CPocketCardView::OnFilePrint() 
{
	// TODO: Add your command handler code here
	PRINTDLG pd;
	theApp.GetPrinterDeviceDefaults(&pd);
	CPrintDialog pdlg(false);
	pdlg.m_pd.hDevNames = pd.hDevNames;
	pdlg.m_pd.hDevMode = pd.hDevMode;
	if(pdlg.DoModal() == IDOK)
	{
		//theApp.GetPrinterDeviceDefaults(&pd);
		//theApp.SelectPrinter(NULL, NULL);
		//theApp.SelectPrinter(pdlg.m_pd.hDevNames, pdlg.m_pd.hDevMode);
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
	}
	
}*/

int CPocketCardView::GetLineCount(CString str)
{
	int ret = 1;
	for(int i = 0; i < str.GetLength(); i++)
	{
		if(str[i] == '\n')
			ret++;
	}
	return ret;
}

void CPocketCardView::GetLine(int lineNo, CString src, char * trg)
{
	int loc = 0, line = 0;
	char *tmp = new char[src.GetLength() + 1];
	strcpy(tmp, src);
	for(int i = 0; i < (int)strlen(tmp) && i < 256; i++)
	{
		if(tmp[i] == '\n')
			line++;
		else if(line == lineNo)
		{
			if(isprint(tmp[i])) 
				trg[loc++] = tmp[i];
		}
	}
	trg[loc] = 0;
	delete[] tmp;
}
int CPocketCardView::SaveToFile(CString _saveNameStr, CRect *_outRect)
{
	//int saveWidth = 800, saveHeight = 600;
		CDC *whdc = AfxGetMainWnd()->GetDC();//GetDC();
		CDC memhdc;
		memhdc.CreateCompatibleDC(NULL);
		HBITMAP picture = CreateCompatibleBitmap(whdc->m_hDC, _outRect->Width(), _outRect->Height());
		HGDIOBJ Old = SelectObject(memhdc.m_hDC, picture);
		CRect rect(0, 0, _outRect->Width(), _outRect->Height());
		DrawToDCRegion(&memhdc, rect);
		SelectObject(memhdc.m_hDC, Old);
		char fName[256];
		strcpy_s(fName, _saveNameStr);
		SaveBitmapFile(memhdc.m_hDC, picture, fName); 
	//	SelectObject(memhdc.m_hDC, Old);
		//ReleaseDC(whdc);
		memhdc.DeleteDC();
		DeleteObject(picture);
	return 0;
}

void CPocketCardView::OnFileSaveAs() 
{
	chdir(theApp.userDir);
	CFileDialog fd(false, theApp.GetGraphicsExt(), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		theApp.GetGraphicsFileStr(), this);
	//int saveWidth = 640, saveHeight = 480;
	int saveWidth = 800, saveHeight = 600;
	if(fd.DoModal() == IDOK)
	{
		CRect rect(0, 0, 800, 600);
		SaveToFile(fd.GetPathName(), &rect);
		theApp.SetUserDir(fd.GetPathName());
		theApp.SetGraphicsExt(fd.GetFileExt());
	}
	chdir(theApp.dbDir);
}

/*BOOL CPocketCardView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	DoPreparePrinting(pInfo);
	return CScrollView::OnPreparePrinting(pInfo);
}*/

void CPocketCardView::OnFilePrint() 
{
	// TODO: Add your command handler code here
	//CPrintInfo printInfo;
	//OnPreparePrinting(&printInfo);
	theApp.SetLandscape();
	CView::OnFilePrint();
}

BOOL CPocketCardView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	return DoPreparePrinting(pInfo);
	
	//return CScrollView::OnPreparePrinting(pInfo);
}

/*void CPocketCardView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CDocument *pDoc = GetDocument();
	CRuntimeClass* prt = pDoc->GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, "CQuickBatch" ) == 0)
	{
		UINT uFlags;
		CPoint wPoint = point;
		ScreenToClient(&wPoint);
		CMenu tMenu;
		tMenu.LoadMenu(IDR_MENU2);
		CMenu *pPopup = tMenu.GetSubMenu(0);
		if(pPopup)
			pPopup->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);

	}
}*/


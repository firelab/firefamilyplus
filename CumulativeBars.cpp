// CumulativeBars.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "CumulativeBars.h"
#include "fireplusSet.h"
#include "SIGStationSet.h"
#include "FireStats.h"
#include "FireStatDoc.h"
#include "CumulativeFrame.h"
#include <math.h>
#include "BatchOutputFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CCumulativeBars

IMPLEMENT_DYNCREATE(CCumulativeBars, CView)

CCumulativeBars::CCumulativeBars()
{
	fd = lfd = mfd = all = NULL;
}

CCumulativeBars::~CCumulativeBars()
{
	if(all)
		delete[] all;
	if(fd)
		delete[] fd;
	if(lfd)
		delete[] lfd;
	if(mfd)
		delete[] mfd;
}


BEGIN_MESSAGE_MAP(CCumulativeBars, CView)
	//{{AFX_MSG_MAP(CCumulativeBars)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCumulativeBars drawing

void CCumulativeBars::OnDraw(CDC* pDC)
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

/////////////////////////////////////////////////////////////////////////////
// CCumulativeBars diagnostics

#ifdef _DEBUG
void CCumulativeBars::AssertValid() const
{
	CView::AssertValid();
}

void CCumulativeBars::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCumulativeBars message handlers
void CCumulativeBars::DrawToDCRegion(CDC *pDC, CRect rect)
{
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
	char temp[64];
	//CFireStatDoc* pDoc = (CFireStatDoc*)GetDocument();
	//figure axis
	int x0, x1, y0, y1, y, x;
	x0 = rect.left + rect.Width() / 10;
	x1 = rect.right - (x0 - rect.left)/ 10;
	y1 = rect.top + rect.Height() / 10;
	y0 = rect.bottom - 2 * (y1 - rect.top);
	//load pens and fonts
    CPen penBlack2, penBlack1, penRed, penBlue, penMagenta;  // Construct it, then initialize
    if(!penBlack2.CreatePen( PS_SOLID, 2, RGB(0,0,0)) 
		|| !penBlack1.CreatePen( PS_SOLID, 1, RGB(0,0,0))     
		|| !penBlue.CreatePen( PS_SOLID, 1, RGB(0,200,200))     
		|| !penMagenta.CreatePen( PS_SOLID, 1, RGB(200,0,200))     
		|| !penRed.CreatePen( PS_SOLID, 1, RGB(200,0,0)) )    
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Resources too low for creating drawing pens.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CFont headerFont, axisFont, vheaderFont;
	if(!headerFont.CreateFont( (y1 - rect.top) / 2, (x0 - rect.left)/ 6, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")
		|| !vheaderFont.CreateFont((x0 - rect.left) / 4,  (y1 - rect.top) / 4 , 900, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")
		|| !axisFont.CreateFont( (y1 - rect.top) / 3, (x0 - rect.left) / 10, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica") )
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create probability graph fonts.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CBrush whiteBrush, blueBrush, magentaBrush, redBrush;
	if(!whiteBrush.CreateSolidBrush(0x00FFFFFF)
		//|| !blueBrush.CreateHatchBrush(HS_BDIAGONAL, RGB(0,200,200))
		|| !blueBrush.CreateSolidBrush(RGB(0,200,200))
		|| !magentaBrush.CreateHatchBrush(HS_FDIAGONAL, RGB(200,0,200))
		|| !redBrush.CreateHatchBrush(HS_BDIAGONAL, RGB(200, 0, 0)))
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create brushes.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	
	CBrush *pOldBrush = pDC->SelectObject(&whiteBrush);
    CPen* pOldPen = pDC->SelectObject( &penBlack1 );
	pDC->Rectangle(rect);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.top);
	pDC->LineTo(rect.left, rect.top);

	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);


	//draw axis
    // Select it into the device context
    // Save the old pen at the same time
    //CPen* pOldPen = 
	pDC->SelectObject( &penBlack2 );
	pDC->MoveTo(x1, y0);
	pDC->LineTo(x0, y0);
	pDC->LineTo(x0, y1);
	// title and x axis label
	//output headers
	CFont *oldFont = pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);
	//HEADERS
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 + 1, m_fires->staStr);
	if(m_fires->end.GetYear() > m_fires->start.GetYear())
		sprintf(temp, "%d - %d", m_fires->start.GetYear(),
			m_fires->end.GetYear());
	else
		sprintf(temp, "%d", m_fires->end.GetYear());
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 * 2 + 4, temp);
	pDC->TextOut(x0 + (x1 - x0) / 2, y0 + (rect.bottom - y0) / 3 * 2, m_fires->varName);
	pDC->SelectObject(&vheaderFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);

	pDC->TextOut(rect.left + (x0 - rect.left) / 3, (y1 + y0) / 2, "Frequency");
//	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);

	//output y axis values
	pDC->SelectObject( &penBlack1 );
	pDC->SelectObject(&axisFont);
	int diff = pDC->GetTextExtent("M").cy / 4;
	pDC->SetTextAlign(TA_BASELINE | TA_RIGHT);
	double yIdeal = IdealFromXMax(ymax);
	int i = 0;
	while(i * yIdeal < ymax)
	{
		y = y0 - (int)(((double)i * yIdeal) / (double)ymax * (y0 - y1));
		pDC->MoveTo(x0, y);
		pDC->LineTo(x0 - (x0 - rect.left)/ 10, y);
		sprintf(temp, "%.0f", i * yIdeal);
		pDC->TextOut(x0 - (x0 - rect.left)/ 10, y + diff, temp);

		i++;
	}
	//figure x axis values
	double ideal, xmax;
	//xmax = m_fires->nv > 0 ? m_fires->allVals[m_fires->nv - 1] : 0;
	xmax = m_fires->nv > 0 ? m_fires->allVals[iEnd] : 0;
	ideal = IdealFromXMax(xmax);
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	int bw, gap, skip;
	x = (x1 - x0) / (nBins + 1);
	bw = x * 2 / 3;
	gap = x / 3;
	x = x0 + bw;
	skip = nBins / 10 + 1;
	for(int b = 0; b < nBins; b++)
	{
		y = y0 - (int)((double)fd[b] / (double)ymax * (double)(y0 - y1));
		pDC->SelectObject( &penBlack1);
		pDC->SelectObject( &blueBrush );
		pDC->Rectangle(x - bw / 2, y, x + bw / 2, y0);
		y = y0 - (int)((double)all[b] / (double)ymax * (double)(y0 - y1));
		pDC->SelectObject( &penBlack1);
		//pDC->SelectObject( &whiteBrush );
		pDC->MoveTo(x - bw / 2, y);
		pDC->LineTo(x - bw / 2, y0);
		pDC->LineTo(x + bw / 2, y0);
		pDC->LineTo(x + bw / 2, y);
		pDC->LineTo(x - bw / 2, y);
		//pDC->Rectangle(x - bw / 2, y, x + bw / 2, y0);
		y = y0 - (int)((double)lfd[b] / (double)ymax * (double)(y0 - y1));
		pDC->SelectObject( &penMagenta);
		pDC->SelectObject( &magentaBrush );
		pDC->Rectangle(x - bw / 2, y, x + bw / 2, y0);
		y = y0 - (int)((double)mfd[b] / (double)ymax * (double)(y0 - y1));
		pDC->SelectObject( &penRed);
		pDC->SelectObject( &redBrush );
		pDC->Rectangle(x - bw / 2, y, x + bw / 2, y0);
		pDC->SelectObject( &penBlack1);
		pDC->MoveTo(x, y0);
		pDC->LineTo(x, y0 + (rect.bottom - y0) / 10);
		if(b % skip == 0)
		{
			CString xLabel;
			//2/24/99
			if(m_fires->binSize >= 1.0)
				xLabel.Format("%.0f", b  * m_fires->binSize + (m_fires->nv > 0 ? m_fires->allVals[iStart] : 0.0));
			else
				xLabel.Format("%.1f", b  * m_fires->binSize + (m_fires->nv > 0 ? m_fires->allVals[iStart] : 0.0));
			//xLabel.Format("%.0f", (b + m_fires->allVals[0]) * m_fires->binSize);
			pDC->TextOut(x, y0 + (rect.bottom - y0) / 10, xLabel);
		}
		x += bw + gap;
	}
	CSize ts;
	pDC->SetTextAlign(TA_TOP | TA_LEFT);
	pDC->SelectObject(&axisFont);
	ts = pDC->GetTextExtent("M");
	bw = (x0 - rect.left) / 10;
	x = rect.left + bw;
	y = rect.bottom - ts.cy - 1;//* 10 / 9;
	pDC->SelectObject( &penRed);
	pDC->SelectObject( &redBrush );
	pDC->Rectangle(x, y, x + bw, y + ts.cy);
	pDC->TextOut(x + 2 * bw, y, "Multi Fire Days");
	pDC->SelectObject( &penMagenta);
	pDC->SelectObject( &magentaBrush );
	y -= ts.cy * 6 / 5;
	pDC->Rectangle(x, y, x + bw, y + ts.cy);
	pDC->TextOut(x + 2 * bw, y, "Large Fire Days");
	pDC->SelectObject( &penBlack1);
	pDC->SelectObject( &blueBrush );
	y -= ts.cy * 6 / 5;
	pDC->Rectangle(x, y, x + bw, y + ts.cy);
	pDC->TextOut(x + 2 * bw, y, "Fire Days");
	pDC->SelectObject(&penBlack1);
	y -= ts.cy * 6 / 5;
	pDC->MoveTo(x, y);
	pDC->LineTo(x + bw, y);
	pDC->LineTo(x + bw, y + ts.cy);
	pDC->LineTo(x, y + ts.cy);
	pDC->LineTo(x, y);
	pDC->TextOut(x + 2 * bw, y, "All Days");
	x = rect.left + rect.Width() / 4;
	//y = rect.bottom - ts.cy * 5 / 4;
	y = rect.bottom - ts.cy - 1;//* 10 / 9;
	CString str;
	str.Format("Multi = %d+", m_fires->mfdFires);
	pDC->TextOut(x, y, str);
	//y -= ts.cy * 5 / 4;
	y -= ts.cy * 6 / 5;
	str.Format("Large = %d+", m_fires->lfAcres);
	pDC->TextOut(x, y, str);
	//y -= ts.cy * 5 / 4;
	y -= ts.cy * 6 / 5;
	if(m_fires->cause == 0)
		str.Format("Cause = All");
	else if(m_fires->cause == 1)
		str.Format("Cause = Lightning");
	else
		str.Format("Cause = Human");
	pDC->TextOut(x, y, str);
	if (m_fires->filterValue != -999.0)
	{
		y -= ts.cy * 6 / 5;
		if (m_fires->filterDirection == FILTER_LESSTHAN_EQUAL)
			str.Format("Filtered %d <= %.0f", m_fires->nFiltered, m_fires->filterValue);
		else
			str.Format("Filtered %d >= %.0f", m_fires->nFiltered, m_fires->filterValue);
		pDC->TextOut(x, y, str);
	}
	//output version/DateTime stamp
	y = rect.bottom - ts.cy * 5 / 4;
	//CString dStr = "%m/%d/%Y-%H:%M";
	//dateTime.Format(dStr);
	//CString ret;
	//ret.Format("FF+%s %s", verStr, dStr); 
	//str = m_fires->dateStamp.Format("FF+3.0 %m/%d/%Y-%H:%M");
	CString dStr = m_fires->dateStamp.Format("%m/%d/%Y-%H:%M");
	//str.Format("FF+%s %s", verStr, dStr);
	str.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	// = m_fires->dateStamp.Format("FF+3.0 %m/%d/%Y-%H:%M");
	CSize xTextSize;
	xTextSize = pDC->GetTextExtent(str);
	pDC->TextOut(x1 - xTextSize.cx , y, str);
	if(m_fires->graphModelString.GetLength() > 0)
	{	//output model code in lower right corner
		y -= ts.cy * 5 / 4;
		CString mStr = "Model: " + m_fires->graphModelString;
		xTextSize = pDC->GetTextExtent(mStr);
		
		pDC->TextOut(x1 - xTextSize.cx , y, mStr);
	}
	
	//restore GDI objects
    pDC->SelectObject( pOldPen );    
    pDC->SelectObject( oldFont );    
	pDC->SelectObject( pOldBrush );    
}

double CCumulativeBars::IdealFromXMax(double xmax)
{
	if(xmax < 0.15)
		return 0.01;
	else if(xmax < 0.6)
		return 0.05;
	else if(xmax < 1.25)
		return 0.1;
	else if(xmax < 2.5)
		return 0.25;
	else if(xmax < 5.0)
		return 0.5;
	else if(xmax < 10.0)
		return 1.0;
	else if(xmax < 25.0)
		return 2.0;
	else if(xmax < 50.0)
		return 5.0;
	else if(xmax < 100.0)
		return 10.0;
	else if(xmax < 250.0)
		return 20.0;
	else if(xmax < 500.0)
		return 50.0;
	else if(xmax < 1000.0)
		return 100.0;
	else if(xmax < 2500.0)
		return 200.0;
	else if(xmax < 5000.0)
		return 500.0;
	else if(xmax < 10000.0)
		return 1000.0;
	//way big
	int power = (int)log10( xmax / 10.0 );
	if(power > 0)
	{
		double step = floor( (xmax / 10.0) / pow(10.0, power) );
		return floor( step * pow(10.0, power ));
	}

	return xmax / 10;
}



void CCumulativeBars::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	//need to create display bins with frequency counts...
	CFireStatDoc* pDoc = (CFireStatDoc*)GetDocument();
	double xT = 0.0, xB = 0.0;
	m_fires->nFiltered = 0;
	iStart = 0;
	iEnd = m_fires->nv - 1;
	if(m_fires->nv > 0)
	{
		xT = m_fires->allVals[m_fires->nv - 1]; 
		xB = m_fires->allVals[0];
		if (m_fires->filterValue != -999.0)
		{
			if (m_fires->filterDirection == FILTER_LESSTHAN_EQUAL)
			{
				for (int i = 0; i < m_fires->nv; i++)
				{
					if (m_fires->allVals[i] > m_fires->filterValue)
					{
						xB = m_fires->allVals[i];
						iStart = i;
						break;
					}
					else
						m_fires->nFiltered++;
				}
			}
			else
			{
				for (int i = m_fires->nv - 1; i >= 0; i--)
				{
					if (m_fires->allVals[i] < m_fires->filterValue)
					{
						xT = m_fires->allVals[i];
						iEnd = i;
						break;
					}
					else
						m_fires->nFiltered++;
				}
			}
		}
	}
	nBins = ((int)((xT - xB) / m_fires->binSize)) + 1;
	if(nBins <= 0)
		nBins = 1;
	all = new long[nBins];
	fd = new long[nBins];
	lfd = new long[nBins];
	mfd = new long[nBins];
	int i;
	for(i = 0; i < nBins; i++)
	{
		all[i] = 0;
		fd[i] = 0;
		lfd[i] = 0;
		mfd[i] = 0;
	}
	ymax = 0;
	for (i = iStart; i <= iEnd; i++)
	//for (i = 0; i < m_fires->nv; i++)
	{
		//2/24/99
		int loc = (int) ((m_fires->allVals[i] - xB + m_fires->binSize / 2) / m_fires->binSize); 
		if(loc < 0)
			loc = 0;
		else if(loc > nBins - 1)
			loc = nBins - 1;
		all[loc]++;
	}
	for(i = 0; i < m_fires->nfd; i++)
	{
		//9/3/2018
		if (m_fires->filterValue != -999.0)
		{
			if (m_fires->filterDirection == FILTER_LESSTHAN_EQUAL && m_fires->fdVals[i] <= m_fires->filterValue)
				continue;
			if (m_fires->filterDirection == FILTER_GREATERTHAN_EQUAL && m_fires->fdVals[i] >= m_fires->filterValue)
				continue;
		}
		//2/24/99
		int loc = (int) ((m_fires->fdVals[i] - xB + m_fires->binSize / 2) / m_fires->binSize); 
		if(loc < 0)
			loc = 0;
		else if(loc > nBins - 1)
			loc = nBins - 1;
		fd[loc]++;
	}
	for(i = 0; i < m_fires->nlfd; i++)
	{
		//9/3/2018
		if (m_fires->filterValue != -999.0)
		{
			if (m_fires->filterDirection == FILTER_LESSTHAN_EQUAL && m_fires->lfdVals[i] <= m_fires->filterValue)
				continue;
			if (m_fires->filterDirection == FILTER_GREATERTHAN_EQUAL && m_fires->lfdVals[i] >= m_fires->filterValue)
				continue;
		}
		//2/24/99
		int loc = (int) ((m_fires->lfdVals[i] - xB + m_fires->binSize / 2) / m_fires->binSize); 
		if(loc < 0)
			loc = 0;
		else if(loc > nBins - 1)
			loc = nBins - 1;
		lfd[loc]++;
	}
	for(i = 0; i < m_fires->nmfd; i++)
	{
		//9/3/2018
		if (m_fires->filterValue != -999.0)
		{
			if (m_fires->filterDirection == FILTER_LESSTHAN_EQUAL && m_fires->mfdVals[i] <= m_fires->filterValue)
				continue;
			if (m_fires->filterDirection == FILTER_GREATERTHAN_EQUAL && m_fires->mfdVals[i] >= m_fires->filterValue)
				continue;
		}
		//2/24/99
		int loc = (int) ((m_fires->mfdVals[i] - xB + m_fires->binSize / 2) / m_fires->binSize); 
		if(loc < 0)
			loc = 0;
		else if(loc > nBins - 1)
			loc = nBins - 1;
		mfd[loc]++;
	}
	for(i = 0; i < nBins; i++)
		if(all[i] > ymax)
			ymax = all[i];
	ymax += ymax / 10;
	if(ymax == 0)
		ymax = 1;
}

void CCumulativeBars::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//CCumulativeFrame *frame = (CCumulativeFrame *)GetParent()->GetParent();
	//frame->MaximizePane(this);
	CWnd *pWnd = GetParent()->GetParent();
	if(pWnd)
	{
		if(pWnd->IsKindOf(RUNTIME_CLASS(CCumulativeFrame)))
			((CCumulativeFrame *)pWnd)->MaximizePane(this);
	}
}

BOOL CCumulativeBars::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	
	return DoPreparePrinting(pInfo);
	//return CView::OnPreparePrinting(pInfo);
}

void CCumulativeBars::OnFilePrint() 
{
	// TODO: Add your command handler code here
	CView::OnFilePrint();	
}

void CCumulativeBars::PrintGraph()
{
	theApp.SetLandscape();
	OnFilePrint();
}

void CCumulativeBars::OnLButtonUp(UINT nFlags, CPoint point)
{
	CCumulativeFrame *frame1 = 0;
	CBatchOutputFrame *frame2 = 0;
	CWnd *pWnd = GetParent()->GetParent();
	if(!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CCumulativeFrame)))
	{
		pWnd = GetParent()->GetParent()->GetParent();
		if(!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CBatchOutputFrame)))
			return;
		else
			frame2 =(CBatchOutputFrame *) pWnd;
	}
	else
		frame1 = (CCumulativeFrame *)GetParent()->GetParent();

	double xmax = 1.0, xmin = 0.0;
	if(m_fires->nv > 0)
	{
		xmax = m_fires->allVals[m_fires->nv - 1];
		xmin = m_fires->allVals[0];
	}
	CRect rect;
	GetClientRect(rect);
	rect.NormalizeRect();
	if(rect.Width() == 0 || rect.Height() == 0)
		return;
	int x0, x1, y0, y1;
	x0 = rect.Width() / 10;
	x1 = rect.right - x0 / 10;
	y1 = rect.Height() / 10;
	y0 = rect.bottom - 2 * y1;
	char xtext[64], ytext[64];
	xtext[0] = 0;
	ytext[0] = 0;
	if(rect.Width() > 0 || rect.Height() > 0)
	{//figure values
		/*x0 = rect.Width() / 8;
		x1 = rect.right - x0 / 10;
		y1 = rect.Height() / 10;
		y0 = rect.bottom - 2 * y1;*/
		if(point.x < x0 || point.x > x1 )
			strcpy_s(xtext, "NA");
		else
		{
			//int bw, gap, skip;
			//double xmin = (m_fires->nv > 0) ? m_fires->allVals[0] : 0.0;
			int tx = (x1 - x0) / (nBins + 1);
			int bw = tx * 2 / 3;
			int gap = tx / 3;
			//x = x0 + bw;
			//skip = nBins / 10 + 1;
			//double x = ((double)(point.x - x0)) / ((double)(x1 - x0)) * (xmax - xmin) + xmin;
			double x = ((double)(point.x - x0)) / ((double)(bw + gap)) + xmin - 1;
			if(m_fires->varID == 9)
				sprintf(xtext, "%.2f", x);
			else
				sprintf(xtext, "%.0f", x);
		}
		if(point.y > y0 || point.y < y1)
			strcpy_s(ytext, "NA");
		else
		{
			//double y = -(((double)(point.y - y0)) / ((double)(y0 - y1)));
			//double y = y0 - (int)(((double)i * yIdeal) / (double)ymax * (y0 - y1));
			double y = -(double)ymax * ((double)(point.y - y0)) / ((double)(y0 - y1)) / 100.0;
			sprintf(ytext, "%.0f", y * 100.0);
		}

	}
	/*CSplitterFrame *frame = (CSplitterFrame *)GetParent()->GetParent();
	frame->m_wndStatusBar.SetPaneText(2, xtext);
	frame->m_wndStatusBar.SetPaneText(4, ytext);*/
	if(frame1)
	{
		frame1->m_wndStatusBar.SetPaneText(2, xtext);
		frame1->m_wndStatusBar.SetPaneText(4, ytext);
	}
	else if(frame2)
	{
		frame2->m_wndStatusBar.SetPaneText(2, xtext);
		frame2->m_wndStatusBar.SetPaneText(4, ytext);
	}
}

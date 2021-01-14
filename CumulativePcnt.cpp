// CumulativePcnt.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "CumulativePcnt.h"
#include "CumulativeFrame.h"
#include "fireplusSet.h"
#include "SIGStationSet.h"
#include "FireStats.h"
#include "FireStatDoc.h"
#include <math.h>
#include "BatchOutputFrame.h"
#include "line.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CCumulativePcnt

IMPLEMENT_DYNCREATE(CCumulativePcnt, CView)

CCumulativePcnt::CCumulativePcnt()
{
}

CCumulativePcnt::~CCumulativePcnt()
{
}


BEGIN_MESSAGE_MAP(CCumulativePcnt, CView)
	//{{AFX_MSG_MAP(CCumulativePcnt)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCumulativePcnt drawing

void CCumulativePcnt::OnDraw(CDC* pDC)
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
		pDC->GetBoundsRect(&rect, DCB_RESET);
	if(rect.Width() == 0 || rect.Height() == 0)
		return;
	DrawToDCRegion(pDC, rect);*/
}

/////////////////////////////////////////////////////////////////////////////
// CCumulativePcnt diagnostics

#ifdef _DEBUG
void CCumulativePcnt::AssertValid() const
{
	CView::AssertValid();
}

void CCumulativePcnt::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCumulativePcnt message handlers
void CCumulativePcnt::DrawToDCRegion(CDC *pDC, CRect rect)
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
	// TODO: add draw code here
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
	CBrush whiteBrush;
	if(!whiteBrush.CreateSolidBrush(0x00FFFFFF))
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create brushes.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	
	CBrush *pOldBrush = pDC->SelectObject(&whiteBrush);
	//pDC->Rectangle(rect);
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

	pDC->TextOut(rect.left + (x0 - rect.left) / 3, (y1 + y0) / 2, "Percentile");
	//output y axis values
	pDC->SelectObject( &penBlack1 );
	pDC->SelectObject(&axisFont);
	int diff = pDC->GetTextExtent("M").cy / 4;
	pDC->SetTextAlign(TA_BASELINE | TA_RIGHT);
	for(int i = 1; i <= 10; i++)
	{
		y = y0 - i * (y0 - y1) / 10;
		pDC->MoveTo(x0, y);
		pDC->LineTo(x0 - (x0 - rect.left)/ 10, y);
		sprintf(temp, "%d", i * 10);
		pDC->TextOut(x0 - (x0 - rect.left)/ 10, y + diff, temp);
	}
	//figure x axis values
	double ideal, xmax = 1.0, xmin = 0.0;
	if(m_fires->nv > 0)
	{
		xmax = m_fires->allVals[m_fires->nv - 1];
		xmin = min(0.0, m_fires->allVals[0]);
	}
	ideal = IdealFromXMax(xmax - xmin);
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	x = x0;
	int z = 0;
	while((z * ideal) > xmin)
			z--;
		//x = 0;
	y = z;// * ideal;//ymin;
	while( x <= x1 )
	{
		if(ideal < 1.0)
			sprintf(temp,"%.1f ", y * ideal);
		else 
			sprintf(temp,"%.0f ", y * ideal);
		pDC->MoveTo(x, y0);
		pDC->LineTo(x, y0 + (rect.bottom - y0) / 10);
		pDC->TextOut(x, y0 + (rect.bottom - y0) / 10, temp);
		y++;
		//x = x0 + (int)(( y * ideal / xmax) * (double)(x1 - x0));
		x = x0 + (int)(( (y - z) * ideal / (xmax - xmin)) * (double)(x1 - x0));
	}
	//draw a line for all days percentiles
	pDC->SelectObject( &penBlack2 );
	/*y = y0;
	x = x0 + (int)(m_fires->allVals[0] / xmax * (x1 - x0));
	pDC->MoveTo(x, y);
	for(i = 0; i < m_fires->nv; i++)
	{
		x = x0 + (int)(m_fires->allVals[i] / xmax * (x1 - x0));
		y = y0 - (int)((double)(i + 1) / (double)m_fires->nv * (double)(y0 - y1));
		pDC->LineTo(x, y);
	}*/
	//pDC->SelectObject( &penGreen2 );
	double xStart = m_fires->nv > 0 ? m_fires->allVals[0] : 0;
	y = y0;
	//x = x0 + (int)((m_fires->allVals[0] - xmin) / (xmax - xmin) * (x1 - x0));
	x = x0 + (int)((xStart - xmin) / (xmax - xmin) * (x1 - x0));
	pDC->MoveTo(x, y);
	for(double xVal = xmin; xVal <= xmax; (m_fires->varID == 9) ? xVal += 0.05 : xVal++)
	{
		double yVal = m_fires->PercentileFromValue(0, xVal);
		y = y0 - ((double)(y0 - y1)) * yVal;
		x = x0 + (int)((xVal - xmin) / (xmax - xmin) * (double)(x1 - x0));
		pDC->LineTo(x, y);
	}
	COLORREF colorBlue = RGB(0, 200, 200);
	COLORREF colorMagenta = RGB(200,0,200);
	COLORREF colorRed = RGB(200,0,0);
	unsigned auxType[8];
	bool Round = true;
	int brushWidth = 3;
	if(pDC->IsPrinting())
	{
		CDC *screenDC = GetDC();
		brushWidth = ScreenPixelsToPrinterPixels(screenDC, pDC, brushWidth);
		ReleaseDC(screenDC);
	}
	int c1 = CDashLine::GetPattern(auxType, Round, brushWidth, 
		CDashLine::DL_SOLID);
	CDashLine auxLine(*pDC, auxType, c1);

	if(m_fires->nfd > 0)
	{
		//pDC->SelectObject( &penBlue );
		/*y = y0;
		x = x0 + (int)(m_fires->fdVals[0] / xmax * (x1 - x0));
		pDC->MoveTo(x, y);
		for(i = 0; i < m_fires->nfd; i++)
		{
			x = x0 + (int)(m_fires->fdVals[i] / xmax * (x1 - x0));
			y = y0 - (int)((double)(i + 1) / (double)m_fires->nfd * (double)(y0 - y1));
			pDC->LineTo(x, y);
		}*/
		pDC->BeginPath();
		y = y0;
		x = x0 + (int)((m_fires->fdVals[0] - xmin) / (xmax - xmin) * (x1 - x0));
		auxLine.MoveTo(x, y);
		for(double xVal = xmin; xVal <= xmax; (m_fires->varID == 9) ? xVal += 0.05 : xVal++)
		{
			double yVal = m_fires->PercentileFromValue(1, xVal);
			y = y0 - ((double)(y0 - y1)) * yVal;
			x = x0 + (int)((xVal - xmin) / (xmax - xmin) * (double)(x1 - x0));
			auxLine.LineTo(x, y);
		}
		pDC->EndPath();
		DrawPathOutline(colorBlue, *pDC, Round, brushWidth);
	}

	c1 = CDashLine::GetPattern(auxType, Round, brushWidth, 
		CDashLine::DL_DASH);
	CDashLine auxLine2(*pDC, auxType, c1);

	if(m_fires->nlfd > 0)
	{
		//pDC->SelectObject( &penMagenta );
		/*y = y0;
		x = x0 + (int)(m_fires->lfdVals[0] / xmax * (x1 - x0));
		pDC->MoveTo(x, y);
		for(i = 0; i < m_fires->nlfd; i++)
		{
			x = x0 + (int)(m_fires->lfdVals[i] / xmax * (x1 - x0));
			y = y0 - (int)((double)(i + 1) / (double)m_fires->nlfd * (double)(y0 - y1));
			pDC->LineTo(x, y);
		}*/
		pDC->BeginPath();
		y = y0;
		x = x0 + (int)((m_fires->lfdVals[0] - xmin) / (xmax - xmin) * (x1 - x0));
		auxLine2.MoveTo(x, y);
		for(double xVal = xmin; xVal <= xmax; (m_fires->varID == 9) ? xVal += 0.05 : xVal++)
		{
			double yVal = m_fires->PercentileFromValue(2, xVal);
			y = y0 - ((double)(y0 - y1)) * yVal;
			x = x0 + (int)((xVal - xmin) / (xmax - xmin) * (double)(x1 - x0));
			auxLine2.LineTo(x, y);
		}
		pDC->EndPath();
		DrawPathOutline(colorMagenta, *pDC, Round, brushWidth);
	}


	c1 = CDashLine::GetPattern(auxType, Round, brushWidth, 
		CDashLine::DL_DOT);
	CDashLine auxLine3(*pDC, auxType, c1);
	if(m_fires->nmfd > 0)
	{
		//pDC->SelectObject( &penRed);
		/*y = y0;
		x = x0 + (int)(m_fires->mfdVals[0] / xmax * (x1 - x0));
		pDC->MoveTo(x, y);
		for(i = 0; i < m_fires->nmfd; i++)
		{
			x = x0 + (int)(m_fires->mfdVals[i] / xmax * (x1 - x0));
			y = y0 - (int)((double)(i + 1) / (double)m_fires->nmfd * (double)(y0 - y1));
			pDC->LineTo(x, y);
		}*/
		pDC->BeginPath();
		y = y0;
		x = x0 + (int)((m_fires->mfdVals[0] - xmin) / (xmax - xmin) * (x1 - x0));
		auxLine3.MoveTo(x, y);
		for(double xVal = xmin; xVal <= xmax; (m_fires->varID == 9) ? xVal += 0.05 : xVal++)
		{
			double yVal = m_fires->PercentileFromValue(3, xVal);
			y = y0 - ((double)(y0 - y1)) * yVal;
			x = x0 + (int)((xVal - xmin) / (xmax - xmin) * (double)(x1 - x0));
			auxLine3.LineTo(x, y);
		}
		pDC->EndPath();
		DrawPathOutline(colorRed, *pDC, Round, brushWidth);
	}
	int lw = (x0 - rect.left) / 2;
	x = rect.left + lw / 4;
	CSize ts;
	pDC->SetTextAlign(TA_TOP | TA_LEFT);
	pDC->SelectObject(&axisFont);
	ts = pDC->GetTextExtent("M");
	y = rect.bottom - ts.cy - 1;// * 10 / 9;
	pDC->BeginPath();
	auxLine3.MoveTo(x, y + ts.cy / 2);
	auxLine3.LineTo(x + lw - 1, y + ts.cy / 2);
	pDC->EndPath();
	DrawPathOutline(colorRed, *pDC, Round, brushWidth);
	pDC->SelectObject(&penRed);
	pDC->TextOut(x + lw + 1, y, "Multi Fire Day");
	y -= ts.cy * 6 / 5;
	pDC->BeginPath();
	auxLine2.MoveTo(x, y + ts.cy / 2);
	auxLine2.LineTo(x + lw - 1, y + ts.cy / 2);
	pDC->EndPath();
	DrawPathOutline(colorMagenta, *pDC, Round, brushWidth);
	pDC->SelectObject(&penMagenta);
	pDC->TextOut(x + lw + 1, y, "Large Fire Day");
	y -= ts.cy * 6 / 5;
	pDC->BeginPath();
	auxLine.MoveTo(x, y + ts.cy / 2);
	auxLine.LineTo(x + lw - 1, y + ts.cy / 2);
	pDC->EndPath();
	DrawPathOutline(colorBlue, *pDC, Round, brushWidth);
	pDC->SelectObject(&penBlue);
	pDC->TextOut(x + lw + 1, y, "Fire Day");
	y -= ts.cy * 6 / 5;
	pDC->SelectObject(&penBlack1);
	pDC->MoveTo(x, y + ts.cy / 2);
	pDC->LineTo(x + lw - 1, y + ts.cy / 2);
	pDC->TextOut(x + lw + 1, y, "All Days");
	x = rect.left + rect.Width() / 4;
	y = rect.bottom - ts.cy * 5 / 4;
	CString str;
	str.Format("Multi = %d+", m_fires->mfdFires);
	pDC->TextOut(x, y, str);
	y -= ts.cy * 5 / 4;
	str.Format("Large = %d+", m_fires->lfAcres);
	pDC->TextOut(x, y, str);
	y -= ts.cy * 5 / 4;
	if(m_fires->cause == 0)
		str.Format("Cause = All");
	else if(m_fires->cause == 1)
		str.Format("Cause = Lightning");
	else
		str.Format("Cause = Human");
	pDC->TextOut(x, y, str);

	//output version/DateTime stamp
	y = rect.bottom - ts.cy * 5 / 4;
	CString dStr = m_fires->dateStamp.Format("%m/%d/%Y-%H:%M");
	//str.Format("FF+%s %s", verStr, dStr);
	str.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	//str = m_fires->dateStamp.Format("FF+3.0 %m/%d/%Y-%H:%M");
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

double CCumulativePcnt::IdealFromXMax(double xmax)
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



void CCumulativePcnt::OnLButtonDblClk(UINT nFlags, CPoint point) 
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

void CCumulativePcnt::PrintGraph()
{
	theApp.SetLandscape();
	OnFilePrint();
}

void CCumulativePcnt::OnFilePrint() 
{
	// TODO: Add your command handler code here
	CView::OnFilePrint();	
	
}

BOOL CCumulativePcnt::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	return DoPreparePrinting(pInfo);
	
	//return CView::OnPreparePrinting(pInfo);
}

void CCumulativePcnt::OnLButtonUp(UINT nFlags, CPoint point)
{
	CView::OnLButtonUp(nFlags, point);
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
		xmin = min(0.0, m_fires->allVals[0]);
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
			strcpy(xtext, "NA");
		else
		{
			double x = ((double)(point.x - x0)) / ((double)(x1 - x0)) * (xmax - xmin) + xmin;
			if(m_fires->varID == 9)
				sprintf(xtext, "%.2f", x);
			else
				sprintf(xtext, "%.0f", x);
		}
		if(point.y > y0 || point.y < y1)
			strcpy(ytext, "NA");
		else
		{
			double y = -(((double)(point.y - y0)) / ((double)(y0 - y1)));
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

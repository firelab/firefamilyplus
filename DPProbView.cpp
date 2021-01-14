// DPProbView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DPProbView.h"
#include "DPDoc.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDPProbView
extern CFireplusApp theApp;
IMPLEMENT_DYNCREATE(CDPProbView, CView)

CDPProbView::CDPProbView()
{
}

CDPProbView::~CDPProbView()
{
}


BEGIN_MESSAGE_MAP(CDPProbView, CView)
	//{{AFX_MSG_MAP(CDPProbView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDPProbView drawing

void CDPProbView::OnDraw(CDC* pDC)
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
// CDPProbView diagnostics

#ifdef _DEBUG
void CDPProbView::AssertValid() const
{
	CView::AssertValid();
}

void CDPProbView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDPProbView message handlers

void CDPProbView::DrawToDCRegion(CDC *pDC, CRect rect)
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
	CDPDoc* pDoc = (CDPDoc*)GetDocument();
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
	CBrush whiteBrush, blueBrush;
	if(!whiteBrush.CreateSolidBrush(0x00FFFFFF)
		|| !blueBrush.CreateSolidBrush(RGB(0,200,200))
		)
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
	//years
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 + 1, pDoc->fires->staStr);
	if(pDoc->fires->end.GetYear() > pDoc->fires->start.GetYear())
		sprintf(temp, "%d - %d", pDoc->fires->start.GetYear(),
			pDoc->fires->end.GetYear());
	else
		sprintf(temp, "%d", pDoc->fires->end.GetYear());
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 * 2 + 4, temp);
	//time of year header
	sprintf(temp, "%d/%d - %d/%d", pDoc->fires->start.GetMonth(), pDoc->fires->start.GetDay(), 
		pDoc->fires->end.GetMonth(), pDoc->fires->end.GetDay());
	pDC->TextOut(x0 + (x1 - x0) / 2, y1 + 5, temp);//rect.top + (y1 - rect.top) / 3 * 2 + 4, temp);

	pDC->TextOut(x0 + (x1 - x0) / 2, y0 + (rect.bottom - y0) / 3 * 2, pDoc->fires->varName);
	pDC->SelectObject(&vheaderFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);

	pDC->TextOut(rect.left + (x0 - rect.left) / 3, (y1 + y0) / 2, "Probability");
//	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);

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
	double ideal, xmax;
	xmax = pDoc->fires->allVals[pDoc->fires->nv - 1];
	ideal = IdealFromXMax(xmax);
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	//for(int i = 1; i 
	x = x0;
	y = 0;
	while( x <= x1 )
	{
		if(ideal < 1.0)
			sprintf(temp,"%.1f ", y * ideal);
		else //if(ideal < 10)
			sprintf(temp,"%.0f ", y * ideal);
		//else
			//sprintf(temp,"%5.0f ", y * ideal);
		pDC->MoveTo(x, y0);
		pDC->LineTo(x, y0 + (rect.bottom - y0) / 10);
		pDC->TextOut(x, y0 + (rect.bottom - y0) / 10, temp);
			//y -= (int)((ideal / ymax) * (double)(y0 - y1));
		y++;
		x = x0 + (int)(( y * ideal / xmax) * (double)(x1 - x0));
	}
	int bullSize = max(2, rect.Width() / 200);
	//draw a line for fd probability data
	double xval, yval;
	x = x0;
	pDC->SelectObject( &penBlue );
	while(x <= x1)
	{
		xval = xmax * ((double)(x - x0) / (double)(x1 - x0));
		yval = 1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0fd + (-1.0 * pDoc->fires->b1fd) * xval));
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		if(x > x0)
			pDC->LineTo(x, y);
		else
			pDC->MoveTo(x, y);
		x++;
	}
	//output average values for fd groups
	pDC->SelectObject(&blueBrush);
	int g;
	for(g = 0; g < pDoc->fires->nFDG; g++)
	{
		x = x0 + (int)((pDoc->fires->xGroupsFD[g].valAvg / xmax) * (double)(x1 - x0));
		yval = ((double)pDoc->fires->xGroupsFD[g].obsHit) / 
			(double)(pDoc->fires->xGroupsFD[g].obsHit + pDoc->fires->xGroupsFD[g].obsNoHit);
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		//Star(pDC, x, y, 5, &penBlue);
		pDC->Ellipse(x - bullSize, y - bullSize, x + bullSize, y + bullSize);
	}
	//lfd line
	pDC->SelectObject( &penMagenta );
	x = x0;
	while(x <= x1)
	{
		xval = xmax * ((double)(x - x0) / (double)(x1 - x0));
		yval = 1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0lfd + (-1.0 * pDoc->fires->b1lfd) * xval));
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		if(x > x0)
			pDC->LineTo(x, y);
		else
			pDC->MoveTo(x, y);

		x++;
	}
	//output average values for Lfd groups
	//pDC->SelectObject(&magentaBrush);
	for(g = 0; g < pDoc->fires->nLFDG; g++)
	{
		x = x0 + (int)((pDoc->fires->xGroupsLFD[g].valAvg / xmax) * (double)(x1 - x0));
		yval = ((double)pDoc->fires->xGroupsLFD[g].obsHit) / 
			(double)(pDoc->fires->xGroupsLFD[g].obsHit + pDoc->fires->xGroupsLFD[g].obsNoHit);
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		POINT pts[5];
		pts[0].x = x - bullSize;
		pts[0].y = y - bullSize;
		pts[1].x = x - bullSize;
		pts[1].y = y + bullSize;
		pts[2].x = x + bullSize;
		pts[2].y = y + bullSize;
		pts[3].x = x + bullSize;
		pts[3].y = y - bullSize;
		pts[4].x = x - bullSize;
		pts[4].y = y - bullSize;
		pDC->Polyline(pts, 5);
	}
	//MFD Line
	x = x0;
	pDC->SelectObject( &penRed);
	while(x <= x1)
	{
		xval = xmax * ((double)(x - x0) / (double)(x1 - x0));
		yval = 1.0 / (1.0 + exp(-1.0 * pDoc->fires->b0mfd + (-1.0 * pDoc->fires->b1mfd) * xval));
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		if(x > x0)
			pDC->LineTo(x, y);
		else
			pDC->MoveTo(x, y);

		x++;
	}
	//output average values for mfd groups
	//pDC->SelectObject(&redBrush);
	POINT pts[5];
	for(g = 0; g < pDoc->fires->nMFDG; g++)
	{
		x = x0 + (int)((pDoc->fires->xGroupsMFD[g].valAvg / xmax) * (double)(x1 - x0));
		yval = ((double)pDoc->fires->xGroupsMFD[g].obsHit) / 
			(double)(pDoc->fires->xGroupsMFD[g].obsHit + pDoc->fires->xGroupsMFD[g].obsNoHit);
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		pts[0].x = x;// - bullSize;
		pts[0].y = y - bullSize;
		pts[1].x = x - bullSize;
		pts[1].y = y;
		pts[2].x = x;
		pts[2].y = y + bullSize;
		pts[3].x = x + bullSize;
		pts[3].y = y;
		pts[4].x = x;// - bullSize;
		pts[4].y = y - bullSize;
		pDC->Polyline(pts, 5);
		//Star(pDC, x, y, 3, &penRed);
	}
	int lw = (x0 - rect.left) / 2;
	x = rect.left + lw / 4;
	CSize ts;
	pDC->SetTextAlign(TA_TOP | TA_LEFT);
	pDC->SelectObject(&axisFont);
	ts = pDC->GetTextExtent("M");
	y = rect.bottom - ts.cy * 5 / 4;
	pDC->SelectObject(&penRed);
	pDC->MoveTo(x, y + ts.cy / 2);
	pDC->LineTo(x + lw - 1, y + ts.cy / 2);
	pDC->TextOut(x + lw + 1, y, "Multi Fire Day");
	pts[0].x = x + (lw + 1) / 2 ;
	pts[0].y = y + ts.cy / 2 - bullSize;
	pts[1].x = pts[0].x - bullSize;
	pts[1].y = pts[0].y + bullSize;
	pts[2].x = pts[0].x;
	pts[2].y = pts[0].y + 2 * bullSize;
	pts[3].x = pts[0].x + bullSize;
	pts[3].y = pts[0].y + bullSize;;
	pts[4].x = pts[0].x;
	pts[4].y = pts[0].y;
	pDC->Polyline(pts, 5);

	y -= ts.cy * 5 / 4;
	pDC->SelectObject(&penMagenta);
	pDC->MoveTo(x, y + ts.cy / 2);
	pDC->LineTo(x + lw - 1, y + ts.cy / 2);
	pDC->TextOut(x + lw + 1, y, "Large Fire Day");
	pts[0].x = x + (lw + 1) / 2 - bullSize;
	pts[0].y = y + ts.cy / 2 - bullSize;
	pts[1].x = pts[0].x;
	pts[1].y = pts[0].y + 2 * bullSize;
	pts[2].x = pts[0].x + 2 * bullSize;
	pts[2].y = pts[0].y + 2 *bullSize;
	pts[3].x = pts[0].x + 2 * bullSize;
	pts[3].y = pts[0].y;
	pts[4].x = pts[0].x;
	pts[4].y = pts[0].y;
	pDC->Polyline(pts, 5);

	y -= ts.cy * 5 / 4;
	pDC->SelectObject(&penBlue);
	pDC->SelectObject(&blueBrush);
	pDC->MoveTo(x, y + ts.cy / 2);
	pDC->LineTo(x + lw - 1, y + ts.cy / 2);
	pDC->TextOut(x + lw + 1, y, "Fire Day");
	CRect eRect;
	eRect.left = x + (lw + 1) / 2 - bullSize;
	eRect.top = y + ts.cy / 2 - bullSize;
	eRect.right = eRect.left + 2 * bullSize;
	eRect.bottom = eRect.top + 2 * bullSize;
	pDC->Ellipse(eRect);
	x = rect.left + rect.Width() / 4;
	y = rect.bottom - ts.cy * 5 / 4;
	CString str;
	str.Format("Multi = %d+", pDoc->fires->mfdFires);
	pDC->TextOut(x, y, str);
	y -= ts.cy * 5 / 4;
	str.Format("Large = %d+", pDoc->fires->lfAcres);
	pDC->TextOut(x, y, str);
	y -= ts.cy * 5 / 4;
	if(pDoc->fires->cause == 0)
		str.Format("Cause = All");
	else if(pDoc->fires->cause == 1)
		str.Format("Cause = Lightning");
	else
		str.Format("Cause = Human");
	pDC->TextOut(x, y, str);
	//output version/DateTime stamp
	y = rect.bottom - ts.cy * 5 / 4;
	CString dStr = pDoc->fires->dateStamp.Format("%m/%d/%Y-%H:%M");
	//str.Format("FF+%s %s", verStr, dStr);
	str.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	//str = pDoc->fires->dateStamp.Format("FF+3.0 %m/%d/%Y-%H:%M");
	CSize xTextSize;
	xTextSize = pDC->GetTextExtent(str);
	pDC->TextOut(x1 - xTextSize.cx , y, str);
	if(pDoc->fires->graphModelString.GetLength() > 0)
	{	//output model code in lower right corner
		y -= ts.cy * 5 / 4;
		CString mStr = "Model: " + pDoc->fires->graphModelString;
		xTextSize = pDC->GetTextExtent(mStr);
		
		pDC->TextOut(x1 - xTextSize.cx , y, mStr);
	}
	//draw class lines
	pDC->SelectObject(&penBlack1);
	pDC->SelectObject(&headerFont);
	if(pDoc->goUp)
		pDC->SetTextAlign(TA_TOP | TA_RIGHT);
	for(int c = 0; c < pDoc->nDP; c++)
	{
		CString cls;
		cls.Format("%d", c + 1);
		x = x0 + (int)((pDoc->lowers[c] / xmax) * (double)(x1 - x0));
		pDC->TextOut(x + 1, y1, cls);
		pDC->MoveTo(x, y0);
		pDC->LineTo(x, y1);
	}
	//restore GDI objects
    pDC->SelectObject( pOldPen );    
    pDC->SelectObject( oldFont );    
	pDC->SelectObject( pOldBrush );    

}

double CDPProbView::IdealFromXMax(double xmax)
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


BOOL CDPProbView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return DoPreparePrinting(pInfo);
}

void CDPProbView::PrintGraph()
{
	theApp.SetLandscape();
	CView::OnFilePrint();
}

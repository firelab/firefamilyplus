// FireStatGraph.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "TermGraph.h"
//#include "fireplusSet.h"
//#include "SIGStationSet.h"
#include "TermData.h"
#include <afxrich.h>
#include "RichTermDoc.h"
#include "TermFrame.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString AddDateStamp();
extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CTermGraph

IMPLEMENT_DYNCREATE(CTermGraph, CView)

CTermGraph::CTermGraph()
{
	sDay = maxDay = 0;
	days = NULL;
	//lastx0 = lastx1 = lasty0 = lasty1 = 0;

}

CTermGraph::~CTermGraph()
{
	if(days)
		delete[] days;
}


BEGIN_MESSAGE_MAP(CTermGraph, CView)
	//{{AFX_MSG_MAP(CTermGraph)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTermGraph drawing

void CTermGraph::OnDraw(CDC* pDC)
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
	//CDocument* pDoc = GetDocument();
	// TODO: add draw code here
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
// CTermGraph diagnostics

#ifdef _DEBUG
void CTermGraph::AssertValid() const
{
	CView::AssertValid();
}

void CTermGraph::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTermGraph message handlers
void CTermGraph::DrawToDCRegion(CDC *pDC, CRect rect)
{
	InitializeDays();

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
	CRichTermDoc* pDoc = (CRichTermDoc*)GetDocument();
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
		|| !penRed.CreatePen( PS_SOLID, 2, RGB(200,0,0)) )    
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
    CPen* pOldPen = pDC->SelectObject( &penBlack2 );
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
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 + 1, termData->staStr);//fires->staStr);
	sprintf(temp, "%d - %d", termData->years[0], termData->years[termData->nDates - 1]);
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 * 2 + 4, temp);
	//sprintf(temp, "%d/%d - %d/%d", fires->start.GetMonth(), fires->start.GetDay(), 
	//	fires->end.GetMonth(), fires->end.GetDay());
	pDC->TextOut(x0 + (x1 - x0) / 2, y1 + 5, "Waiting Time to Term Event");//rect.top + (y1 - rect.top) / 3 * 2 + 4, );

	pDC->TextOut(x0 + (x1 - x0) / 2, y0 + (rect.bottom - y0) / 3 * 2, "Day of Year");
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
		sprintf(temp, "%.1f", ((double)i) / 10.0);
		pDC->TextOut(x0 - (x0 - rect.left)/ 10, y + diff, temp);
	}
	//figure x axis values
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	x = x0;
	CSize xTextSize = pDC->GetTextExtent("M", 1);
	CString str;
	int xlabels = 0;
	//str.Format("%d/%d", termData->startDate.GetMonth(), termData->startDate.GetDay());
	//pDC->TextOut(x, y0 + (rect.bottom - y0) / 10, temp); 
	COleDateTime tDay = termData->startDate;
	COleDateTimeSpan span(1, 0, 0, 0);
	pDC->SelectObject(&penBlack1);
	//pDC->MoveTo(x, y0);
	int step = (double)(x1 - x0) / (double)(maxDay - 1);
	for(int d = 0; d < maxDay; d++)
	{
		x = x0 + (int)(d * step);
		pDC->MoveTo(x, y0);
		if(tDay.GetDay() == 1
				|| maxDay < 12 || d == 0)
		{
			pDC->LineTo(x, y0 + (rect.bottom - y0) / 10);
			str.Format("%d/%d", tDay.GetMonth(), tDay.GetDay());
			if(xlabels % 2 == 0)
				pDC->TextOut(x, y0 + (rect.bottom - y0) / 10, str); 
			else
				pDC->TextOut(x, y0 + (rect.bottom - y0) / 10 + xTextSize.cy, str); 				
			xlabels++;
		}
		else
			pDC->LineTo(x, y0 + (rect.bottom - y0) / 15);
		tDay += span;
	}

	//draw the model output line...
	pDC->SelectObject(&penRed);
	pDC->MoveTo(x0, y0);
	double p;
	for(int d = 0; d < maxDay; d++)
	{
		x = x0 + (int)(d * step);
		p = 1.0 - exp(-(pow((termData->beta * d), termData->alpha)));
		y = y0 - ((double)(y0 - y1)) * p;
		pDC->LineTo(x, y);
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
	pDC->TextOut(x + lw + 1, y, "Estimated");
	

	//output observed term dates (cummulative probability)
	pDC->SelectObject(&penBlue);
	pDC->SelectObject(&blueBrush);
	int radius = 4;
	for(int yr = 0; yr < termData->nDates; yr++)
	{
		int n = termData->streamDays[yr];//termData->dates[yr].GetDayOfYear() - termData->startDate.GetDayOfYear();
		//if(n < 0)
			//n += 365;
		x = x0 + (int)(n * step);
		p = (double)(yr + 1) / (double)(termData->nDates);
		y = y0 - ((double)(y0 - y1)) * p;
		//draw a circle...
		//int extentX = 4;
		//int extentY = 4;//(y0 - y1) / termData->nDates;
		//extentY = max(extentY, 4);
		pDC->Ellipse(x - radius, y - radius, x + radius, y + radius);
	}
	y = rect.bottom - ts.cy * 5 / 4;
	y -= ts.cy * 5 / 4;
	x = rect.left + lw / 4;
	x += (lw + 1) / 2;
	pDC->MoveTo(x, y + ts.cy / 2);
	pDC->Ellipse(x - radius, y - radius, x + radius, y + radius);
	x = rect.left + lw / 4;
	pDC->SetTextAlign(TA_TOP | TA_LEFT);
	pDC->TextOut(x + lw + 1, y - ts.cy/2, "Empirical");

	//add the version/date stamp
	pDC->SetTextAlign(TA_BOTTOM | TA_RIGHT);
	pDC->TextOut(x1, rect.bottom - 5, termData->dateStamp);
	//int z = 0;
	//while((z * ideal) > xmin)
	//		z--;
		//x = 0;
/*	y = z;// * ideal;//ymin;
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
		//x = x0 + (int)(( y * ideal / xmax) * (double)(x1 - x0));
		x = x0 + (int)(( (y - z) * ideal / (xmax - xmin)) * (double)(x1 - x0));
	}*/
/*	int bullSize = max(2, rect.Width() / 200);
	//draw a line for fd probability data
	double xval, yval;
	x = x0;
	pDC->SelectObject( &penBlue );
	while(x <= x1)
	{
		xval = (xmax - xmin) * ((double)(x - x0) / (double)(x1 - x0));
		yval = 1.0 / (1.0 + exp(-1.0 * fires->b0fd + (-1.0 * fires->b1fd) * xval));
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
	for(g = 0; g < fires->nFDG; g++)
	{
		if(fires->xGroupsFD[g].obsHit + fires->xGroupsFD[g].obsNoHit <= 0)
			continue;
		x = x0 + (int)(((fires->xGroupsFD[g].valAvg - xmin) / (xmax - xmin)) * (double)(x1 - x0));
		yval = ((double)fires->xGroupsFD[g].obsHit) / 
			(double)(fires->xGroupsFD[g].obsHit + fires->xGroupsFD[g].obsNoHit);
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		//Star(pDC, x, y, 5, &penBlue);
		pDC->Ellipse(x - bullSize, y - bullSize, x + bullSize, y + bullSize);
	}
	//lfd line
	pDC->SelectObject( &penMagenta );
	x = x0;
	while(x <= x1)
	{
		xval = (xmax - xmin) * ((double)(x - x0) / (double)(x1 - x0));
		yval = 1.0 / (1.0 + exp(-1.0 * fires->b0lfd + (-1.0 * fires->b1lfd) * xval));
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		if(x > x0)
			pDC->LineTo(x, y);
		else
			pDC->MoveTo(x, y);

		x++;
	}
	//output average values for Lfd groups
	//pDC->SelectObject(&magentaBrush);
	for(g = 0; g < fires->nLFDG; g++)
	{
		if(fires->xGroupsLFD[g].obsHit + fires->xGroupsLFD[g].obsNoHit <= 0)
			continue;
		x = x0 + (int)(((fires->xGroupsLFD[g].valAvg - xmin) / (xmax - xmin)) * (double)(x1 - x0));
		yval = ((double)fires->xGroupsLFD[g].obsHit) / 
			(double)(fires->xGroupsLFD[g].obsHit + fires->xGroupsLFD[g].obsNoHit);
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
		xval = (xmax - xmin) * ((double)(x - x0) / (double)(x1 - x0));
		yval = 1.0 / (1.0 + exp(-1.0 * fires->b0mfd + (-1.0 * fires->b1mfd) * xval));
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
	for(g = 0; g < fires->nMFDG; g++)
	{
		if(fires->xGroupsMFD[g].obsHit + fires->xGroupsMFD[g].obsNoHit <= 0)
			continue;
		x = x0 + (int)(((fires->xGroupsMFD[g].valAvg - xmin) / (xmax - xmin)) * (double)(x1 - x0));
		yval = ((double)fires->xGroupsMFD[g].obsHit) / 
			(double)(fires->xGroupsMFD[g].obsHit + fires->xGroupsMFD[g].obsNoHit);
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
	str.Format("Multi = %d+", fires->mfdFires);
	pDC->TextOut(x, y, str);
	y -= ts.cy * 5 / 4;
	str.Format("Large = %d+", fires->lfAcres);
	pDC->TextOut(x, y, str);
	y -= ts.cy * 5 / 4;
	if(fires->cause == 0)
		str.Format("Cause = All");
	else if(fires->cause == 1)
		str.Format("Cause = Lightning");
	else
		str.Format("Cause = Human");
	pDC->TextOut(x, y, str);
	//output version/DateTime stamp
	y = rect.bottom - ts.cy * 5 / 4;
	CString dStr = fires->dateStamp.Format("%m/%d/%Y-%H:%M");
	str.Format("FF+%s %s", verStr, dStr);
	//str = fires->dateStamp.Format("FF+3.0 %m/%d/%Y-%H:%M");
	CSize xTextSize;
	xTextSize = pDC->GetTextExtent(str);
	pDC->TextOut(x1 - xTextSize.cx , y, str);
	if(fires->graphModelString.GetLength() > 0)
	{	//output model code in lower right corner
		y -= ts.cy * 5 / 4;
		CString mStr = "Model: " + fires->graphModelString;
		xTextSize = pDC->GetTextExtent(mStr);
		
		pDC->TextOut(x1 - xTextSize.cx , y, mStr);
	}*/

	//restore GDI objects
    pDC->SelectObject( pOldPen );    
    pDC->SelectObject( oldFont );    
	pDC->SelectObject( pOldBrush );    
}

/*double CTermGraph::IdealFromXMax(double xmax)
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
}*/



void CTermGraph::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CTermFrame *frame = (CTermFrame *)GetParent()->GetParent();
	frame->MaximizePane(this);
}

void CTermGraph::OnFilePrint() 
{
	// TODO: Add your command handler code here
	CView::OnFilePrint();	
	
}

BOOL CTermGraph::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	
	return DoPreparePrinting(pInfo);
	//return CView::OnPreparePrinting(pInfo);
}

void CTermGraph::PrintGraph()
{
	theApp.SetLandscape();
	OnFilePrint();
}

/*void CTermGraph::Star(CDC *pDC, int x, int y, int starSize, CPen *pen)
{
	int lWid = starSize / 5, s = starSize;
	if(lWid <= 1)
		lWid++;
	if(starSize % 2 != 0)
		s++;
	CPen *oldPen = pDC->SelectObject(pen);
	pDC->MoveTo(x - s, y);
	pDC->LineTo(x + s, y);
	pDC->MoveTo(x - s / 2, y - s * 2 / 3);
	pDC->LineTo(x + s / 2, y + s * 2 / 3);
	pDC->MoveTo(x - s / 2, y + s * 2 / 3);
	pDC->LineTo(x + s / 2, y - s * 2 / 3);

	pDC->SelectObject(oldPen);
}*/
void CTermGraph::InitializeDays()
{
	if(days != NULL)
		return;
	// want to figure extent of x axis only once, do it here
	sDay = termData->startDate.GetDayOfYear();
	days = new int[termData->nDates];
	int maxRealDay = 0;
	for(int d = 0; d < termData->nDates; d++)
	{
		days[d] = termData->dates[d].GetDayOfYear() - sDay;
		if(days[d] < 0)//must wrap year end
			days[d] += 365;
		maxRealDay = max(maxRealDay, days[d]);
	}
	//now plug into Wiebull equation to figureout when probability reaches 100%
	double p;
	maxDay = maxRealDay;
	for(int d = maxRealDay; d < 365; d++)
	{
		p = 1.0 - exp(-(pow((termData->beta * d), termData->alpha)));
		maxDay = maxRealDay + d;
		if(p >= 0.9901)
			break; //hit it, break out
	}
	maxDay = (maxDay - maxRealDay) * 1.1;
	maxDay = min(maxDay, 365);
}

void CTermGraph::OnInitialUpdate()
{
	CView::OnInitialUpdate();


}

void CTermGraph::OnMouseMove(UINT nFlags, CPoint point)
{
	CWnd *pWnd = GetParent()->GetParent();
	if(!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CTermFrame)))
		return;
	CTermFrame *frame = (CTermFrame *)GetParent()->GetParent();
	CRect rect;
	GetClientRect(rect);
	rect.NormalizeRect();
	CString xtext = "", ytext = "";
	if(rect.Width() > 0 || rect.Height() > 0)
	{//figure values
		int x0, x1, y0, y1, y, x;
		x0 = rect.left + rect.Width() / 10;
		x1 = rect.right - (x0 - rect.left)/ 10;
		y1 = rect.top + rect.Height() / 10;
		y0 = rect.bottom - 2 * (y1 - rect.top);
		COleDateTime tDate;
		if(point.x >= x0 || point.x <= x1 )
		{
			if(maxDay <= 0)
			{
				xtext = "NA";
				ytext = "0.00";
			}
			else
			{
				int d;
				if(maxDay > 1)
				{
					int step = (double) (x1 - x0) / (double)(maxDay - 1);
					d = ((double)(point.x - x0)) / step;
				}
				else
					d = 0;
				if(d < 0)
					d = 0;
				if(d >= maxDay)
					d = maxDay - 1;
				COleDateTimeSpan span((int)d, 0, 0, 0);
				tDate = termData->startDate + span;
				xtext = tDate.Format("%B %d");
				double prob = 1.0 - exp(-(pow((termData->beta * d), termData->alpha))); 
				ytext.Format("%.2f", prob);
			}
			frame->m_wndStatusBar.SetPaneText(2, xtext);
			frame->m_wndStatusBar.SetPaneText(4, ytext);
		}
	}

	CView::OnMouseMove(nFlags, point);
}

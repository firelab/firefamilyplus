// FireStatGraph.cpp : implementation file
//
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "FireStatGraph.h"
#include "fireplusSet.h"
#include "SIGStationSet.h"
#include "FireStats.h"
#include <afxrich.h>
#include "RichFireDoc.h"
#include "FireStatFrame.h"
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
// CFireStatGraph

IMPLEMENT_DYNCREATE(CFireStatGraph, CView)

CFireStatGraph::CFireStatGraph()
{
}

CFireStatGraph::~CFireStatGraph()
{
}


BEGIN_MESSAGE_MAP(CFireStatGraph, CView)
	//{{AFX_MSG_MAP(CFireStatGraph)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireStatGraph drawing
/*void CFireStatGraph::DrawPathOutline(const COLORREF& c, CDC& dc, bool Round, int PenSize)
{
	// make pen and stroke path
	LOGBRUSH lbrush;
	lbrush.lbStyle = BS_SOLID;
	lbrush.lbColor = c;
	CPen Pen(PS_GEOMETRIC | PS_SOLID | 
				(Round ? 
				(PS_JOIN_ROUND | PS_ENDCAP_ROUND) :
				(PS_JOIN_MITER | PS_ENDCAP_FLAT)), 
				PenSize, &lbrush);
	CPen *pOldPen = dc.SelectObject(&Pen);
	if(PenSize > 1 && dc.IsPrinting())
		dc.WidenPath();
	dc.StrokePath();
	dc.SelectObject(pOldPen);
}*/


void CFireStatGraph::OnDraw(CDC* pDC)
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
// CFireStatGraph diagnostics

#ifdef _DEBUG
void CFireStatGraph::AssertValid() const
{
	CView::AssertValid();
}

void CFireStatGraph::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFireStatGraph message handlers
void CFireStatGraph::DrawToDCRegion(CDC *pDC, CRect rect)
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
	CRichFireDoc* pDoc = (CRichFireDoc*)GetDocument();
	//figure axis
	int x0, x1, y0, y1, y, x;
	x0 = rect.left + rect.Width() / 10;
	x1 = rect.right - (x0 - rect.left)/ 10;
	y1 = rect.top + rect.Height() / 10;
	y0 = rect.bottom - 2 * (y1 - rect.top);

	COLORREF colorBlue = RGB(0, 200, 200);
	COLORREF colorMagenta = RGB(200,0,200);
	COLORREF colorRed = RGB(200,0,0);
	//load pens and fonts
    CPen penBlack2, penBlack1, penRed, penBlue, penMagenta;  // Construct it, then initialize
    if(!penBlack2.CreatePen( PS_SOLID, 2, RGB(0,0,0)) 
		|| !penBlack1.CreatePen( PS_SOLID, 1, RGB(0,0,0))     
		|| !penBlue.CreatePen( PS_SOLID, 2, RGB(0,200,200))     
		|| !penMagenta.CreatePen( PS_DOT, 2, RGB(200,0,200))     
		|| !penRed.CreatePen( PS_DASH, 2, RGB(200,0,0)) )    
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
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 + 1, fires->staStr);
	if(fires->end.GetYear() > fires->start.GetYear())
		sprintf(temp, "%d - %d", fires->start.GetYear(),
			fires->end.GetYear());
	else
		sprintf(temp, "%d", fires->end.GetYear());
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 * 2 + 4, temp);
	//time of year header
	sprintf(temp, "%d/%d - %d/%d", fires->start.GetMonth(), fires->start.GetDay(), 
		fires->end.GetMonth(), fires->end.GetDay());
	pDC->TextOut(x0 + (x1 - x0) / 2, y1 + 5, temp);//rect.top + (y1 - rect.top) / 3 * 2 + 4, temp);

	pDC->TextOut(x0 + (x1 - x0) / 2, y0 + (rect.bottom - y0) / 3 * 2, fires->varName);
	pDC->SelectObject(&vheaderFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);

	pDC->TextOut(rect.left + (x0 - rect.left) / 3, (y1 + y0) / 2, (fires->m_conditional == false) ? "Probability" : "Conditional Probability");
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
	double ideal, xmax, xmin;
	xmax = fires->nv > 0 ? fires->allVals[fires->nv - 1] : 1.0;
	if(xmax == 0)
		xmax = 1.0;
	xmin = min(0.0, fires->nv > 0 ? fires->allVals[0] : 0.0);
	ideal = IdealFromXMax(xmax - xmin);
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	//for(int i = 1; i 
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
	}
	int bullSize = max(2, rect.Width() / 200);
	//draw a line for fd probability data
	int brushWidth = 3;
	//CBrush brush1, brush2, brush3;
	pDC->SelectObject( pOldBrush ); 
//	brush1.CreateSolidBrush(colorBlue);
//	pDC->SelectObject(&brush1);
	unsigned auxType[8];
	bool Round = true;
	if(pDC->IsPrinting())
	{
		CDC *screenDC = GetDC();
		brushWidth = ScreenPixelsToPrinterPixels(screenDC, pDC, brushWidth);
		ReleaseDC(screenDC);
	}
	int c1 = CDashLine::GetPattern(auxType, Round, brushWidth, 
		CDashLine::DL_SOLID);
	CDashLine auxLine(*pDC, auxType, c1);



	double xval, yval;
	x = x0;
	//pDC->SelectObject( &penBlue );
	pDC->BeginPath();
	while(x <= x1)
	{
		xval = (xmax - xmin) * ((double)(x - x0) / (double)(x1 - x0));
		yval = 1.0 / (1.0 + exp(-1.0 * fires->b0fd + (-1.0 * fires->b1fd) * xval));
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		if(x > x0)
			auxLine.LineTo(x, y);
			//pDC->LineTo(x, y);
		else
			auxLine.MoveTo(x, y);
			//pDC->MoveTo(x, y);
		x++;
	}
	pDC->EndPath();
	DrawPathOutline(colorBlue, *pDC, Round, brushWidth);
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
	pDC->SelectObject( pOldBrush ); 
	//brush.DeleteObject();
//	brush2.CreateSolidBrush(colorMagenta);
	c1 = CDashLine::GetPattern(auxType, Round, brushWidth, 
		CDashLine::DL_DASH);
	CDashLine auxLine2(*pDC, auxType, c1);
	//pDC->SelectObject( &penMagenta );
	pDC->BeginPath();
	x = x0;
	while(x <= x1)
	{
		xval = (xmax - xmin) * ((double)(x - x0) / (double)(x1 - x0));
		yval = 1.0 / (1.0 + exp(-1.0 * fires->b0lfd + (-1.0 * fires->b1lfd) * xval));
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		if(x > x0)
			auxLine2.LineTo(x, y);
		else
			auxLine2.MoveTo(x, y);

		x++;
	}
	pDC->EndPath();
	DrawPathOutline(colorMagenta, *pDC, Round, brushWidth);
	//output average values for Lfd groups
	//pDC->SelectObject(&magentaBrush);
	pDC->SelectObject( &penMagenta );
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
	pDC->SelectObject( pOldBrush ); 
	//brush.DeleteObject();
//	brush3.CreateSolidBrush(colorRed);
	c1 = CDashLine::GetPattern(auxType, Round, brushWidth, 
		CDashLine::DL_DOT);
	CDashLine auxLine3(*pDC, auxType, c1);
	//pDC->SelectObject( &penMagenta );
	pDC->BeginPath();
	x = x0;
	//pDC->SelectObject( &penRed);
	while(x <= x1)
	{
		xval = (xmax - xmin) * ((double)(x - x0) / (double)(x1 - x0));
		yval = 1.0 / (1.0 + exp(-1.0 * fires->b0mfd + (-1.0 * fires->b1mfd) * xval));
		y = y0 - (int)(yval * (double)(y0 - y1)); 
		if(x > x0)
			auxLine3.LineTo(x, y);
		else
			auxLine3.MoveTo(x, y);

		x++;
	}
	pDC->EndPath();
	DrawPathOutline(colorRed, *pDC, Round, brushWidth);
	//output average values for mfd groups
	pDC->SelectObject(pOldBrush);
	//brush.DeleteObject();
	pDC->SelectObject( &penRed);
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
	pDC->BeginPath();
	auxLine3.MoveTo(x, y + ts.cy / 2);
	auxLine3.LineTo(x + lw - 1, y + ts.cy / 2);
	pDC->EndPath();
	DrawPathOutline(colorRed, *pDC, Round, 3);
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
	pDC->BeginPath();
	auxLine2.MoveTo(x, y + ts.cy / 2);
	auxLine2.LineTo(x + lw - 1, y + ts.cy / 2);
	pDC->EndPath();
	DrawPathOutline(colorMagenta, *pDC, Round, 3);
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
	pDC->BeginPath();
	auxLine.MoveTo(x, y + ts.cy / 2);
	auxLine.LineTo(x + lw - 1, y + ts.cy / 2);
	pDC->EndPath();
	DrawPathOutline(colorBlue, *pDC, Round, 3);
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
	else {
		str.Format("Cause = Human");
		if (fires->humanCause != 1022)
		{
			 str.Format("Cause = ");
			 	for (int i=2; i <= 20; i++)
				{
					int testFlag = (fires->humanCause >> (i-1)) & 1;
					if (testFlag == 1)
					{
						CString tempstr;
						tempstr.Format("%d ", i);
						str.Append(tempstr);
					}
				}
		}
	}
	pDC->TextOut(x, y, str);
	//output version/DateTime stamp
	y = rect.bottom - ts.cy * 5 / 4;
	CString dStr = fires->dateStamp.Format("%m/%d/%Y-%H:%M");
	//str.Format("FF+%s %s", verStr, dStr);
	str.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
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
	}
//	brush1.DeleteObject();
//	brush2.DeleteObject();
//	brush3.DeleteObject();

	//restore GDI objects
    pDC->SelectObject( pOldPen );    
    pDC->SelectObject( oldFont );    
	pDC->SelectObject( pOldBrush );    
}

double CFireStatGraph::IdealFromXMax(double xmax)
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



void CFireStatGraph::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//CFireStatFrame *frame = (CFireStatFrame *)GetParent()->GetParent();
	//frame->MaximizePane(this);
	CWnd *pWnd = GetParent()->GetParent();
	if(pWnd)
	{
		if(pWnd->IsKindOf(RUNTIME_CLASS(CFireStatFrame)))
			((CFireStatFrame *)pWnd)->MaximizePane(this);
	}
}

void CFireStatGraph::OnFilePrint() 
{
	// TODO: Add your command handler code here
	CView::OnFilePrint();	
	
}

BOOL CFireStatGraph::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	
	return DoPreparePrinting(pInfo);
	//return CView::OnPreparePrinting(pInfo);
}

void CFireStatGraph::PrintGraph()
{
	theApp.SetLandscape();
	OnFilePrint();
}

/*void CFireStatGraph::Star(CDC *pDC, int x, int y, int starSize, CPen *pen)
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

void CFireStatGraph::OnLButtonUp(UINT nFlags, CPoint point)
{
	CFireStatFrame *frame1 = 0;
	CBatchOutputFrame *frame2 = 0;
	CWnd *pWnd = GetParent()->GetParent();
	if(!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CFireStatFrame)))
	{
		pWnd = GetParent()->GetParent()->GetParent();
		if(!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CBatchOutputFrame)))
			return;
		else
			frame2 =(CBatchOutputFrame *) pWnd;
	}
	else
		frame1 = (CFireStatFrame *)GetParent()->GetParent();

	double xmax = 1.0, xmin = 0.0;
	if(fires->nv > 0)
	{
		xmax = fires->allVals[fires->nv - 1];
		xmin = min(0.0, fires->allVals[0]);
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
			if(fires->varID == 9)
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

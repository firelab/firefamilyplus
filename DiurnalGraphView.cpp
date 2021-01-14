// DiurnalGraphView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DiurnalDoc.h"
#include "DiurnalFrame.h"
#include "DiurnalGraphView.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CDiurnalGraphView

IMPLEMENT_DYNCREATE(CDiurnalGraphView, CView)

CDiurnalGraphView::CDiurnalGraphView()
{
}

CDiurnalGraphView::~CDiurnalGraphView()
{
}


BEGIN_MESSAGE_MAP(CDiurnalGraphView, CView)
	//{{AFX_MSG_MAP(CDiurnalGraphView)
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiurnalGraphView drawing

void CDiurnalGraphView::OnDraw(CDC* pDC)
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
// CDiurnalGraphView diagnostics

#ifdef _DEBUG
void CDiurnalGraphView::AssertValid() const
{
	CView::AssertValid();
}

void CDiurnalGraphView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDiurnalGraphView message handlers

void CDiurnalGraphView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	CDiurnalDoc* pDoc = (CDiurnalDoc*)GetDocument();
	CDiurnalFrame *frame = (CDiurnalFrame *)GetParent()->GetParent();
	int varID = frame->varID;
	ymax = 0.1;
	int d;
	for(d = 0; d < 366; d++)
	{
		for(int h = 0; h < 24; h++)
		{
			if(pDoc->da->days[varID][d].hours[h].N() > 0)
				ymax = max(ymax, pDoc->da->days[varID][d].hours[h].Mean());
		}
	}
	if(varID == 23)
	{
		ideal = 1.0;
		ymax = 9.0;
	}
	else
		ideal = IdealFromYMax();//ymax / 10.0;
	int h;
	for(h = 0; h < 24; h++)
	{
		for(d = 0; d < 366; d++)
		{
			if(pDoc->da->days[varID][d].hours[h].N() > 0 && varID != 23)
				hours[h].Accumulate(pDoc->da->days[varID][d].hours[h].Mean());
			else if(pDoc->da->days[varID][d].hours[h].N() > 0)//wind direction
			{//wind dir needs mode value....use all values to get it
				for(int v = 0; v < pDoc->da->days[varID][d].hours[h].N(); v++)
					hours[h].Accumulate(pDoc->da->days[varID][d].hours[h].vals[v]);
			}
		}
		hours[h].Complete();
	}
	for(h = 0; h < 24; h++)
	{
		//if(varID == 23)//set the mode for WindDir
		//	hours[h].SetMeanToMode();
		hours[h].CleanStorage();
	}
}

void CDiurnalGraphView::DrawToDCRegion(CDC * destDC, CRect rect)
{
	if(destDC->IsPrinting())
	{
		int lx, ly;
		ly = destDC->GetDeviceCaps(VERTRES);
		lx = destDC->GetDeviceCaps(HORZRES);
		rect.left = 0;
		rect.top = 0;
		rect.right = lx;
		rect.bottom = ly;
	}
	long recsUsed = 0;
	CString temp;
	int x0, x1, y0, y1, x, y;//, t, b, r, l;
	CDiurnalDoc* pDoc = (CDiurnalDoc*)GetDocument();
	CDiurnalFrame *frame = (CDiurnalFrame *)GetParent()->GetParent();
	int varID = frame->varID;

	CDC memDC;
	CDC* pDC = &memDC;
	CRect clientRect(rect);
	CBitmap bmp;
	CBitmap* pOldBmp;
	if(!destDC->IsPrinting())
	{
		pDC->CreateCompatibleDC(destDC);
		bmp.CreateCompatibleBitmap(destDC, rect.Width(), rect.Height());
		pOldBmp = pDC->SelectObject(&bmp);
		pDC->FillSolidRect(&rect, PALETTERGB(255,255,255));
	}
	else
		pDC = destDC;

	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);

	//figure axis
	x0 = rect.left + rect.Width() / 8;
	x1 = rect.right - (x0 - rect.left)/ 10;
	y1 = rect.top + rect.Height() / 10;
	y0 = rect.bottom - 2 * (y1 - rect.top);

	//draw axis
    CPen penBlack2, penBlack1, penGray, penRed, penHGray, penVGray;  // Construct it, then initialize
    if(!penBlack2.CreatePen( PS_SOLID, 2, RGB(0,0,0)) 
		|| !penBlack1.CreatePen( PS_SOLID, 1, RGB(0,0,0))     
		|| !penRed.CreatePen( PS_SOLID, 1, RGB(0xCC,0,0))     
		|| !penGray.CreatePen( PS_SOLID, 1, RGB(200,200,200))    
		)    
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
			_T("ERROR: Unable to create graph fonts.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CBrush grayBrush, whiteBrush, blackBrush;
	if(!grayBrush.CreateSolidBrush(0x00DDDDDD)
		|| !whiteBrush.CreateSolidBrush(0x00FFFFFF)
		|| !blackBrush.CreateSolidBrush(0x00000000))
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create brushes.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CBrush *pOldBrush = pDC->SelectObject(&whiteBrush);
    CPen* pOldPen = pDC->SelectObject( &penBlack1 );
	//force black bounding rectangle....(grrrr)
	pDC->Rectangle(rect);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.top);
	pDC->LineTo(rect.left, rect.top);

	//draw axis
	pDC->SelectObject( &penBlack2 );
	pDC->MoveTo(x1, y0);
	pDC->LineTo(x0, y0);
	pDC->LineTo(x0, y1);
	//output headers
	CFont *pOldFont = pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);
	//HEADERS
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 + 1, pDoc->stationName);
	if(pDoc->start.GetYear() != pDoc->end.GetYear())
		temp.Format("%d - %d", pDoc->start.GetYear(),
			pDoc->end.GetYear());
	else
		temp.Format("%d", pDoc->start.GetYear());
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 * 2 + 4, temp);
	temp.Format("%d/%d - %d/%d", pDoc->start.GetMonth(), pDoc->start.GetDay(),
		pDoc->end.GetMonth(), pDoc->end.GetDay());
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) + 4, temp);
	temp.Format("Hour of Day");
	pDC->TextOut(x0 + (x1 - x0) / 2, y0 + (rect.bottom - y0) / 3 * 2, temp);
	pDC->SelectObject(&vheaderFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	//pDC->TextOut(x0 / 3, (y1 + y0) / 2, frame->varName);
	pDC->TextOut(rect.left + (x0 - rect.left) / 3, (y1 + y0) / 2, 
		(varID == 23) ? "Mode " + frame->varName : "Avg " + frame->varName);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	
	//figure y extent
	//figure y axis values
	//output the y axis values
	pDC->SelectObject(&axisFont);
	pDC->SetTextAlign(TA_BASELINE | TA_RIGHT);
	pDC->SelectObject(&penBlack1);
	y = y0;
	x = 0;
	if(varID != 23)
	{
		while( y >= y1 )
		{
			if(ideal < 1.0)
				temp.Format("%.1f ", x * ideal);
			else if(ideal < 10)
				temp.Format("%.0f ", x * ideal);
			else
				temp.Format("%5.0f ", x * ideal);
			pDC->MoveTo(x0 - (x0 - rect.left)/10, y);
			pDC->LineTo(x0, y);
			pDC->TextOut(x0 - (x0 - rect.left)/10, y, temp);
			x++;
			y = y0 - (int)(( x * ideal / ymax) * (double)(y0 - y1));
		}
	}
	else
	{///wind direction...
		CString wDir;
		for(int d = 0; d < 9; d++)
		{
			switch(d)
			{
			case 0:
				wDir = "Calm";
				break;
			case 8:
				wDir = "N";
				break;
			case 1:
				wDir = "NE";
				break;
			case 2:
				wDir = "E";
				break;
			case 3:
				wDir = "SE";
				break;
			case 4:
				wDir = "S";
				break;
			case 5:
				wDir = "SW";
				break;
			case 6:
				wDir = "W";
				break;
			case 7:
				wDir = "NW";
				break;
			}
			pDC->MoveTo(x0 - (x0 - rect.left)/10, y);
			pDC->LineTo(x0, y);
			pDC->TextOut(x0 - (x0 - rect.left)/10, y, wDir);
			x++;
			y = y0 - (int)(( x * ideal / ymax) * (double)(y0 - y1));
		}
	}
	int xstep = (x1 - x0) / 25;
	x = x0 + xstep / 2;
	pDC->SelectObject(&penBlack1);
	pDC->SelectObject(&axisFont);
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	int h;
	for(h = 0; h < 24; h++)
	{
		pDC->MoveTo(x, y0);
		pDC->LineTo(x, y0 + (rect.bottom - y0) / 10);
		if(h % 2 == 0)
		{
			temp.Format("%d", h);
			pDC->TextOut(x, y0 + (rect.bottom - y0) / 10, temp); 
		} 				
		x += xstep;
	}
	//draw line
	x = x0 + xstep / 2;
//	Stats hour;
//	for(int d = 0; d < 366; d++)
//	{
//		if(pDoc->da->days[varID][d].hours[0].N() > 0)
//			hour.Accumulate(pDoc->da->days[varID][d].hours[0].Mean());
//	}
//	hour.Complete();
	if(varID != 23)
		y = y0 - (int)(hours[0].Mean() / ymax * (double)(y0 - y1));
	else
		y = y0 - (int)(hours[0].mode / ymax * (double)(y0 - y1));
	pDC->MoveTo(x, y);
	for(h = 1; h < 24; h++)
	{
		//hour.Reset();
		//for(d = 0; d < 366; d++)
		//{
		//	if(pDoc->da->days[varID][d].hours[h].N() > 0)
		//		hour.Accumulate(pDoc->da->days[varID][d].hours[h].Mean());
		//}
		//hour.Complete();
		if(varID != 23)
			y = y0 - (int)(hours[h].Mean() / ymax * (double)(y0 - y1));
		else
			y = y0 - (int)(hours[h].mode / ymax * (double)(y0 - y1));
		x += xstep;
		//y = y0 - pDoc->da->days[varID][k].hours[h].Mean() / ymax * (double)(y0 - y1);
		pDC->LineTo(x, y);
	}
//	hour.Reset();
	CSize xTextSize;
	pDC->SetTextAlign(TA_BOTTOM | TA_LEFT);
	if(OptionFromVarID(frame->varID) == 1 && pDoc->model.GetLength() > 0)
	{	//output model code in lower right corner
		CString mStr = "Model: " + pDoc->model;
		xTextSize = pDC->GetTextExtent(mStr);
		
		pDC->TextOut(x1 - xTextSize.cx , y0 + xTextSize.cy * 4 /*+ xTextSize.cy / 2*/, mStr);
	}
	//output # observations in lower right corner
	temp.Format("%ld Wx Observations", pDoc->nWxObs);
	xTextSize = pDC->GetTextExtent(temp);
	pDC->TextOut(x1 - xTextSize.cx , y0 + xTextSize.cy * 5 /*+ xTextSize.cy / 2*/, temp);
	//output version/DateTime stamp
	CString dStr = pDoc->dateTime.Format("%m/%d/%Y-%H:%M");
	//temp.Format("FF+%s %s", verStr, dStr);
	temp.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	//temp = pDoc->dateTime.Format("FF+3.0 %m/%d/%Y-%H:%M");
	xTextSize = pDC->GetTextExtent(temp);
	pDC->TextOut(x1 - xTextSize.cx , y0 + xTextSize.cy * 6 /*+ xTextSize.cy / 2*/, temp);
	//restore drawing tools to original state
    pDC->SelectObject( pOldPen );    
    pDC->SelectObject( pOldFont );    
	pDC->SelectObject( pOldBrush );    
	if(!destDC->IsPrinting())
	{
		// bit from memdc to paintdc
		destDC->BitBlt(clientRect.left, clientRect.top, clientRect.Width(), clientRect.Height(), pDC, 0, 0, SRCCOPY);
		pDC->SelectObject(pOldBmp);
	}

}

double CDiurnalGraphView::IdealFromYMax()
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


void CDiurnalGraphView::PrintGraph()
{
	theApp.SetLandscape();
	CView::OnFilePrint();
}

BOOL CDiurnalGraphView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return DoPreparePrinting(pInfo);
	
	//return CView::OnPreparePrinting(pInfo);
}

void CDiurnalGraphView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CDiurnalFrame *frame = (CDiurnalFrame *)GetParent()->GetParent();
	frame->MaximizePane(this);
	
//	CView::OnLButtonDblClk(nFlags, point);
}

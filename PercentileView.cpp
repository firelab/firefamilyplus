// PercentileView.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "PercentileView.h"
#include "wxSet.h"
#include "SplitterFrame.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include <math.h>
#include "climateSet.h"
#include "SIGStationSet.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"
#include ".\percentileview.h"
#include "GraphSaveDialog.h"
#include <direct.h>
#include "BatchOutputFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPercentileView

IMPLEMENT_DYNCREATE(CPercentileView, CView)

CPercentileView::CPercentileView()
{
	filterValue = -999;
	filterDirection = FILTER_LESSTHAN_EQUAL;
	nFiltered = 0;
}

CPercentileView::~CPercentileView()
{
}


BEGIN_MESSAGE_MAP(CPercentileView, CView)
	//{{AFX_MSG_MAP(CPercentileView)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	//ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPercentileView drawing

void CPercentileView::OnDraw(CDC* pDC)
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
	}
	if(rect.Width() == 0 || rect.Height() == 0)
		return;
	DrawToDCRegion(pDC, rect);*/
}

/////////////////////////////////////////////////////////////////////////////
// CPercentileView diagnostics

#ifdef _DEBUG
void CPercentileView::AssertValid() const
{
	CView::AssertValid();
}

void CPercentileView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPercentileView message handlers

void CPercentileView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	//fill vals for each percent based on sorted values
	nFiltered = 0;
	if(varID != 8)
	{
		for(int p = 0; p < m_pAnalysis->numPeriods; p++)
			for(int y = 0; y < m_pAnalysis->periods[varID][p]->years; y++)
				for (int v = 0; v < m_pAnalysis->periods[varID][p]->yStats[y].N(); v++)
				{
					if (filterValue != -999.0)
					{
						if (filterDirection == FILTER_LESSTHAN_EQUAL
							&& m_pAnalysis->periods[varID][p]->yStats[y].vals[v] <= filterValue)
						{
							nFiltered++;
							continue;
						}
						else if (filterDirection == FILTER_GREATERTHAN_EQUAL
							&& m_pAnalysis->periods[varID][p]->yStats[y].vals[v] >= filterValue)
						{
							nFiltered++;
							continue;
						}
					}
					stats.Accumulate(m_pAnalysis->periods[varID][p]->yStats[y].vals[v]);
				}
	}
	else
	{
		Period **sums = new Period *[m_pAnalysis->numPeriods];
		long minPrecipObs = 0;//GetMinPrecipObs(m_pAnalysis->m_opts., COleDateTime end);
		int p;
		for(p = 0; p < m_pAnalysis->numPeriods; p++)
		{
			sums[p] = new Period(m_pAnalysis->periods[varID][p]->start, m_pAnalysis->periods[varID][p]->end);
		}
		for(p = 0; p < m_pAnalysis->numPeriods; p++)
		{
			minPrecipObs = GetMinPrecipObs(m_pAnalysis->periods[varID][p]->start, m_pAnalysis->periods[varID][p]->end);
			for(int y = 0; y < m_pAnalysis->periods[varID][p]->years; y++)
			{
				if(m_pAnalysis->periods[varID][p]->yStats[y].N() >= minPrecipObs)
				{
					COleDateTime pDate;
					pDate.SetDate(m_pAnalysis->periods[varID][p]->start.GetYear() + y, m_pAnalysis->periods[varID][p]->start.GetMonth(),
						m_pAnalysis->periods[varID][p]->start.GetDay());
					sums[p]->Accumulate(pDate, m_pAnalysis->periods[varID][p]->yStats[y].Mean() 
						* m_pAnalysis->periods[varID][p]->yStats[y].N());
				}
			}
			sums[p]->Complete();
		}
		for(p = 0; p < m_pAnalysis->numPeriods; p++)
			for(int y = 0; y < sums[p]->years; y++)
				for(int v = 0; v < sums[p]->yStats[y].N(); v++)
					stats.Accumulate(sums[p]->yStats[y].vals[v]);
		for(p = m_pAnalysis->numPeriods - 1; p >= 0; p--)
			delete sums[p];
		delete[] sums;
	}
	stats.Complete();
	xmax = stats.Percentile(1.0);
	xmin = 0.0;
	xmin = min(0.0, stats.Percentile(0.0)); 
	if(xmax == 0.0)
		xmax = 1.0;
	ideal = IdealFromXMax(xmax - xmin);
}

double CPercentileView::IdealFromXMax(double xMax)
{
	if(xMax < 0.15)
		return 0.01;
	else if(xMax < 0.6)
		return 0.05;
	else if(xMax < 1.25)
		return 0.1;
	else if(xMax < 2.5)
		return 0.25;
	else if(xMax < 5.0)
		return 0.5;
	else if(xMax < 10.0)
		return 1.0;
	else if(xMax < 25.0)
		return 2.0;
	else if(xMax < 50.0)
		return 5.0;
	else if(xMax < 100.0)
		return 10.0;
	else if(xMax < 250.0)
		return 20.0;
	else if(xMax < 500.0)
		return 50.0;
	else if(xMax < 1000.0)
		return 100.0;
	else if(xMax < 2500.0)
		return 200.0;
	else if(xMax < 5000.0)
		return 500.0;
	else if(xMax < 10000.0)
		return 1000.0;
	//way big
	int power = (int)log10( xMax / 10.0 );
	if(power > 0)
	{
		double step = floor( (xMax / 10.0) / pow(10.0, power) );
		return floor( step * pow(10.0, power ));
	}

	return xMax / 10;
}

void CPercentileView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//frame->MaximizePane(this);
	CWnd *pWnd = GetParent()->GetParent();
	if(pWnd)
	{
		if(pWnd->IsKindOf(RUNTIME_CLASS(CSplitterFrame)))
			((CSplitterFrame *)pWnd)->MaximizePane(this);
	}
}

void CPercentileView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CView::OnLButtonUp(nFlags, point);
	CSplitterFrame *frame1 = 0;
	CBatchOutputFrame *frame2 = 0;
	CWnd *pWnd = GetParent()->GetParent();
	if(!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CSplitterFrame)))
	{
		pWnd = GetParent()->GetParent()->GetParent();
		if(!pWnd || !pWnd->IsKindOf(RUNTIME_CLASS(CBatchOutputFrame)))
			return;
		else
			frame2 =(CBatchOutputFrame *) pWnd;
	}
	else
		frame1 = (CSplitterFrame *)GetParent()->GetParent();

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
			double x = ((double)(point.x - x0)) / ((double)(x1 - x0)) * (xmax - xmin) + xmin;
			if(varID == 8 || varID == 39)
				sprintf(xtext, "%.2f", x);
			else
				sprintf(xtext, "%.0f", x);
		}
		if(point.y > y0 || point.y < y1)
			strcpy_s(ytext, "NA");
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

void CPercentileView::PrintToDC(CDC * pDC)
{
	OnDraw(pDC);
}

void CPercentileView::OnFileSaveAs() 
{
	// TODO: Add your command handler code here
//	frame->SaveGraph();
	
}

void CPercentileView::DrawToDCRegion(CDC *pDC, CRect rect)
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
	//long recsUsed = 0;
	//bool isSIG = false;
	char temp[64];
	//figure axis
	int x0, x1, y0, y1, y, x;
	x0 = rect.left + rect.Width() / 10;
	x1 = rect.right - (x0 - rect.left)/ 10;
	y1 = rect.top + rect.Height() / 10;
	y0 = rect.bottom - 2 * (y1 - rect.top);
	//load pens and fonts
    CPen penBlack2, penBlack1, penRed2, penRed3, penGreen2;  // Construct it, then initialize
    if(!penBlack2.CreatePen( PS_SOLID, 2, RGB(0,0,0)) 
		|| !penBlack1.CreatePen( PS_SOLID, 1, RGB(0,0,0))     
		|| !penRed2.CreatePen( PS_SOLID, 1, RGB(0xCC,0,0)) //)    
		|| !penRed3.CreatePen( PS_SOLID, 2, RGB(0x99,0,0)) //)    
		|| !penGreen2.CreatePen( PS_SOLID, 2, RGB(0,0x99,0)) )    
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
			_T("ERROR: Unable to create percentile graph fonts.\n\n"));
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
//	pDC->Rectangle(rect);
    CPen* pOldPen = pDC->SelectObject( &penBlack1 );
	pDC->Rectangle(rect);
	//force black bounding rectangle....(grrrr)
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
	/*CFont *oldFont = pDC->SelectObject(&headerFont);
	//pDC->TextOut(x0 + (x1 - x0) / 2, y1 / 2, "Percentiles");
	//pDC->TextOut(x0 + (x1 - x0) / 2, y0 + (rect.Height() - y0) / 3 * 2, frame->varName);
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 2, "Percentiles");
	pDC->TextOut(x0 + (x1 - x0) / 2, y0 + (rect.bottom - y0) / 3 * 2, frame->varName);*/
	//output headers
	CFont *oldFont = pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);
	//HEADERS
//	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 + 1, m_pAnalysis->m_opts.stationName);
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 2 + 1, m_pAnalysis->m_opts.stationName);
	if(m_pAnalysis->periods[varID][0]->years > 1)
		sprintf(temp, "%d - %d", m_pAnalysis->periods[varID][0]->baseYear,
			m_pAnalysis->periods[varID][0]->baseYear + m_pAnalysis->periods[varID][0]->years - 1);
	else
		sprintf(temp, "%d", m_pAnalysis->periods[varID][0]->baseYear);
	//pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 * 2 + 4, temp);
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 6 * 5 + 4, temp);
	pDC->TextOut(x0 + (x1 - x0) / 2, y0 + (rect.bottom - y0) / 3 * 2, varName);
	pDC->SelectObject(&vheaderFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);

	pDC->TextOut(rect.left + (x0 - rect.left) / 3, (y1 + y0) / 2, "Percentile");
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
	//double ideal = vals[99] / 10.0, xmax;
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	//for(int i = 1; i 
	int z = 0;
	while((z * ideal) > xmin)
			z--;
		//x = 0;
	y = z;// * ideal;//ymin;
	x = x0;
	xmin = z * ideal;
	//y = 0;
	while( x <= x1 )
	{
		if(ideal < 1.0)
			sprintf(temp,"%.1f ", y * ideal);
		else if(ideal < 10)
			sprintf(temp,"%.0f ", y * ideal);
		else
			sprintf(temp,"%5.0f ", y * ideal);
		pDC->MoveTo(x, y0);
		pDC->LineTo(x, y0 + (rect.bottom - y0) / 10);
		pDC->TextOut(x, y0 + (rect.bottom - y0) / 10, temp);
			//y -= (int)((ideal / ymax) * (double)(y0 - y1));
		y++;
		x = x0 + (int)(( (y - z) * ideal / (xmax - xmin)) * (double)(x1 - x0));
//			y = y0 - (int)( (x-z) * ideal / (ymax - ymin) * (double)(y0 - y1));
	}
	//draw a red horizontal line at critical percentiles
	pDC->SelectObject( &penRed2 );
	//first one
	//at y value
	y = (int)(y0 - (y0 - y1) * criticalPercentile + 1);
//	x = x0 + (int)(stats.Percentile(frame->criticalPercentile) / (xmax - xmin) * (double)(x1 - x0));
	x = x0 + (int)((stats.Percentile(criticalPercentile) - xmin) / (xmax - xmin) * (double)(x1 - x0));
	pDC->MoveTo(x0, y);
	pDC->LineTo(x, y);
	//now from where it will intersect graph down to x axis
	y = (int)(y0 - (y0 - y1) * stats.PercentileFromValue(stats.Percentile(criticalPercentile)) + 1);
	pDC->MoveTo(x, y);
	pDC->LineTo(x, y0);
	//second one
	//at y value
	y = (int)(y0 - (y0 - y1) * criticalPercentile2 + 1);
	x = x0 + (int)((stats.Percentile(criticalPercentile2) - xmin) / (xmax - xmin) * (double)(x1 - x0));
	pDC->MoveTo(x0, y);
	pDC->LineTo(x, y);
	//now from where it will intersect graph down to x axis
	y = (int)(y0 - (y0 - y1) * stats.PercentileFromValue(stats.Percentile(criticalPercentile2)) + 1);
	pDC->MoveTo(x, y);
	pDC->LineTo(x, y0);

	//draw a line for percentiles data
	pDC->SelectObject( &penBlack1 );
	pDC->MoveTo(x0, y0);
	//int loc;
	/*double pct;
	for(y = y0; y > y1; y--)
	{
		pct = ((double)(y0 - y)) / ((double)(y0 - y1));
		x = x0 + (int)(stats.Percentile(pct) / xmax * (double)(x1 - x0));
		pDC->LineTo(x, y);
	}*/
	//x-axis major style
	pDC->SelectObject( &penGreen2 );
	pDC->MoveTo(x0, y0);
	double newXstep = 1.0;
	if (varID == 8)
		newXstep = 0.05;
	else if (varID == 39)
		newXstep = 0.1;
	for (double xVal = xmin; xVal <= xmax; xVal += newXstep)//(varID == 8) ? xVal += 0.05 : xVal++)
		//for (double xVal = xmin; xVal <= xmax; (varID == 8) ? xVal += 0.05 : xVal++)
	{
		double yVal = stats.PercentileFromValue(xVal);
		y = y0 - ((double)(y0 - y1)) * yVal;
		x = x0 + (int)(((xVal - xmin) / (xmax - xmin))* (double)(x1 - x0));
		pDC->LineTo(x, y);
	}
/*	for(double xVal = 0.0; xVal <= xmax; (varID == 8) ? xVal += 0.05 : xVal++)
	{
		double yVal = stats.PercentileFromValue(xVal);
		y = y0 - ((double)(y0 - y1)) * yVal;
		x = x0 + (int)((xVal / (xmax  - xmin))* (double)(x1 - x0));
		pDC->LineTo(x, y);
	}*/

	pDC->SelectObject( &penBlack1 );
	CSize xTextSize;
	pDC->SetTextAlign(TA_BOTTOM | TA_LEFT);
	if(optionType == 1 && m_pAnalysis->m_opts.model.GetLength() > 0)
	{	//output model code in lower right corner
		CString mStr = "Model: " + m_pAnalysis->m_opts.model;
		xTextSize = pDC->GetTextExtent(mStr);

		pDC->TextOut(x1 - xTextSize.cx , y0 + xTextSize.cy * 4 /*+ xTextSize.cy / 2*/, mStr);
	}
	//output # observations in lower right corner
	sprintf(temp, "%ld Wx Observations", stats.N());
	xTextSize = pDC->GetTextExtent(temp, strlen(temp));
	pDC->TextOut(x1 - xTextSize.cx , y0 + xTextSize.cy * 5/* + xTextSize.cy / 2*/, temp);
	//output version/DateTime stamp
	CString tStr;
	CString dStr = m_pAnalysis->m_opts.dateTime.Format("%m/%d/%Y-%H:%M");
	//tStr.Format("FF+%s %s", verStr, dStr);
	tStr.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	//CString tStr = m_pAnalysis->m_opts.dateTime.Format("FF+3.0 %m/%d/%Y-%H:%M");
	xTextSize = pDC->GetTextExtent(tStr);
	pDC->TextOut(x1 - xTextSize.cx , y0 + xTextSize.cy * 6 /*+ xTextSize.cy / 2*/, tStr);
	//add number filtered if filterValue  != -999.0
	if (filterValue != -999.0)
	{
		if (filterDirection == FILTER_LESSTHAN_EQUAL)
			tStr.Format("Filtered %d <= %.0f", nFiltered, filterValue);
		else
			tStr.Format("Filtered %d >= %.0f", nFiltered, filterValue);
		pDC->TextOut(3, y0 + xTextSize.cy * 6, tStr);
	}
	//restore GDI objects
    pDC->SelectObject( pOldPen );    
    pDC->SelectObject( oldFont );    
	pDC->SelectObject( pOldBrush );    
}


void CPercentileView::OnFileSave() 
{
	// TODO: Add your command handler code here
	OnFileSaveAs();
}

BOOL CPercentileView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	
	return DoPreparePrinting(pInfo);
	//return CView::OnPreparePrinting(pInfo);
}

void CPercentileView::PrintGraph()
{
	theApp.SetLandscape();
	CView::OnFilePrint();
}

void CPercentileView::SaveGraph(void)
{
	CGraphSaveDialog sgd(this);
	if(sgd.DoModal() == IDOK)
	{
		chdir(theApp.userDir);
		CFileDialog fd(false, theApp.GetGraphicsExt(), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			theApp.GetGraphicsFileStr(), this);
		if(fd.DoModal() == IDOK)
		{
			CDC *whdc = GetDC();
			CDC memhdc;
			memhdc.CreateCompatibleDC(NULL);
			HBITMAP picture = CreateCompatibleBitmap(whdc->m_hDC, sgd.width, sgd.height);
			HGDIOBJ Old = SelectObject(memhdc.m_hDC, picture);
			CRect bnd(0, 0, sgd.width, sgd.height);
			CRect rect(0, 0, sgd.width, sgd.height);
			DrawToDCRegion(&memhdc, rect);
			char fName[256];
			strcpy_s(fName, fd.GetPathName());
			SaveBitmapFile(memhdc.m_hDC, picture, fName); 
			SelectObject(memhdc.m_hDC, Old);
			ReleaseDC(whdc);
			memhdc.DeleteDC();
			DeleteObject(picture);
			theApp.SetUserDir(fd.GetPathName());
			theApp.SetGraphicsExt(fd.GetFileExt());
		}
		chdir(theApp.dbDir);
	}
}

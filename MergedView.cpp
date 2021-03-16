// MergedView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "MergedView.h"
#include "MergedFrame.h"
#include "wxSet.h"
#include "SplitterFrame.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "Line.h"
#include <math.h>
#include <direct.h>
#include "climateSet.h"
#include "SIGStationSet.h"
#include "FireSumSet.h"
#include "ClimAnalysis.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
//extern char *merge_abbrevs[];
/////////////////////////////////////////////////////////////////////////////
// CMergedView

const double NODATA = -9999.0;

IMPLEMENT_DYNCREATE(CMergedView, CView)

CMergedView::CMergedView()
{
	staTitle = "";
	yrsTitle = "";
	recsUsed = 0;
	for(int v = 0; v < MAXMERGES; v++)
		vals[v] = NULL;
}

CMergedView::~CMergedView()
{
	for(int v = 0; v < MAXMERGES; v++)
	{
		if(vals[v])
			delete[] vals[v];
	}
}


BEGIN_MESSAGE_MAP(CMergedView, CView)
	//{{AFX_MSG_MAP(CMergedView)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergedView drawing

void CMergedView::OnDraw(CDC* pDC)
{
	//CDocument* pDoc = GetDocument();
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
}

/////////////////////////////////////////////////////////////////////////////
// CMergedView diagnostics

#ifdef _DEBUG
void CMergedView::AssertValid() const
{
	CView::AssertValid();
}

void CMergedView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMergedView message handlers

void CMergedView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	BuildData();
}


void CMergedView::DrawToDCRegion(CDC * destDC, CRect rect)
{
	int lWid[MAXMERGES];
	CPeriodDoc* pDoc = (CPeriodDoc*)GetDocument();
	if(destDC->IsPrinting())
	{
		int lx, ly;
		ly = destDC->GetDeviceCaps(VERTRES);
		lx = destDC->GetDeviceCaps(HORZRES);
		rect.left = 0;
		rect.top = 0;
		rect.right = lx;
		rect.bottom = ly;
		CDC *screenDC = GetDC();
		for(int l = 0; l < MAXMERGES; l++)
			lWid[l] = ScreenPixelsToPrinterPixels(screenDC, destDC, widths[l]);
		ReleaseDC(screenDC);
	}else
	{
		for(int l = 0; l < MAXMERGES; l++)
			lWid[l] = widths[l];
	}
	int firstVar = -1;
	for(int j = 0; j < MAXMERGES; j++)
	{
		if(vars[j] >= 0)
		{
			firstVar = vars[j];
			break;
		}
	}
	if(firstVar < 0)
		return;
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
	
	
	//figure axis
	int x0, x1, y0, y1;
	x0 = rect.left + rect.Width() / 10;
	x1 = rect.right - x0;//2 * (x0 - rect.left) / 3;
	y1 = rect.top + rect.Height() / 10;
	y0 = rect.bottom - 2 * (y1 - rect.top);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);
	//create drawing objects
    CPen penBlack2, penBlack1;//, penGray, penRed, penHGray, penVGray;  // Construct it, then initialize
    if(!penBlack2.CreatePen( PS_SOLID, 2, RGB(0,0,0)) 
		|| !penBlack1.CreatePen( PS_SOLID, 1, RGB(0,0,0)))     
	//	|| !penRed.CreatePen( PS_SOLID, 1, RGB(0xCC,0,0))     
	//	|| !penGray.CreatePen( PS_SOLID, 1, RGB(200,200,200))    
	//	|| !penHGray.CreatePen( StringToLineStyle(frame->m_H_Line_Style), 1, RGB(200,200,200))    
	//	|| !penVGray.CreatePen( StringToLineStyle(frame->m_V_Line_Style), 1, RGB(200,200,200)))    
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Resources too low for creating drawing pens.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CFont headerFont, axisFont;//, vheaderFont;
	if(!headerFont.CreateFont( (y1 - rect.top) / 2, (x0 - rect.left)/ 6, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")
		//|| !vheaderFont.CreateFont((x0 - rect.left) / 4,  (y1 - rect.top) / 4 , 900, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
		//OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")
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
	CFont *pOldFont = pDC->SelectObject(&headerFont);
	pDC->Rectangle(rect);
	//force black bounding rectangle....(grrrr)
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.top);
	pDC->LineTo(rect.left, rect.top);
	//draw axis
	pDC->SelectObject( &penBlack2 );
	pDC->MoveTo(x0, y1);
	pDC->LineTo(x0, y0);
	pDC->LineTo(x1, y0);
	pDC->LineTo(x1, y1);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);
	//HEADERS
	//pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 + 1, staTitle);
	//pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 * 2 + 4, yrsTitle);
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 2 + 1, staTitle);
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 6 * 5 + 4, yrsTitle);
	CString temp;
	temp.Format("%d Day Periods", pDoc->m_pAnalysis->m_opts.periodLength);
	pDC->TextOut(x0 + (x1 - x0) / 2, y0 + (rect.bottom - y0) / 3 * 2, temp);
	CSize xTextSize;
	pDC->SelectObject(&axisFont);
	pDC->SetTextAlign(TA_BOTTOM | TA_LEFT);
	if(optionType == 1 && pDoc->m_pAnalysis->m_opts.model.GetLength() > 0)
	{	//output model code in lower right corner
		CString mStr = "Model: " + pDoc->m_pAnalysis->m_opts.model;
		xTextSize = pDC->GetTextExtent(mStr);		
		pDC->TextOut(rect.right - (x0 / 5) - xTextSize.cx , y0 + xTextSize.cy * 4 /*+ xTextSize.cy / 2*/, mStr);
	}
	//output # observations in lower right corner
	temp.Format("%ld Wx Observations", recsUsed);
	xTextSize = pDC->GetTextExtent(temp);
	pDC->TextOut(rect.right - (x0 / 5) - xTextSize.cx , y0 + xTextSize.cy * 5 /*+ xTextSize.cy / 2*/, temp);
	//output version/DateTime stamp
	CString dStr = pDoc->m_pAnalysis->m_opts.dateTime.Format("%m/%d/%Y-%H:%M");
	//temp.Format("FF+%s %s", verStr, dStr);
	temp.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	//temp = pDoc->m_pAnalysis->m_opts.dateTime.Format("FF+3.0 %m/%d/%Y-%H:%M");
	xTextSize = pDC->GetTextExtent(temp);
	pDC->TextOut(rect.right - (x0 / 5) - xTextSize.cx , y0 + xTextSize.cy * 6 /*+ xTextSize.cy / 2*/, temp);
	
	//x axis labels
	int y, x;
	int xlabels = 0;
	double step;
	if(pDoc->m_pAnalysis->m_opts.numPeriods > 1)
		step = (double)(x1 - x0) / (double)(pDoc->m_pAnalysis->m_opts.numPeriods - 1);
	else
		step = (x1 - x0) / 2;
	//bool drawing = false;
	pDC->SetTextAlign(TA_TOP | TA_CENTER);
	for(int k = 0; k < pDoc->m_pAnalysis->m_opts.numPeriods; k++)
	{
		x = x0 + (int)(k * step);
		pDC->SelectObject(&penBlack1);
		pDC->MoveTo(x, y0);
		if(pDoc->m_pAnalysis->periods[firstVar][k]->start.GetDay() == 1
			|| pDoc->m_pAnalysis->m_opts.numPeriods < 12 || k == 0)
		{
			pDC->LineTo(x, y0 + (rect.bottom - y0) / 10);
			temp.Format("%d/%d", pDoc->m_pAnalysis->periods[firstVar][k]->start.GetMonth(), 
				pDoc->m_pAnalysis->periods[firstVar][k]->start.GetDay());
			if(xlabels % 2 == 0)
				pDC->TextOut(x, y0 + (rect.bottom - y0) / 10, temp); 
			else
				pDC->TextOut(x, y0 + (rect.bottom - y0) / 10 + xTextSize.cy, temp); 				
			xlabels++;
		}
		else
			pDC->LineTo(x, y0 + (rect.bottom - y0) / 15);
		//pDC->MoveTo(x, y);
	}
				
	CPen mergePen;

	pDC->SetTextAlign(TA_TOP | TA_LEFT);
	//draw the lines
	int m;
	for(m = 0; m < MAXMERGES; m++)
	{
		if(vars[m] != -1)
		{
			int lt, lb;
			pDC->SelectObject(pOldPen);
			mergePen.DeleteObject();
			//mergePen.CreatePen(PS_SOLID, 1, pDC->GetNearestColor(colors[m]));
			mergePen.CreatePen(PS_SOLID, 1, colors[m]);
			pDC->SelectObject(&mergePen);
			unsigned auxType[8];
			bool Round = true;
			int c1 = CDashLine::GetPattern(auxType, Round, lWid[m], 
				styles[m]);
			CDashLine auxLine(*pDC, auxType, c1);
			pDC->BeginPath();
			int cnt = 0;
			//bool drawing = false;
			for(int k = 0; k < pDoc->m_pAnalysis->m_opts.numPeriods; k++)
			{
				if(pDoc->m_pAnalysis->periods[vars[m]][k]->N() > 0 && vals[m][k] != NODATA)

				{
					x = x0 + (int)(k * step);
					y = y0 - (int)((vals[m][k] - yMin[m]) / (yMax[m] - yMin[m])* (double)(y0 - y1));
					if(!cnt)
						auxLine.MoveTo(x, y);
					else
					{
						auxLine.LineTo(x, y);
					}
					cnt++;
				}
				else
					cnt = 0;
			}
			pDC->EndPath();
			//DrawPathOutline(pDC->GetNearestColor(colors[m]), *pDC, Round, lWid[m]);
			DrawPathOutline(colors[m], *pDC, Round, lWid[m]);
			//draw legend entry
				//unsigned mrgType[8];
				//bool mRound = true;
			xTextSize = pDC->GetTextExtent("MMMM");
				//int brushWidth = mergeWidths[m];
				//int c1 = CDashLine::GetPattern(mrgType, mRound, mergeWidths[m], 
				//	StringToDashLineStyle(frame->mergedStyles[m]));
				//CDashLine mrgLine(*pDC, mrgType, c1);
			if(m %2 == 0)
				lt = y0 + 3 * xTextSize.cy;// + xTextSize.cy / 2;
			else
				lt = y0 + 4 * xTextSize.cy + xTextSize.cy / 2;
			if(m < 2)
				lb = rect.left + xTextSize.cx + xTextSize.cx / 2;
			else
				lb = rect.left + 3 * xTextSize.cx;// + xTextSize.cx / 2;
			pDC->TextOut(lb, lt, varNames[m]);
			pDC->BeginPath();
			//if(m == 1)
				//auxLine.MoveTo(hBase - xTextSize.cx + 1, y1 - (y1 - rect.top) / 4);
			//else
				//auxLine.MoveTo(hBase + xTextSize.cx - 1, y1 - (y1 - rect.top) / 4);
			auxLine.MoveTo(lb - 2, lt + xTextSize.cy / 2);
			auxLine.LineTo(lb - xTextSize.cx / 2, lt + xTextSize.cy / 2);
			pDC->EndPath();
			DrawPathOutline(colors[m], *pDC, Round, lWid[m]);
		}
	}
	//merged variable axis labels
	for(m = 0; m < MAXMERGES; m++)
	{
		if(vars[m] != -1)
		{
			pDC->SelectObject(pOldPen);
			mergePen.DeleteObject();
		//	mergePen.CreatePen(PS_SOLID, 1, pDC->GetNearestColor(colors[m]));
			mergePen.CreatePen(PS_SOLID, 1, colors[m]);
			pDC->SelectObject(&mergePen);
			int hBase = x1;
			switch(m)
			{
			case 0:
				hBase = x0 - (rect.right - x1) / 8;
				pDC->SetTextAlign(TA_BASELINE | TA_RIGHT);
				break;
			case 1:
				hBase = x1 + (rect.right - x1) / 8;
				pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
				break;
			case 3:
				hBase = x1 - (rect.right - x1) / 8;
				pDC->SetTextAlign(TA_BASELINE | TA_RIGHT);
				break;
			case 2:
				hBase = x0 + (rect.right - x1) / 8;
				pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
				break;
			}
			//CString mStr = abbrevs[vars[m]];
			//mStr.TrimLeft();
			//mStr.TrimRight();
			pDC->TextOut(hBase, y1 - (y1 - rect.top) / 3, varNames[m]);
			
			double mIdeal = IdealFromYMax(yMax[m] - yMin[m]);
			int z = 0;
			while((z * mIdeal) > yMin[m])
				z--;
			x = z;
			y = y0;// - (int)(( x * mIdeal / yMax[m]) * (double)(y0 - y1));
			while( y >= y1 )
			{
				if(mIdeal < 1.0)
					temp.Format("%.2f ", x * mIdeal);
				else if(mIdeal < 10)
					temp.Format("%.0f ", x * mIdeal);
				else
					temp.Format("%5.0f ", x * mIdeal);
				pDC->MoveTo(hBase, y);
				pDC->LineTo((m == 0 || m == 2) ? x0 : x1, y);
				pDC->TextOut(hBase, y, temp);
				x++;
				//y = y0 - (int)(( x * mIdeal / yMax[m]) * (double)(y0 - y1));
				y = y0 - (int)( (x-z) * mIdeal / (yMax[m] - yMin[m]) * (double)(y0 - y1));
			}
		}
	}


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

double CMergedView::IdealFromYMax(double ymax)
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

/*void CMergedView::DrawPathOutline(const COLORREF& c, CDC& dc, bool Round, int PenSize)
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


void CMergedView::BuildData()
{
	// first clear everything....
	int v;
	for(v = 0; v < MAXMERGES; v++)
	{
		if(vals[v])
			delete[] vals[v];
		vals[v] = NULL;
		vars[v] = -1;
	}
	recsUsed = 0;
	//Now build the data
	CPeriodDoc* pDoc = (CPeriodDoc*)GetDocument();
	optionType = 0;
	int nP = pDoc->m_pAnalysis->m_opts.numPeriods;
	CMergedFrame *frame = (CMergedFrame *)GetParent();//->GetParent();
	int firstVar = -1;
	for(int j = 0; j < MAXMERGES; j++)
	{
		if(frame->vars[j] >= 0)
		{
			firstVar = frame->vars[j];
			break;
		}
	}
	if(firstVar < 0)
		return;
	year = frame->year;
	yMax[0] = yMax[1] = yMax[2] = yMax[3] = 0.1;//1.0;
	yMin[0] = yMin[1] = yMin[2] = yMin[3] = 0.0;//1.0;
	for(int i = 0; i < MAXMERGES; i++)
	{
		colors[i] = theApp.mergeColors[i];
		styles[i] = theApp.mergeStyles[i];
		widths[i] = theApp.mergeWidths[i];
		yMax[i] = frame->yMax[i];
		yMin[i] = frame->yMin[i];
	}
	staTitle = pDoc->m_pAnalysis->m_opts.stationName;
	if(year <= 0)
		yrsTitle.Format("%d - %d", pDoc->m_pAnalysis->periods[firstVar][0]->baseYear,
			pDoc->m_pAnalysis->periods[firstVar][0]->baseYear + pDoc->m_pAnalysis->periods[firstVar][0]->years - 1);
	else
		yrsTitle.Format("%d", year);
	long minPrecipObs = 0;//GetMinPrecipObs(pDoc->m_pAnalysis->m_opts., COleDateTime end);
	for(v = 0; v < MAXMERGES; v++)
	{
		if(frame->vars[v] >= 0)
		{
			//create memory and init to zero
			vars[v] = frame->vars[v];
			if(OptionFromVarID(vars[v]) == 1)
				optionType = 1;
			varNames[v] = pDoc->m_pAnalysis->varNames[frame->vars[v]];
			varNames[v].TrimLeft();
			varNames[v].TrimRight();
			vals[v] = new double[nP];
			for(int p = 0; p < nP; p++)
				vals[v][p] = NODATA;
			//get the values
			//double mult;
			if(year <= 0)//averages
			{
				if(vars[v] == 8)//precip
				{
					Period ** pcpSums = new Period *[nP];
					for(int p = 0; p < nP; p++)
						pcpSums[p] = new Period(pDoc->m_pAnalysis->periods[vars[v]][p]->start, pDoc->m_pAnalysis->periods[vars[v]][p]->end);
					for(int p = 0; p < nP; p++)
					{
						minPrecipObs = GetMinPrecipObs(pDoc->m_pAnalysis->periods[vars[v]][p]->start, pDoc->m_pAnalysis->periods[vars[v]][p]->end);
						for(int y = 0; y < pDoc->m_pAnalysis->periods[vars[v]][p]->years; y++)
						{
							if(pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[y].N() >= minPrecipObs)
							{
								COleDateTime pDate;
								pDate.SetDate(pDoc->m_pAnalysis->periods[vars[v]][p]->start.GetYear() + y, pDoc->m_pAnalysis->periods[vars[v]][p]->start.GetMonth(),
									pDoc->m_pAnalysis->periods[vars[v]][p]->start.GetDay());
								pcpSums[p]->Accumulate(pDate, pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[y].Mean() 
									* pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[y].N());
								if(v == 0)
									recsUsed += pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[y].N();
							}
						}
					}
					for(int p = 0; p < nP; p++)
					{
						pcpSums[p]->Complete();
						vals[v][p] = pcpSums[p]->mean;
						//yMax[v] = max(yMax[v], vals[v][p]);
						delete pcpSums[p];
					}
					delete[] pcpSums;
				}
				else
				{
					for(int p = 0; p < nP; p++)
					{
						if (pDoc->m_pAnalysis->periods[vars[v]][p]->N() > 0)
						{
							vals[v][p] = pDoc->m_pAnalysis->periods[vars[v]][p]->mean;
							//yMax[v] = max(yMax[v], vals[v][p]);
							if (v == 0)
								recsUsed += pDoc->m_pAnalysis->periods[vars[v]][p]->N();
						}
					}
				}
			}
			else // individual year
			{
				int yr = year - pDoc->m_pAnalysis->periods[vars[v]][0]->baseYear;
				if(yr >= 0 && yr < pDoc->m_pAnalysis->periods[vars[v]][0]->years)//normal year
				{
					for(int p = 0; p < nP; p++)
					{
						if(vars[v] == 8)//precip
						{
							minPrecipObs = GetMinPrecipObs(pDoc->m_pAnalysis->periods[vars[v]][p]->start, pDoc->m_pAnalysis->periods[vars[v]][p]->end);
							if(pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[yr].N() >= minPrecipObs)
							{
								vals[v][p] = pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[yr].Mean() * pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[yr].N();
								if(v == 0)
									recsUsed += pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[yr].N();
							}
						}
						else
						{
							if (pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[yr].N() > 0)
							{
								vals[v][p] = pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[yr].Mean();
								if (v == 0)
									recsUsed += pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[yr].N();
							}
						}
					//	yMax[v] = max(yMax[v], vals[v][p]);
						//if(v == 0)
							//recsUsed += pDoc->m_pAnalysis->periods[vars[v]][p]->yStats[yr].N();
					}
				}
				else//auxilliary year
				{
					yr = year - pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][0]->baseYear;
					for(int p = 0; p < nP; p++)
					{
						if(vars[v] == 8)//precip
						{
							minPrecipObs = GetMinPrecipObs(pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][p]->start, pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][p]->end);
							if(pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][p]->yStats[yr].N() >= minPrecipObs)
							{
								vals[v][p] = pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][p]->yStats[yr].Mean() * pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][p]->yStats[yr].N();
								if(v == 0)
									recsUsed += pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][p]->yStats[yr].N();
							}
						}
						else
						{
							if (pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][p]->yStats[yr].N() > 0)
							{
								vals[v][p] = pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][p]->yStats[yr].Mean();
								if (v == 0)
									recsUsed += pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][p]->yStats[yr].N();
							}
						}
						//vals[v][p] = pDoc->m_pAnalysis->m_opts.tPeriods[vars[v]][p]->yStats[yr].Mean() * mult;
					//	yMax[v] = max(yMax[v], vals[v][p]);
					}
				}
			}
		}
		else
		{
			vars[v] = -1;
			varNames[v] = "";
		}
	}
}

void CMergedView::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();	
}

void CMergedView::OnFilePrint() 
{
	CView::OnFilePrint();
	
}

BOOL CMergedView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	
	return DoPreparePrinting(pInfo);
}

void CMergedView::OnFileSaveAs() 
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

// GraphView.cpp : implementation file
//

#include "stdafx.h"
#include <afxrich.h>
#include "richdoc.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "GraphView.h"
#include "wxSet.h"
#include "SplitterFrame.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include <math.h>
#include "OverlayDialog.h"
#include "Line.h"
#include "ReportOptionsSet.h"
#include "fireplusset.h"
#include "climateSet.h"
#include "SIGStationSet.h"
#include "FireSumSet.h"
#include "ClimAnalysis.h"
#include ".\graphview.h"
#include "GraphSaveDialog.h"
#include <direct.h>
#include "BatchOutputFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// added for batch2
extern int curr_runID;
extern CFireplusApp theApp;

//extern char *abbrevs[];
/////////////////////////////////////////////////////////////////////////////
// CGraphView

IMPLEMENT_DYNCREATE(CGraphView, CView)

CGraphView::CGraphView()
{
	ymax = ymin = 0;
	cps = NULL;
	cp2s = NULL;
	mins = maxs = NULL;
	pcpSums = NULL;
	pcpAuxSums = NULL;
	m_pAnalysis = NULL;
}

CGraphView::~CGraphView()
{
	if(cps)
		delete[] cps;
	if(cp2s)
		delete[] cp2s;
	if(mins)
		delete[] mins;
	if(maxs)
		delete[] maxs;
	int nP = m_pAnalysis->m_opts.numPeriods;
	if(pcpSums)
	{
		for(int p = nP - 1; p >= 0; p--)
			delete pcpSums[p];
		delete[] pcpSums;
	}
	nP = m_pAnalysis->m_opts.numtPeriods;
	if(pcpAuxSums)
	{
		for(int p = nP - 1; p >= 0; p--)
			delete pcpAuxSums[p];
		delete[] pcpAuxSums;
	}
	//if(statsfName)
	//	free(statsfName);
}

BEGIN_MESSAGE_MAP(CGraphView, CView)
	//{{AFX_MSG_MAP(CGraphView)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_OPTIONS_GRAPHTYPE_LINEATAVERAGE, OnOptionsGraphtypeLineataverage)
	ON_COMMAND(ID_OPTIONS_GRAPHTYPE_STATISTICALBARS, OnOptionsGraphtypeStatisticalbars)
	ON_WM_LBUTTONUP()
	//ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_OPTIONS_VIEWHEADERINFO, OnViewHeader)
	ON_COMMAND(ID_FILE_EXPORTGRAPHDATA,OnExportData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphView drawing

void CGraphView::OnDraw(CDC* pDC)
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
}

double CGraphView::IdealFromYMax(double yMax)
{
	if(yMax < 0.15)
		return 0.01;
	else if(yMax < 0.6)
		return 0.05;
	else if(yMax < 1.25)
		return 0.1;
	else if(yMax < 2.5)
		return 0.25;
	else if(yMax < 5.0)
		return 0.5;
	else if(yMax < 10.0)
		return 1.0;
	else if(yMax < 25.0)
		return 2.0;
	else if(yMax < 50.0)
		return 5.0;
	else if(yMax < 100.0)
		return 10.0;
	else if(yMax < 250.0)
		return 20.0;
	else if(yMax < 500.0)
		return 50.0;
	else if(yMax < 1000.0)
		return 100.0;
	else if(yMax < 2500.0)
		return 200.0;
	else if(yMax < 5000.0)
		return 500.0;
	else if(yMax < 10000.0)
		return 1000.0;
	//way big
	int power = (int)log10( yMax / 10.0 );
	if(power > 0)
	{
		double step = floor( (yMax / 10.0) / pow(10.0, power) );
		return floor( step * pow(10.0, power ));
	}

	return yMax / 10;
}

/////////////////////////////////////////////////////////////////////////////
// CGraphView diagnostics

#ifdef _DEBUG
void CGraphView::AssertValid() const
{
	CView::AssertValid();
}

void CGraphView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CGraphView message handlers

void CGraphView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CWnd *pWnd = GetParent()->GetParent();
	if(pWnd)
	{
		if(pWnd->IsKindOf(RUNTIME_CLASS(CSplitterFrame)))
			((CSplitterFrame *)pWnd)->MaximizePane(this);
	}
//	frame->MaximizePane(this);
}

void CGraphView::OnOptionsGraphtypeLineataverage()
{
	if(m_graphOpts.isBargraph)
	{
		m_graphOpts.isBargraph = !m_graphOpts.isBargraph;
		//force redisplay
		RedrawWindow();
	}
}

void CGraphView::OnOptionsGraphtypeStatisticalbars()
{
	if(!m_graphOpts.isBargraph)
	{
		m_graphOpts.isBargraph = !m_graphOpts.isBargraph;
		//force redisplay
		RedrawWindow();
	}
}

void CGraphView::OnLButtonUp(UINT nFlags, CPoint point)
{
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
	int x0, x1, y0, y1;
	CView::OnLButtonUp(nFlags, point);
	CRect rect;
	GetClientRect(rect);
	rect.NormalizeRect();
	CString xtext = "", ytext = "";
	if(rect.Width() > 0 || rect.Height() > 0)
	{//figure values
		x0 = rect.Width() / 8;
		x1 = rect.right - x0 / 10;
		y1 = rect.Height() / 10;
		y0 = rect.bottom - 2 * y1;
		if(point.x < x0 || point.x > x1 )
			xtext = "NA";
		else
		{
			if(m_pAnalysis->m_opts.numPeriods <= 0)
				xtext = "NA";
			else
			{
				double p;
				if(m_graphOpts.isBargraph)
					p = ((double)(point.x - x0)) / ((double)(barwidth + gap));// - 0.5;
				else
				{
					if(m_pAnalysis->m_opts.numPeriods > 1)
					{
						double step = (double) (x1 - x0) / (double)(m_pAnalysis->m_opts.numPeriods - 1);
						p = ((double)(point.x - x0)) / step;
					}
					else
						p = 0;
				}
				if(p < 0)
					p = 0;
				if(p >= m_pAnalysis->m_opts.numPeriods)
					p = m_pAnalysis->m_opts.numPeriods - 1;
				xtext.Format("%d/%d - %d/%d",
					m_pAnalysis->periods[varID][(int)p]->start.GetMonth(),
					m_pAnalysis->periods[varID][(int)p]->start.GetDay(),
					m_pAnalysis->periods[varID][(int)p]->end.GetMonth(),
					m_pAnalysis->periods[varID][(int)p]->end.GetDay());
			}
		}
		if(point.y > y0 || point.y < y1)
			ytext = "NA";
		else
		{
			double y =
				-((double)(ymax - ymin) * (point.y - y0)) / ((double)(y0 - y1)) + ymin;
			if(varID != 8 && varID != 39)
				ytext.Format("%.0f", y);
			else
				ytext.Format("%.2f", y);
		}
	}
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

void CGraphView::PreDrawCalcs()
{
	int nP = m_pAnalysis->m_opts.numPeriods;
	cps = new double[nP];
	cp2s = new double[nP];
	mins = new double[nP];
	maxs = new double[nP];
	fpSet = m_graphOpts.fpSet;
	varID = m_graphOpts.varID;
	optionType = m_graphOpts.optionType;
	// special case for precip amount
	if(varID == 8)
	{
		pcpSums = new Period *[nP];
		int p;
		for(p = 0; p < nP; p++)
		{
			pcpSums[p] = new Period(m_pAnalysis->periods[varID][p]->start, m_pAnalysis->periods[varID][p]->end);
		}
		if(m_pAnalysis->m_opts.numtPeriods > 0 && m_pAnalysis->m_opts.tPeriods)
		{
			pcpAuxSums = new Period *[m_pAnalysis->m_opts.numtPeriods];
			for(int a = 0; a < m_pAnalysis->m_opts.numtPeriods; a++)
				pcpAuxSums[a] = new Period(m_pAnalysis->m_opts.tPeriods[varID][a]->start, m_pAnalysis->m_opts.tPeriods[varID][a]->end);
		}
		//accumulate sums
		long minPrecipObs = 0;//GetMinPrecipObs(m_pAnalysis->m_opts., COleDateTime end);

		for(p = 0; p < nP; p++)
		{
			minPrecipObs = GetMinPrecipObs(m_pAnalysis->periods[varID][p]->start, m_pAnalysis->periods[varID][p]->end);
			for(int y = 0; y < m_pAnalysis->periods[varID][p]->years; y++)
			{
				if(m_pAnalysis->periods[varID][p]->yStats[y].N() >= minPrecipObs)
				{
					COleDateTime pDate;
					pDate.SetDate(m_pAnalysis->periods[varID][p]->start.GetYear() + y, m_pAnalysis->periods[varID][p]->start.GetMonth(),
						m_pAnalysis->periods[varID][p]->start.GetDay());
					pcpSums[p]->Accumulate(pDate, m_pAnalysis->periods[varID][p]->yStats[y].Mean()
						* m_pAnalysis->periods[varID][p]->yStats[y].N());
				}
			}
			pcpSums[p]->Complete();
		}
		//accumulate auxSums
		for(p = 0; p < m_pAnalysis->m_opts.numtPeriods; p++)
		{
			minPrecipObs = GetMinPrecipObs(m_pAnalysis->periods[varID][p]->start, m_pAnalysis->periods[varID][p]->end);
			for(int y = 0; y < m_pAnalysis->m_opts.tPeriods[varID][p]->years; y++)
			{
				if(m_pAnalysis->m_opts.tPeriods[varID][p]->yStats[y].N() >= minPrecipObs)
				{
					COleDateTime pDate;
					pDate.SetDate(m_pAnalysis->m_opts.tPeriods[varID][p]->start.GetYear() + y, m_pAnalysis->m_opts.tPeriods[varID][p]->start.GetMonth(),
						m_pAnalysis->m_opts.tPeriods[varID][p]->start.GetDay());
					pcpAuxSums[p]->Accumulate(pDate, m_pAnalysis->m_opts.tPeriods[varID][p]->yStats[y].Mean()
						* m_pAnalysis->m_opts.tPeriods[varID][p]->yStats[y].N());
				}
			}
			pcpAuxSums[p]->Complete();
		}
	}
	//
	Stats cpStats, maxStats, minStats, overAllStats;
	for(int i = 0; i < nP; i++)
	{
		if((varID != 8 && m_pAnalysis->periods[varID][i]->N() > 0) || (varID == 8 && pcpSums[i]->N() > 0))
		{
			if(varID != 8)
			{
				for(int y = 0; y < m_pAnalysis->periods[varID][i]->years; y++)
				{
					if(m_pAnalysis->periods[varID][i]->yStats[y].N() > 0)
					{
						for(int v = 0; v < m_pAnalysis->periods[varID][i]->yStats[y].N(); v++)
						{
							cpStats.Accumulate(m_pAnalysis->periods[varID][i]->yStats[y].vals[v]);
							overAllStats.Accumulate(m_pAnalysis->periods[varID][i]->yStats[y].vals[v]);
						}
						//maxStats.Accumulate(m_pAnalysis->periods[varID][i]->yStats[y].Max());
						//minStats.Accumulate(m_pAnalysis->periods[varID][i]->yStats[y].Min());
						maxStats.Accumulate(m_pAnalysis->periods[varID][i]->yStats[y].Mean());
						minStats.Accumulate(m_pAnalysis->periods[varID][i]->yStats[y].Mean());
					}
				}
				ymax = max(ymax, m_pAnalysis->periods[varID][i]->max);
				ymin = min(ymin, m_pAnalysis->periods[varID][i]->min);
			}
			else
			{
				for(int y = 0; y < pcpSums[i]->years; y++)
				{
					if(pcpSums[i]->yStats[y].N() > 0)
					{
						for(int v = 0; v < pcpSums[i]->yStats[y].N(); v++)
						{
							cpStats.Accumulate(pcpSums[i]->yStats[y].vals[v]);
							overAllStats.Accumulate(pcpSums[i]->yStats[y].vals[v]);
						}
					//	maxStats.Accumulate(pcpSums[i]->yStats[y].Max());
					//	minStats.Accumulate(pcpSums[i]->yStats[y].Min());
						maxStats.Accumulate(pcpSums[i]->yStats[y].Mean());
						minStats.Accumulate(pcpSums[i]->yStats[y].Mean());
					}
				}
				ymax = max(ymax, pcpSums[i]->max);
				ymin = min(ymin, pcpSums[i]->min);
			}
		}
		cpStats.Complete();
		maxStats.Complete();
		minStats.Complete();
		if(cpStats.N() > 0)
		{
			cps[i] = cpStats.Percentile(m_graphOpts.criticalPercentile);
			cp2s[i] = cpStats.Percentile(m_graphOpts.criticalPercentile2);
		}
		else
		{
			cps[i] = 0.0;
			cp2s[i] = 0.0;
		}
		if(maxStats.N() > 0)
		{
			maxs[i] = maxStats.Max();
			mins[i] = minStats.Min();
		}
		else
		{
			maxs[i] = 0;
			mins[i] = 0;
		}
		cpStats.Reset();
		maxStats.Reset();
		minStats.Reset();
		if(m_pAnalysis->m_opts.tPeriods && varID != 8)
		{
			if(m_pAnalysis->m_opts.tPeriods[varID][i]->N() > 0)
			{
				ymax = max(ymax, m_pAnalysis->m_opts.tPeriods[varID][i]->max );
				ymin = min(ymin, m_pAnalysis->m_opts.tPeriods[varID][i]->min );
			}
		}
		if(pcpAuxSums)
		{
			if(pcpAuxSums[i]->N() > 0)
			{
				ymax = max(ymax, pcpAuxSums[i]->max);
				ymin = min(ymin, pcpAuxSums[i]->min);
			}
		}
	}
	overAllStats.Complete();
	overallCP1 = overAllStats.Percentile(m_graphOpts.criticalPercentile);
	overallCP2 = overAllStats.Percentile(m_graphOpts.criticalPercentile2);
		//safety
	if(ymax <= 0.1)
		ymax = 0.1;
	ideal = IdealFromYMax(ymax - ymin);
}
void CGraphView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	PreDrawCalcs();
}

void CGraphView::PrintToDC(CDC * pDC)
{
	OnDraw(pDC);
}

void CGraphView::OnFileSaveAs()
{
}

void CGraphView::DrawToDCRegion(CDC * destDC, CRect rect)
{
	int lineWidth = m_graphOpts.m_Line_Width, cp1Width = m_graphOpts.m_cp1Width, cp2Width = m_graphOpts.m_cp2Width,
		minsWidth = m_graphOpts.m_minsWidth, maxsWidth = m_graphOpts.m_maxsWidth;//, mergeWidths[3];
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
		lineWidth = ScreenPixelsToPrinterPixels(screenDC, destDC, lineWidth);
		cp1Width = ScreenPixelsToPrinterPixels(screenDC, destDC, cp1Width);
		cp2Width = ScreenPixelsToPrinterPixels(screenDC, destDC, cp2Width);
		maxsWidth = ScreenPixelsToPrinterPixels(screenDC, destDC, maxsWidth);
		minsWidth = ScreenPixelsToPrinterPixels(screenDC, destDC, minsWidth);
//		mergeWidths[0] = ScreenPixelsToPrinterPixels(screenDC, destDC, mergeWidths[0]);
//		mergeWidths[1] = ScreenPixelsToPrinterPixels(screenDC, destDC, mergeWidths[1]);
//		mergeWidths[2] = ScreenPixelsToPrinterPixels(screenDC, destDC, mergeWidths[2]);
		ReleaseDC(screenDC);
	}
	long recsUsed = 0;
	int radius, triHgt, triWdth;
	double step;
	//bool isSIG = false;
	varID = m_graphOpts.varID;
	CString temp;
	CString string;
	int nP = m_pAnalysis->m_opts.numPeriods, x0, x1, y0, y1, x, y, t, b, r, l;

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

	//check forecasts for exceeding ymax or ymin
	if(m_pAnalysis->m_opts.forecast.nDays > 0)
	{
		for(int d = 0; d < m_pAnalysis->m_opts.forecast.nDays; d++)
		{
			ymax = max(ymax, m_pAnalysis->m_opts.forecast.values[varID][d]);
			ymin = min(ymin, m_pAnalysis->m_opts.forecast.values[varID][d]);
		}
	}
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
		|| !penHGray.CreatePen( StringToLineStyle(m_graphOpts.m_H_Line_Style), 1, RGB(200,200,200))
		|| !penVGray.CreatePen( StringToLineStyle(m_graphOpts.m_V_Line_Style), 1, RGB(200,200,200)))
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
	pDC->Rectangle(rect);
	//force black bounding rectangle....(grrrr)
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.top);
	pDC->LineTo(rect.left, rect.top);

	//draw axis
	pDC->SelectObject( &penBlack2 );
	//pDC->MoveTo(x1, y0);
	//pDC->LineTo(x0, y0);
	//pDC->LineTo(x0, y1);
	pDC->MoveTo(x0, y1);
	pDC->LineTo(x0, y0);
	pDC->LineTo(x1, y0);
	//output headers
	CFont *pOldFont = pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);
	//HEADERS
//	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 + 1, m_pAnalysis->m_opts.stationName);
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 2 + 1, m_pAnalysis->m_opts.stationName);
	if(m_pAnalysis->periods[varID][0]->years > 1)
		temp.Format("%d - %d", m_pAnalysis->periods[varID][0]->baseYear,
			m_pAnalysis->periods[varID][0]->baseYear + m_pAnalysis->periods[varID][0]->years - 1);
	else
		temp.Format("%d", m_pAnalysis->periods[varID][0]->baseYear);
	//pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 3 * 2 + 4, temp);
	pDC->TextOut(x0 + (x1 - x0) / 2, rect.top + (y1 - rect.top) / 6 * 5 + 4, temp);
	temp.Format("%d Day Periods", m_pAnalysis->m_opts.periodLength);
	if (m_pAnalysis->m_opts.model.GetLength() > 0 && m_pAnalysis->m_usedExtremes && isNFDRS2016(m_pAnalysis->m_opts.model[0]))
	{
		if(m_pAnalysis->m_extremeIsMax[varID])
			temp += _T(" - Daily Max");
		else
			temp += _T(" - Daily Min");
	}
	pDC->TextOut(x0 + (x1 - x0) / 2, y0 + (rect.bottom - y0) / 3 * 2, temp);
	pDC->SelectObject(&vheaderFont);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->TextOut(rect.left + (x0 - rect.left) / 3, (y1 + y0) / 2, m_graphOpts.varName);
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	int nlegends = 0;
	CSize xTextSize;
	if(nP > 0)
	{
		//figure y extent
		//figure y axis values
		//output the y axis values
		pDC->SelectObject(&axisFont);
		pDC->SetTextAlign(TA_BASELINE | TA_RIGHT);
		pDC->SelectObject(&penBlack1);
		y = y0;
		int z = 0;
		while((z * ideal) > ymin)
			z--;
		//x = 0;
		x = z;// * ideal;//ymin;

		//added 4/14/2004 to fix screwy stuff with negative values
		ymin = z * ideal;
		while( y >= y1 )
		{
			if(ideal < 1.0)
				temp.Format("%.2f ", x * ideal);
			else if(ideal < 10)
				temp.Format("%.0f ", x * ideal);
			else
				temp.Format("%5.0f ", x * ideal);
			pDC->MoveTo(x0 - (x0 - rect.left)/10, y);
			pDC->LineTo(x0, y);
			if(m_graphOpts.m_Horizontal_Lines)
			{
				pDC->SelectObject(&penHGray);
				pDC->LineTo(x1, y);
				pDC->SelectObject(&penBlack1);
			}
			pDC->TextOut(x0 - (x0 - rect.left)/10, y, temp);
			x++;
			y = y0 - (int)( (x-z) * ideal / (ymax - ymin) * (double)(y0 - y1));
		}
		//merged variable axis labels
		CPen mergePen;
		if(m_graphOpts.m_OverallCP1 == TRUE)
		{
			CPen tPen;
			if(!tPen.CreatePen( PS_SOLID, 1, m_graphOpts.m_OverallCP1Color))
			{
				CString strMsg;
				strMsg.Format(_T("ERROR: Unable to create overall CP1 pen.\n\n"));
				AfxMessageBox(strMsg);
				return;
			}
			y = y0 - (int)((overallCP1 - ymin) / (ymax - ymin)* (double)(y0 - y1));
			CPen *restore = pDC->SelectObject(&tPen);
			pDC->MoveTo(x0, y);
			pDC->LineTo(x1, y);
			pDC->SelectObject(restore);
			temp.Format("%.0f%%", m_graphOpts.criticalPercentile * 100.0);
			COLORREF restoreColor = pDC->SetTextColor(m_graphOpts.m_OverallCP1Color);
			UINT oldAlign = pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
			pDC->TextOut(x0 + 1, y - 1, temp);
			pDC->SetTextColor(restoreColor);
			pDC->SetTextAlign(oldAlign);
		}
		if(m_graphOpts.m_OverallCP2 == TRUE)
		{
			CPen tPen;
			if(!tPen.CreatePen( PS_SOLID, 1, m_graphOpts.m_OverallCP2Color))
			{
				CString strMsg;
				strMsg.Format(_T("ERROR: Unable to create overall CP2 pen.\n\n"));
				AfxMessageBox(strMsg);
				return;
			}
			y = y0 - (int)((overallCP2 - ymin) / (ymax - ymin) * (double)(y0 - y1));
			CPen *restore = pDC->SelectObject(&tPen);
			pDC->MoveTo(x0, y);
			pDC->LineTo(x1, y);
			pDC->SelectObject(restore);
			temp.Format("%.0f%%", m_graphOpts.criticalPercentile2 * 100.0);
			COLORREF restoreColor = pDC->SetTextColor(m_graphOpts.m_OverallCP2Color);
			UINT oldAlign = pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
			pDC->TextOut(x0 + 1, y - 1, temp);
			pDC->SetTextColor(restoreColor);
			pDC->SetTextAlign(oldAlign);
		}
		//figure x axis steps
		barwidth = (x1 - x0) / (nP + 1);
		gap =  barwidth / nP;
		if((nP > 10) || (gap < 0))
		{
			barwidth = (x1 - x0) / nP;
			gap = 0;
		}
		if(barwidth == 0)
		{
			barwidth = 1;
			gap = 0;
		}
		//output data and x axis labels
		pDC->SetTextAlign(TA_TOP | TA_CENTER);
		pDC->SelectObject(&grayBrush);
		xTextSize = pDC->GetTextExtent("M", 1);
		l = x0 + gap;
		int xlabels = 0;
		for(int j = 0; j < nP; j++)
		{
			recsUsed += m_pAnalysis->periods[varID][j]->N();
			if(varID == 8)
				y = y0 - (int)((pcpSums[j]->mean / (ymax - ymin)) * (double)(y0 - y1));
			else
				y = y0 - (int)(((m_pAnalysis->periods[varID][j]->mean - ymin) / (ymax - ymin)) * (double)(y0 - y1));
			r = l + barwidth;
			if(m_graphOpts.isBargraph)//DRAW BARS NOW
			{
				if(varID == 8)
				{
					t = y - (int)((pcpSums[j]->stdDev / (ymax - ymin)) * (double)(y0 - y1));
					b = y + (int)((pcpSums[j]->stdDev / (ymax - ymin)) * (double)(y0 - y1));
				}
				else
				{
					//t = y - (int)(((m_pAnalysis->periods[varID][j]->stdDev - ymin) / (ymax - ymin)) * (double)(y0 - y1));
					//b = y + (int)(((m_pAnalysis->periods[varID][j]->stdDev - ymin) / (ymax - ymin)) * (double)(y0 - y1));
					double tTop = m_pAnalysis->periods[varID][j]->mean + m_pAnalysis->periods[varID][j]->stdDev;
					double tBottom = m_pAnalysis->periods[varID][j]->mean - m_pAnalysis->periods[varID][j]->stdDev;

					t = y0 - (int)(((tTop - ymin) / (ymax - ymin)) * (double)(y0 - y1));
					b = y0 - (int)(((tBottom - ymin) / (ymax - ymin)) * (double)(y0 - y1));
				}
				if( t == b)
				{
					t--;
					b++;
				}
				//draw rectangle of vals within 1 sd
				pDC->SelectObject(&penBlack1);
				pDC->SelectObject(&grayBrush);
				pDC->Rectangle(l, t, r, min(b, y0));

				//draw line thru rectangle at Mean Value
				pDC->SelectObject(&penBlack2);
				pDC->MoveTo(l, y);
				pDC->LineTo(r, y);

				if(m_graphOpts.m_Range_Tics)
				{
					//draw lines at min and max
					t = y0 - (int)(((maxs[j] - ymin) / (ymax - ymin)) * (double)(y0 - y1));
					b = y0 - (int)(((mins[j] - ymin) / (ymax - ymin)) * (double)(y0 - y1));
					pDC->SelectObject(&penGray);
					pDC->MoveTo(l, t);
					pDC->LineTo(r, t);
					pDC->MoveTo(l, b);
					pDC->LineTo(r, b);
					//draw range line from min to max
					pDC->MoveTo(l + (r - l) / 2, t);
					pDC->LineTo(l + (r - l) / 2, b);
				}
				if(m_graphOpts.m_CP_Tics)
				{
					//draw red line at criticalPercentile
					pDC->SelectObject(&penRed);
					t = y0 - (int)(((cps[j] - ymin) / (ymax - ymin)) * (double)(y0 - y1));
					pDC->MoveTo(l, t);
					pDC->LineTo(r, t);
				}
				//output x-axis tic and label
				pDC->SelectObject(&penBlack1);
				pDC->MoveTo(l, y0);

				if(m_pAnalysis->periods[varID][j]->start.GetDay() == 1
					|| nP < 12  || j == 0)
				{
					pDC->LineTo(l, y0 + (rect.bottom - y0) / 10);
					temp.Format("%d/%d", m_pAnalysis->periods[varID][j]->start.GetMonth(),
						m_pAnalysis->periods[varID][j]->start.GetDay());
					if(xlabels % 2 == 0)
						pDC->TextOut(l, y0 + (rect.bottom - y0) / 10, temp);
					else
						pDC->TextOut(l, y0 + (rect.bottom - y0) / 10 + xTextSize.cy, temp);
					xlabels++;
				}
				else
					pDC->LineTo(l, y0 + (rect.bottom - y0) / 15);
				//draw vertical lines
				if (m_graphOpts.m_Vertical_Lines)
				{
					pDC->SelectObject(&penVGray);
					pDC->MoveTo(l, y0);
					pDC->LineTo(l, y1);
					pDC->SelectObject(&penBlack1);
				}
			}

			l = r + gap;
		}
		if(!m_graphOpts.isBargraph)//DRAW LINE THRU MEANS
		{
			int lineCount = 1;
			CPen lgPen, cp1Pen, cp2Pen, maxsPen, minsPen;
			CBrush lgBrush;
			if(!lgPen.CreatePen( PS_SOLID, lineWidth, m_graphOpts.m_Line_Color)
				|| !lgBrush.CreateSolidBrush(m_graphOpts.m_Line_Color))
			{
				CString strMsg;
				strMsg.Format(_T("ERROR: Unable to create line graph pen.\n\n"));
				AfxMessageBox(strMsg);
				return;
			}
			int cnt = 0;
			if(m_pAnalysis->m_opts.numPeriods > 1)
				step = (double)(x1 - x0) / (double)(m_pAnalysis->m_opts.numPeriods - 1);
			else
				step = (x1 - x0) / 2;
			pDC->SelectObject( &lgPen );
			pDC->SelectObject( &lgBrush );
			bool drawing = false;
			for(int k = 0; k < m_pAnalysis->m_opts.numPeriods; k++)
			{
				x = x0 + (int)(k * step);
				if(varID == 8)
					y = y0 - (int)(pcpSums[k]->mean / (ymax - ymin) * (double)(y0 - y1));
				else
					y = y0 - (int)((m_pAnalysis->periods[varID][k]->mean - ymin) / (ymax - ymin) * (double)(y0 - y1));
				if(m_pAnalysis->periods[varID][k]->N() > 0)
				{
					pDC->SelectObject( &lgPen );
					pDC->SelectObject( &lgBrush );
					if(!cnt || !drawing)
						pDC->MoveTo(x, y);
					else
						pDC->LineTo(x, y);
					cnt++;
					drawing = true;
					//CString tempStr;
				}
				else
					drawing = false;
				pDC->SelectObject(&penBlack1);
				pDC->MoveTo(x, y0);
				if(m_pAnalysis->periods[varID][k]->start.GetDay() == 1
				|| nP < 12 || k == 0)
				{
					pDC->LineTo(x, y0 + (rect.bottom - y0) / 10);
					temp.Format("%d/%d", m_pAnalysis->periods[varID][k]->start.GetMonth(),
						m_pAnalysis->periods[varID][k]->start.GetDay());
					if(xlabels % 2 == 0)
						pDC->TextOut(x, y0 + (rect.bottom - y0) / 10, temp);
					else
						pDC->TextOut(x, y0 + (rect.bottom - y0) / 10 + xTextSize.cy, temp);
					xlabels++;
				}
				else
					pDC->LineTo(x, y0 + (rect.bottom - y0) / 15);
				//draw vertical lines
				if (m_graphOpts.m_Vertical_Lines)
				{
					pDC->SelectObject(&penVGray);
					pDC->MoveTo(x, y0);
					pDC->LineTo(x, y1);
					pDC->SelectObject(&penBlack1);
				}
				pDC->MoveTo(x, y);
			}
			if(m_graphOpts.m_cp1s)
			{
				lineCount++;
				if(!cp1Pen.CreatePen( PS_SOLID, cp1Width, m_graphOpts.m_cp1Color))
				{
					CString strMsg;
					strMsg.Format(_T("ERROR: Unable to create cp1 line graph pen.\n\n"));
					AfxMessageBox(strMsg);
					return;
				}
				pDC->SelectObject( &cp1Pen );
				drawing = false;
				cnt = 0;
				for(int k = 0; k < m_pAnalysis->m_opts.numPeriods; k++)
				{
					x = x0 + (int)(k * step);
					y = y0 - (int)((cps[k]  - ymin)/ (ymax - ymin) * (double)(y0 - y1));
					if(m_pAnalysis->periods[varID][k]->N() > 0)
					{
						if(!cnt || !drawing)
							pDC->MoveTo(x, y);
						else
							pDC->LineTo(x, y);
						cnt++;
						drawing = true;
					}
					else
						drawing = false;
				}
			}
			if(m_graphOpts.m_cp2s)
			{
				lineCount++;
				if(!cp2Pen.CreatePen( PS_SOLID, cp2Width, m_graphOpts.m_cp2Color))
				{
					CString strMsg;
					strMsg.Format(_T("ERROR: Unable to cp2 line graph pen.\n\n"));
					AfxMessageBox(strMsg);
					return;
				}
				pDC->SelectObject( &cp2Pen );
				drawing = false;
				cnt = 0;
				for(int k = 0; k < m_pAnalysis->m_opts.numPeriods; k++)
				{
					x = x0 + (int)(k * step);
					y = y0 - (int)((cp2s[k] - ymin) / (ymax - ymin) * (double)(y0 - y1));
					if(m_pAnalysis->periods[varID][k]->N() > 0)
					{
						if(!cnt || !drawing)
							pDC->MoveTo(x, y);
						else
							pDC->LineTo(x, y);
						cnt++;
						drawing = true;
					}
					else
						drawing = false;
				}
			}
			if(m_graphOpts.m_mins)
			{
				lineCount++;
				if(!minsPen.CreatePen( PS_SOLID, minsWidth, m_graphOpts.m_minsColor))
				{
					CString strMsg;
					strMsg.Format(_T("ERROR: Unable to mins line graph pen.\n\n"));
					AfxMessageBox(strMsg);
					return;
				}
				pDC->SelectObject( &minsPen );
				drawing = false;
				cnt = 0;
				for(int k = 0; k < m_pAnalysis->m_opts.numPeriods; k++)
				{
					x = x0 + (int)(k * step);
					y = y0 - (int)((mins[k] - ymin) / (ymax - ymin) * (double)(y0 - y1));
					if(m_pAnalysis->periods[varID][k]->N() > 0)
					{
						if(!cnt || !drawing)
							pDC->MoveTo(x, y);
						else
							pDC->LineTo(x, y);
						cnt++;
						drawing = true;
					}
					else
						drawing = false;
				}
			}
			if(m_graphOpts.m_maxs)
			{
				lineCount++;
				if(!maxsPen.CreatePen( PS_SOLID, maxsWidth, m_graphOpts.m_maxsColor))
				{
					CString strMsg;
					strMsg.Format(_T("ERROR: Unable to maxs line graph pen.\n\n"));
					AfxMessageBox(strMsg);
					return;
				}
				pDC->SelectObject( &maxsPen );
				drawing = false;
				cnt = 0;
				for(int k = 0; k < m_pAnalysis->m_opts.numPeriods; k++)
				{
					x = x0 + (int)(k * step);
					y = y0 - (int)((maxs[k] - ymin) / (ymax - ymin) * (double)(y0 - y1));
					if(m_pAnalysis->periods[varID][k]->N() > 0)
					{
						if(!cnt || !drawing)
							pDC->MoveTo(x, y);
						else
							pDC->LineTo(x, y);
						cnt++;
						drawing = true;
					}
					else
						drawing = false;
				}
			}
			//now draw legend for lines drawn (lower left corner, up to five of them)
			pDC->SetTextAlign(TA_TOP | TA_LEFT);
			pDC->SelectObject( &lgPen );
			xTextSize = pDC->GetTextExtent("M", 1);
			y = y0 + ((6 - lineCount) / 2)* (rect.bottom - y0) / 5;
			pDC->MoveTo(5, y + xTextSize.cy / 2);
			pDC->LineTo(x0 / 3, y + xTextSize.cy / 2);
			pDC->TextOut(x0 / 3 + 2, y, "Avg");
			if(m_graphOpts.m_maxs)
			{
				pDC->SelectObject(&maxsPen);
				y += (rect.bottom - y0) / 5;
				pDC->MoveTo(5, y + xTextSize.cy / 2);
				pDC->LineTo(x0 / 3, y + xTextSize.cy / 2);
				pDC->TextOut(x0 / 3 + 2, y, "Max");
			}
			if(m_graphOpts.m_mins)
			{
				pDC->SelectObject(&minsPen);
				y += (rect.bottom - y0) / 5;
				pDC->MoveTo(5, y + xTextSize.cy / 2);
				pDC->LineTo(x0 / 3, y + xTextSize.cy / 2);
				pDC->TextOut(x0 / 3 + 2, y, "Min");
			}
			if(m_graphOpts.m_cp1s)
			{
				pDC->SelectObject(&cp1Pen);
				y += (rect.bottom - y0) / 5;
				pDC->MoveTo(5, y + xTextSize.cy / 2);
				pDC->LineTo(x0 / 3, y + xTextSize.cy / 2);
				temp.Format("%.0f%%ile", m_graphOpts.criticalPercentile * 100.0);
				pDC->TextOut(x0 / 3 + 2, y, temp);
			}
			if(m_graphOpts.m_cp2s)
			{
				pDC->SelectObject(&cp2Pen);
				y += (rect.bottom - y0) / 5;
				pDC->MoveTo(5, y + xTextSize.cy / 2);
				pDC->LineTo(x0 / 3, y + xTextSize.cy / 2);
				temp.Format("%.0f%%ile", m_graphOpts.criticalPercentile2 * 100.0);
				pDC->TextOut(x0 / 3 + 2, y, temp);
			}
		}

		//now draw any overlays
		CBrush brush;
		pDC->SetTextAlign(TA_BOTTOM | TA_LEFT);
		xTextSize = pDC->GetTextExtent("M", 1);
		triWdth = max(1, xTextSize.cx / 3);
		triHgt = max(1, xTextSize.cy / 2);
		radius = max(1, xTextSize.cx / 6);
		bool hasOverlays = false;
		int yFD = 0, yLFD = 0, yMFD = 0;
		for(int i = 0; i < OVERLAYS; i++)
		{
			if(m_pAnalysis->m_opts.overLayYears[i] > 0)
			{
				int brushWidth = m_pAnalysis->m_opts.lineWidths[i];
				hasOverlays = true;
				yFD = 0;
				yLFD = 0;
				yMFD = 0;
				if(m_pAnalysis->m_opts.numPeriods > 1)
					step = (double)(x1 - x0) / (double)(m_pAnalysis->m_opts.numPeriods - 1);
				else
					step = (x1 - x0) / 2;
				pDC->SelectObject( pOldBrush );
				brush.DeleteObject();
				brush.CreateSolidBrush(m_pAnalysis->m_opts.overLayColors[i]);
				pDC->SelectObject(&brush);
				unsigned auxType[8];
				bool Round = true;
				if(pDC->IsPrinting())
				{
					CDC *screenDC = GetDC();
					brushWidth = ScreenPixelsToPrinterPixels(screenDC, destDC, brushWidth);
					ReleaseDC(screenDC);
				}
				int c1 = CDashLine::GetPattern(auxType, Round, brushWidth,
					m_pAnalysis->m_opts.lineStyles[i]);
				CDashLine auxLine(*pDC, auxType, c1);
				int cnt = 0;
				int loc = m_pAnalysis->m_opts.overLayYears[i] - m_pAnalysis->periods[varID][0]->baseYear;
				if(loc < 0 || loc >= m_pAnalysis->periods[varID][0]->years && m_pAnalysis->m_opts.tPeriods)//auxiliary year
				{
					//make sure there's data!!!!!!
					//if(m_pAnalysis->m_opts.tPeriods &&
					loc = m_pAnalysis->m_opts.overLayYears[i] - m_pAnalysis->m_opts.tPeriods[varID][0]->baseYear;
					pDC->BeginPath();
					int k;
					for(k = 0; k < m_pAnalysis->m_opts.numtPeriods; k++)
					{
						if (m_pAnalysis->m_opts.tPeriods[varID][k]->yStats[loc].N() > 0)
						{
							if (!m_graphOpts.isBargraph)
								x = x0 + (int)(k * step);
							else
								x = x0 + k * (barwidth + gap) + (barwidth + gap) / 2;
							if (varID == 8)
								y = y0 - (int)(pcpAuxSums[k]->yStats[loc].Mean() / (ymax - ymin) * (double)(y0 - y1));
							else
								y = y0 - (int)((m_pAnalysis->m_opts.tPeriods[varID][k]->yStats[loc].Mean() - ymin) / (ymax - ymin) * (double)(y0 - y1));
							if (!cnt)
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
					DrawPathOutline(m_pAnalysis->m_opts.overLayColors[i], *pDC, Round, brushWidth);
					//output fires for overlaid auxyear
					for(k = 0; k < m_pAnalysis->m_opts.numtPeriods; k++)
					{
						if(m_pAnalysis->m_opts.tPeriods[varID][k]->yStats[loc].N() > 0)
						{
							if(!m_graphOpts.isBargraph)
								x = x0 + (int)(k * step);
							else
								x = x0 + k * (barwidth + gap)  + (barwidth + gap) / 2;
							if(varID == 8)
								y = y0 - (int)(pcpAuxSums[k]->yStats[loc].Mean() / (ymax - ymin) * (double)(y0 - y1));
							else
								y = y0 - (int)((m_pAnalysis->m_opts.tPeriods[varID][k]->yStats[loc].Mean() - ymin) / (ymax - ymin) * (double)(y0 - y1));
							bool lfd = false, mfd = false, fd = false;
							if(m_graphOpts.m_LFD)
							{
								switch(m_graphOpts.m_Cause)
								{
								case 0://all
									if(m_pAnalysis->m_opts.tFires[loc][k].hAcresMax + m_pAnalysis->m_opts.tFires[loc][k].lAcresMax >= m_graphOpts.m_LFDAcres)
										lfd = true;
									break;
								case 1://lightning
									if(m_pAnalysis->m_opts.tFires[loc][k].lAcresMax >= m_graphOpts.m_LFDAcres)
										lfd = true;
									break;
								case 2://human
									if(m_pAnalysis->m_opts.tFires[loc][k].hAcresMax >= m_graphOpts.m_LFDAcres)
										lfd = true;
									break;
								}
							}
							if(m_graphOpts.m_MFD)
							{
								switch(m_graphOpts.m_Cause)
								{
								case 0://all
									if(m_pAnalysis->m_opts.tFires[loc][k].human + m_pAnalysis->m_opts.tFires[loc][k].lightning >= m_graphOpts.m_MFDFires)
										mfd = true;
									break;
								case 1://lightning
									if(m_pAnalysis->m_opts.tFires[loc][k].lightning >= m_graphOpts.m_MFDFires)
										mfd = true;
									break;
								case 2://human
									if(m_pAnalysis->m_opts.tFires[loc][k].human >= m_graphOpts.m_MFDFires)
										mfd = true;
									break;
								}
							}
							if(mfd)
							{
								CPoint pts[3];
								pts[0].x = x;
								pts[0].y = y + triHgt;
								pts[1].x = x - triWdth;
								pts[1].y = y;
								pts[2].x = x + triWdth;
								pts[2].y = y;
								pDC->SelectObject(&penBlack1);
								pDC->Polygon(pts, 3);
								yMFD++;
								yFD++;
							}
							if(lfd)
							{
								CPoint pts[3];
								pts[0].x = x;
								pts[0].y = y - triHgt;
								pts[1].x = x - triWdth;
								pts[1].y = y;
								pts[2].x = x + triWdth;
								pts[2].y = y;
								pDC->SelectObject(&penBlack1);
								pDC->Polygon(pts, 3);
								yLFD++;
								if (!mfd)
									yFD++;
							}
							if(m_graphOpts.m_FD && !mfd && !lfd)//!(mfd || lfd))
							{
								switch(m_graphOpts.m_Cause)
								{
								case 0://all
									if(m_pAnalysis->m_opts.tFires[loc][k].human + m_pAnalysis->m_opts.tFires[loc][k].lightning > 0)
										fd = true;
									break;
								case 1://lightning
									if(m_pAnalysis->m_opts.tFires[loc][k].lightning > 0)
										fd = true;
									break;
								case 2://human
									if(m_pAnalysis->m_opts.tFires[loc][k].human > 0)
										fd = true;
									break;
								}
								if(fd)
								{
									pDC->SelectObject(&penBlack1);
									pDC->Ellipse(x - radius, y - radius, x + radius, y + radius);
									yFD++;
								}
							}
						}
					}
				}
				else
				{//normal years
					pDC->BeginPath();
					int k;
					cnt = 0;
					for(k = 0; k < m_pAnalysis->m_opts.numPeriods; k++)
					{
						if(m_pAnalysis->periods[varID][k]->yStats[loc].N() > 0)
						{
							if(!m_graphOpts.isBargraph)
								x = x0 + (int)(k * step);
							else
								x = x0 + k * (barwidth + gap)  + (barwidth + gap) / 2;
							if(varID == 8)
								y = y0 - (int)(pcpSums[k]->yStats[loc].Mean() / (ymax - ymin) * (double)(y0 - y1));
							else
								y = y0 - (int)((m_pAnalysis->periods[varID][k]->yStats[loc].Mean() - ymin) / (ymax - ymin) * (double)(y0 - y1));
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
					DrawPathOutline(m_pAnalysis->m_opts.overLayColors[i], *pDC, Round, m_pAnalysis->m_opts.lineWidths[i]);
					//output fires for overlaid year
					for(k = 0; k < m_pAnalysis->m_opts.numPeriods; k++)
					{
						if(m_pAnalysis->periods[varID][k]->yStats[loc].N() > 0)
						{
							if(!m_graphOpts.isBargraph)
								x = x0 + (int)(k * step);
							else
								x = x0 + k * (barwidth + gap)  + (barwidth + gap) / 2;
							if(varID == 8)
								y = y0 - (int)(pcpSums[k]->yStats[loc].Mean() / (ymax - ymin) * (double)(y0 - y1));
							else
								y = y0 - (int)((m_pAnalysis->periods[varID][k]->yStats[loc].Mean() - ymin) / (ymax - ymin) * (double)(y0 - y1));
							bool lfd = false, mfd = false, fd = false;
							if(m_graphOpts.m_LFD)
							{
								switch(m_graphOpts.m_Cause)
								{
								case 0://all
									if(m_pAnalysis->m_opts.fires[loc][k].hAcresMax + m_pAnalysis->m_opts.fires[loc][k].lAcresMax >= m_graphOpts.m_LFDAcres)
										lfd = true;
									break;
								case 1://lightning
									if(m_pAnalysis->m_opts.fires[loc][k].lAcresMax >= m_graphOpts.m_LFDAcres)
										lfd = true;
									break;
								case 2://human
									if(m_pAnalysis->m_opts.fires[loc][k].hAcresMax >= m_graphOpts.m_LFDAcres)
										lfd = true;
									break;
								}
							}
							if(m_graphOpts.m_MFD)
							{
								switch(m_graphOpts.m_Cause)
								{
								case 0://all
									if(m_pAnalysis->m_opts.fires[loc][k].human + m_pAnalysis->m_opts.fires[loc][k].lightning >= m_graphOpts.m_MFDFires)
										mfd = true;
									break;
								case 1://lightning
									if(m_pAnalysis->m_opts.fires[loc][k].lightning >= m_graphOpts.m_MFDFires)
										mfd = true;
									break;
								case 2://human
									if(m_pAnalysis->m_opts.fires[loc][k].human >= m_graphOpts.m_MFDFires)
										mfd = true;
									break;
								}
							}
							if(mfd)
							{
								CPoint pts[3];
								pts[0].x = x;
								pts[0].y = y + triHgt;
								pts[1].x = x - triWdth;
								pts[1].y = y;
								pts[2].x = x + triWdth;
								pts[2].y = y;
								pDC->SelectObject(&penBlack1);
								pDC->Polygon(pts, 3);
								yMFD++;
								yFD++;
							}
							if(lfd)
							{
								CPoint pts[3];
								pts[0].x = x;
								pts[0].y = y - triHgt;
								pts[1].x = x - triWdth;
								pts[1].y = y;
								pts[2].x = x + triWdth;
								pts[2].y = y;
								pDC->SelectObject(&penBlack1);
								pDC->Polygon(pts, 3);
								yLFD++;
								if(!mfd)
									yFD++;
							}
							if(m_graphOpts.m_FD && !mfd && ! lfd)//!(mfd || lfd))
							{
								switch(m_graphOpts.m_Cause)
								{
								case 0://all
									if(m_pAnalysis->m_opts.fires[loc][k].human + m_pAnalysis->m_opts.fires[loc][k].lightning > 0)
										fd = true;
									break;
								case 1://lightning
									if(m_pAnalysis->m_opts.fires[loc][k].lightning > 0)
										fd = true;
									break;
								case 2://human
									if(m_pAnalysis->m_opts.fires[loc][k].human > 0)
										fd = true;
									break;
								}
								if(fd)
								{
									pDC->SelectObject(&penBlack1);
									pDC->Ellipse(x - radius, y - radius, x + radius, y + radius);
									yFD++;
								}
							}
						}
					}
				}
				//add legend entry for year
				pDC->BeginPath();
				auxLine.MoveTo((nlegends < 3) ? x0 : x0 + (x1 - x0) / 7,
					y0 + xTextSize.cy * ((nlegends < 3) ? nlegends + 3 : nlegends));
				auxLine.LineTo((nlegends < 3) ? x0 + (x1 - x0) / 20 : x0 + (x1 - x0) / 7 + (x1 - x0) / 20,
					y0 + xTextSize.cy * ((nlegends < 3) ? nlegends + 3 : nlegends));
				pDC->EndPath();
				DrawPathOutline(m_pAnalysis->m_opts.overLayColors[i], *pDC, Round, m_pAnalysis->m_opts.lineWidths[i]);

				temp.Format("%d", m_pAnalysis->m_opts.overLayYears[i]);
				if(m_graphOpts.m_FD || m_graphOpts.m_LFD  || m_graphOpts.m_MFD)
				{
					bool hadone = false;
					temp += " (";
					if(m_graphOpts.m_FD)
					{
						string.Format("%d", yFD);
						temp += string;
						hadone = true;
					}
					if(m_graphOpts.m_LFD)
					{
						if(hadone)
							temp += "/";
						string.Format("%d", yLFD);
						temp += string;
						hadone = true;
					}
					if(m_graphOpts.m_MFD)
					{
						if(hadone)
							temp += "/";
						string.Format("%d", yMFD);
						temp += string;
					}
					temp += ")";
				}
				pDC->TextOut((nlegends < 3) ? x0 + (x1 - x0) / 20 + 2 : x0 + (x1 - x0) / 7 + (x1 - x0) / 20 + 2,
					y0 + xTextSize.cy * ((nlegends < 3) ? nlegends + 3 : nlegends) + xTextSize.cy / 2, temp);
				nlegends++;
			}
		}
		int rbound = x1;
		pDC->SetTextAlign(TA_BOTTOM | TA_RIGHT);
		if(optionType == 1 && m_pAnalysis->m_opts.model.GetLength() > 0)
		{	//output model code in lower right corner
			CString mStr = "Model: " + m_pAnalysis->m_opts.model;
			//xTextSize = pDC->GetTextExtent(mStr);
			//pDC->TextOut(x1 - xTextSize.cx , y0 + xTextSize.cy * 4 , mStr);
			pDC->TextOut(rbound, y0 + xTextSize.cy * 4 , mStr);
		}
		//output # observations in lower right corner
		temp.Format("%ld Wx Observations", recsUsed);
		//xTextSize = pDC->GetTextExtent(temp);
		//pDC->TextOut(x1 - xTextSize.cx , y0 + xTextSize.cy * 5 , temp);
		pDC->TextOut(rbound, y0 + xTextSize.cy * 5 , temp);
		//output version/DateTime stamp
		CString dStr = m_pAnalysis->m_opts.dateTime.Format("%m/%d/%Y-%H:%M");
		//temp.Format("FF+%s %s", verStr, dStr);
		temp.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
		//temp = m_pAnalysis->m_opts.dateTime.Format("FF+3.0 %m/%d/%Y-%H:%M");
		//xTextSize = pDC->GetTextExtent(temp);
		//pDC->TextOut(x1 - xTextSize.cx , y0 + xTextSize.cy * 6 , temp);
		pDC->TextOut(rbound, y0 + xTextSize.cy * 6 , temp);
		//output Fires symbols in upper left corner
		if(hasOverlays)
		{
			pDC->SetTextAlign(TA_TOP | TA_LEFT);
			pDC->SelectObject(&penBlack1);
			pDC->SelectObject(&whiteBrush);
			xTextSize = pDC->GetTextExtent("M", 1);
			y = y1;
			x = x0 + (x0 - rect.left) / 10;
			if(m_graphOpts.m_FD)
			{
				int yf = y + xTextSize.cy / 2;
				temp.Format("Fire Day");
				pDC->TextOut(x + triWdth + 1, y, temp);
				pDC->Ellipse(x - radius, yf - radius, x + radius, yf + radius);
				y += xTextSize.cy + xTextSize.cy / 20;
			}
			if(m_graphOpts.m_LFD)
			{
				int yf = y + xTextSize.cy / 2;
				CPoint pts[3];
				pts[0].x = x;
				pts[0].y = yf - triHgt;
				pts[1].x = x - triWdth;
				pts[1].y = yf;
				pts[2].x = x + triWdth;
				pts[2].y = yf;
				pDC->Polygon(pts, 3);

				temp.Format("Large Fire Day (%d+)", m_graphOpts.m_LFDAcres);
				pDC->TextOut(x + triWdth + 1, y, temp);
				y += xTextSize.cy + xTextSize.cy / 20;
			}
			if(m_graphOpts.m_MFD)
			{
				int yf = y + xTextSize.cy / 2;
				CPoint pts[3];
				pts[0].x = x;
				pts[0].y = yf + triHgt;
				pts[1].x = x - triWdth;
				pts[1].y = yf;
				pts[2].x = x + triWdth;
				pts[2].y = yf;
				pDC->Polygon(pts, 3);
				temp.Format("Multi Fire Day (%d+)", m_graphOpts.m_MFDFires);
				pDC->TextOut(x + triWdth + 1, y, temp);
			}
		}
	}
	//now do any forecasts...
	if(m_pAnalysis->m_opts.forecast.nBins > 0 && m_pAnalysis->m_opts.forecast.values[varID] != NULL)
	{
		int fxBrushWidth = m_graphOpts.m_fxWidth;
		if(destDC->IsPrinting())
		{
			CDC *screenDC = GetDC();
			fxBrushWidth = ScreenPixelsToPrinterPixels(screenDC, destDC, fxBrushWidth);
			ReleaseDC(screenDC);
		}
		//have forecasted values
		if(m_pAnalysis->m_opts.forecast.nBins == 1 && m_pAnalysis->m_opts.forecast.overlap)
		{//just draw a circle
			CBrush fxBrush;
			if(fxBrush.CreateSolidBrush(m_pAnalysis->m_opts.forecast.lineColor))
			{
				pDC->SelectObject(&penBlack1);
				pDC->SelectObject(&fxBrush);
				int p = m_pAnalysis->m_opts.forecast.startP;
				y = y0 - (int)(((m_pAnalysis->m_opts.forecast.binVals[varID][0] - ymin) / (ymax - ymin)) * (double)(y0 - y1));
				if(!m_graphOpts.isBargraph)
					x = x0 + (int)(p * step);
				else
					x = x0 + p * (barwidth + gap)  + (barwidth + gap) / 2;
				pDC->Ellipse((int)(x - step), (int)(y - step), (int)(x + step), (int)(y + step));
			}
		}
		else//greater than 1 or no overlap, can draw a line
		{
			unsigned Type1[8];
			int c1 = CDashLine::GetPattern(Type1, false, fxBrushWidth, m_graphOpts.m_fxStyle);//m_pAnalysis->m_opts.forecast.lineStyle);
			CDashLine line(*pDC, Type1, c1);
			pDC->BeginPath();
			for(int d = 0; d < m_pAnalysis->m_opts.forecast.nBins; d++)
			{
				int p = m_pAnalysis->m_opts.forecast.startP + d;
				y = y0 - (int)(((m_pAnalysis->m_opts.forecast.binVals[varID][d] - ymin) / (ymax - ymin)) * (double)(y0 - y1));
				if(!m_graphOpts.isBargraph)
					x = x0 + (int)(p * step);
				else
					x = x0 + p * (barwidth + gap)  + (barwidth + gap) / 2;
				if(d == 0)
					line.MoveTo(x, y);
				else
					line.LineTo(x, y);
			}
			pDC->EndPath();
			DrawPathOutline(m_graphOpts.m_fxColor, *pDC, false, fxBrushWidth);
		}
		//now do legend for forecast
		unsigned Type1[8];
		int c1 = CDashLine::GetPattern(Type1, false, fxBrushWidth, m_graphOpts.m_fxStyle);//m_pAnalysis->m_opts.forecast.lineStyle);
		CDashLine line(*pDC, Type1, c1);
		pDC->BeginPath();
		line.MoveTo((nlegends < 3) ? x0 : x0 + (x1 - x0) / 7,
			y0 + xTextSize.cy * ((nlegends < 3) ? nlegends + 3 : nlegends));
		line.LineTo((nlegends < 3) ? x0 + (x1 - x0) / 20 : x0 + (x1 - x0) / 7 + (x1 - x0) / 20,
			y0 + xTextSize.cy * ((nlegends < 3) ? nlegends + 3 : nlegends));
		pDC->EndPath();
		DrawPathOutline(m_graphOpts.m_fxColor, *pDC, false, fxBrushWidth);

		temp = "Projected";
		pDC->SetTextAlign(TA_BOTTOM | TA_LEFT);
		pDC->TextOut((nlegends < 3) ? x0 + (x1 - x0) / 20 + 2 : x0 + (x1 - x0) / 7 + (x1 - x0) / 20 + 2,
			y0 + xTextSize.cy * ((nlegends < 3) ? nlegends + 3 : nlegends) + xTextSize.cy / 2, temp);
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

/*void CGraphView::DrawPathOutline(const COLORREF& c, CDC& dc, bool Round, int PenSize)
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

void CGraphView::OnFileSave()
{
	// TODO: Add your command handler code here
	OnFileSaveAs();
}

BOOL CGraphView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box

	return DoPreparePrinting(pInfo);
	//return CView::OnPreparePrinting(pInfo);
}

void CGraphView::PrintGraph()

{
	theApp.SetLandscape();
	CView::OnFilePrint();
}

// added for batch2
void	CGraphView::OnViewHeader()
{
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");

	// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(m_pAnalysis->pDB);//fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(stream, "FireFamily Plus Header Information...\n");

	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", m_pAnalysis->pDB->GetDatabaseName());

	fprintf(stream,"\n");
	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,this->m_pAnalysis->fpSet);
	fprintf(stream,"%s",headerStr);
	// end added for batch
	fclose(stream);

	//now display the file in a child window
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate =
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("TextFile"))
		{
			CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(fName);
			pDoc->isTemp = true;
			free(fName);
			CString tTitle;
			tTitle.Format("FireFamily Plus Header Information...");

			pDoc->SetTitle(tTitle);
			return;
		}
	}
	free(fName);
}

void CGraphView::OnExportData()
{
	m_pAnalysis->StatsGraphDataListing(varID);
}

void CGraphView::SaveGraph(CString _outName, CRect *_outRect)
{
	CDC *whdc = AfxGetMainWnd()->GetDC();
	CDC memhdc;
	//HDC whdc = ::GetDC();
	memhdc.CreateCompatibleDC(NULL);
	HBITMAP picture = CreateCompatibleBitmap(whdc->m_hDC, _outRect->Width(), _outRect->Height());
	HGDIOBJ Old = SelectObject(memhdc.m_hDC, picture);
	//CRect bnd(0, 0, _outRect->Width(), _outRect->Height());
	CRect rect(0, 0, _outRect->Width(), _outRect->Height());
	DrawToDCRegion(&memhdc, rect);
	char fName[256];
	strcpy_s(fName, _outName);
	SaveBitmapFile(memhdc.m_hDC, picture, fName);
	SelectObject(memhdc.m_hDC, Old);
	//ReleaseDC(whdc);
	memhdc.DeleteDC();
	DeleteObject(picture);
}

void CGraphView::SaveGraph(void)
{
	CGraphSaveDialog sgd(this);
	if(sgd.DoModal() == IDOK)
	{
		chdir(theApp.userDir);
		CFileDialog fd(false, theApp.GetGraphicsExt(), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			theApp.GetGraphicsFileStr(), this);
		//fd.SetDefExt(theApp.GetGraphicsExt());
		if(fd.DoModal() == IDOK)
		{
			CRect rect(0, 0, sgd.width, sgd.height);
			SaveGraph(fd.GetPathName(), &rect);
			/*CDC *whdc = GetDC();
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
			DeleteObject(picture);*/
			theApp.SetUserDir(fd.GetPathName());
			theApp.SetGraphicsExt(fd.GetFileExt());
		}
		chdir(theApp.dbDir);
	}
}
// DPFormView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DPFormView.h"
#include "DPDoc.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CDPFormView

IMPLEMENT_DYNCREATE(CDPFormView, CView)

CDPFormView::CDPFormView()
//	: CFormView(CDPFormView::IDD)
{
	//{{AFX_DATA_INIT(CDPFormView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CDPFormView::~CDPFormView()
{
}

/*void CDPFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDPFormView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}*/


BEGIN_MESSAGE_MAP(CDPFormView, CView)
	//{{AFX_MSG_MAP(CDPFormView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDPFormView diagnostics

#ifdef _DEBUG
void CDPFormView::AssertValid() const
{
	CView::AssertValid();
}

void CDPFormView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDPFormView message handlers

void CDPFormView::OnDraw(CDC* pDC) 
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

void CDPFormView::DrawToDCRegion(CDC *pDC, CRect rect)
{
	CString tmp;
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
	CDPDoc* pDoc = (CDPDoc*)GetDocument();
	// TODO: add draw code here
	//figure axis
	int xt0, xt1, yt0, yt1, 
		xa0, xa1, ya0, ya1,
		xb0, xb1, yb0, yb1,
		xc0, xc1, yc0, yc1,
		xd0, xd1, yd0, yd1,
		a, b, c, d, y, x, bw = 10;
	x = rect.Width() / 8 * 3;
	y = rect.Height() / 4;
	y *= 9;
	y /= 10;
	//top graph
	xt0 = rect.left + rect.Width() / 10;
	xt1 = rect.right - (xt0 - rect.left) / 10;
	yt1 = rect.top + rect.Height() / 20;
	yt0 = yt1  + y;
	//graph a
	xa0 = xt0;
	xa1 = xa0 + x;
	ya1 = yt0 + y / 4;
	ya0 = ya1 + y;
	//graph b
	xb1 = xt1;
	//xb0 = rect.right - x - rect.Width() / 20;
	xb0 = xb1 - x;
	yb1 = ya1;
	yb0 = ya0;
	//graph c
	xc0 = xa0;
	xc1 = xa1;
	yc1 = ya0 + y / 4;
	yc0 = yc1 + y;
	//graph d
	xd0 = xb0;
	xd1 = xb1;
	yd1 = yc1;
	yd0 = yc0;

	//figure barwidth
	if(pDoc->nDP > 0)
		bw = (xt1 - xt0) / 5 / pDoc->nDP;
	//x0 = rect.left + rect.Width() / 10;
	//x1 = rect.right - (x0 - rect.left)/ 10;
	//y1 = rect.top + rect.Height() / 20;
	//y0 = rect.bottom - 2 * (y1 - rect.top);
	//load pens and fonts
    CPen penBlack2, penBlack1, penRed, penBlue, penMagenta, gridPen;  // Construct it, then initialize
    if(!penBlack2.CreatePen( PS_SOLID, 2, RGB(0,0,0)) 
		|| !penBlack1.CreatePen( PS_SOLID, 1, RGB(0,0,0))     
		|| !penBlue.CreatePen( PS_SOLID, 1, RGB(0,200,200))     
		|| !penMagenta.CreatePen( PS_SOLID, 1, RGB(200,0,200))     
		|| !gridPen.CreatePen( PS_DOT, 1, RGB(220,220,220))     
		|| !penRed.CreatePen( PS_SOLID, 1, RGB(200,0,0)) )    
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Resources too low for creating drawing pens.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CFont headerFont, axisFont, vheaderFont;
	if(!headerFont.CreateFont( (yt1 - rect.top)  * 2/ 3, (xt0 - rect.left)/ 6, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")
		|| !vheaderFont.CreateFont((xt0 - rect.left) / 4,  (yt1 - rect.top) / 4 , 900, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")
//		|| !axisFont.CreateFont( (yt1 - rect.top) / 3, (xt0 - rect.left) / 10, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		|| !axisFont.CreateFont( (yt1 - rect.top) / 2, (xt0 - rect.left) / 8, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica") )
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create probability graph fonts.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CBrush whiteBrush, adBrush, fdBrush, lfdBrush, mfdBrush;
	if(!whiteBrush.CreateSolidBrush(0x00FFFFFF)
		|| !adBrush.CreateSolidBrush(RGB(0, 0, 220))
		|| !fdBrush.CreateHatchBrush(HS_FDIAGONAL, RGB(0,200,200))
		|| !lfdBrush.CreateHatchBrush(HS_CROSS, RGB(200,0,200))
		|| !mfdBrush.CreateHatchBrush(HS_BDIAGONAL, RGB(200, 0, 0))
		)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create brushes.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	
	CBrush *pOldBrush = pDC->SelectObject(&whiteBrush);
	CPen *pOldPen = pDC->SelectObject(&penBlack1);
	pDC->Rectangle(rect);
	pDC->MoveTo(rect.left, rect.top);
	pDC->LineTo(rect.left, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.bottom - 1);
	pDC->LineTo(rect.right - 1, rect.top);
	pDC->LineTo(rect.left, rect.top);
	//draw all axis
	pDC->MoveTo(xt0, yt1);
	pDC->LineTo(xt0, yt0);
	pDC->LineTo(xt1, yt0);

	pDC->MoveTo(xa0, ya1);
	pDC->LineTo(xa0, ya0);
	pDC->LineTo(xa1, ya0);

	pDC->MoveTo(xb0, yb1);
	pDC->LineTo(xb0, yb0);
	pDC->LineTo(xb1, yb0);

	pDC->MoveTo(xc0, yc1);
	pDC->LineTo(xc0, yc0);
	pDC->LineTo(xc1, yc0);

 	pDC->MoveTo(xd0, yd1);
	pDC->LineTo(xd0, yd0);
	pDC->LineTo(xd1, yd0);
	
	CFont *pOldFont = pDC->SelectObject(&headerFont);
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	tmp.Format("Classes and Percent Days Based on %s", pDoc->fires->shortName);	
	pDC->TextOut(rect.left + rect.Width() / 2, yt1, tmp);
	//draw y labels and grid lines
	pDC->SelectObject(&axisFont);
	pDC->SetTextAlign(TA_RIGHT | TA_BASELINE);
	d = pDC->GetTextExtent("100").cy / 2;
	c = (xt0 - rect.left) / 10;
	int i;
	for(i = 1; i <= 5; i++)
	{
		tmp.Format("%d", i * 20);
		//section t
		a = xt0 - 1;
		b = yt0 - (y * i / 5 );
		pDC->SelectObject(&penBlack1);
		pDC->MoveTo(a, b);
		pDC->LineTo(a - c, b);
		pDC->TextOut(a - c, b + d, tmp);
		pDC->SelectObject(&gridPen);
		pDC->MoveTo(xt0 + 1, b);
		pDC->LineTo(xt1, b);
		//section a
		a = xa0 - 1;
		b = ya0 - (y * i / 5);
		pDC->SelectObject(&penBlack1);
		pDC->MoveTo(a, b);
		pDC->LineTo(a - c, b);
		pDC->TextOut(a - c, b + d, tmp);
		pDC->SelectObject(&gridPen);
		pDC->MoveTo(xa0 + 1, b);
		pDC->LineTo(xa1, b);
		//section b
		a = xb0 - 1;
		b = yb0 - (y * i / 5);
		pDC->SelectObject(&penBlack1);
		pDC->MoveTo(a, b);
		pDC->LineTo(a - c, b);
		pDC->TextOut(a - c, b + d, tmp);
		pDC->SelectObject(&gridPen);
		pDC->MoveTo(xb0 + 1, b);
		pDC->LineTo(xb1, b);
		//section c
		a = xc0 - 1;
		b = yc0 - (y * i / 5);
		pDC->SelectObject(&penBlack1);
		pDC->MoveTo(a, b);
		pDC->LineTo(a - c, b);
		pDC->TextOut(a - c, b + d, tmp);
		pDC->SelectObject(&gridPen);
		pDC->MoveTo(xc0 + 1, b);
		pDC->LineTo(xc1, b);
		//section d
		a = xd0 - 1;
		b = yd0 - (y * i / 5);
		pDC->SelectObject(&penBlack1);
		pDC->MoveTo(a, b);
		pDC->LineTo(a - c, b);
		pDC->TextOut(a - c, b + d, tmp);
		pDC->SelectObject(&gridPen);
		pDC->MoveTo(xd0 + 1, b);
		pDC->LineTo(xd1, b);
	}
	tmp = "%";
	d = pDC->GetTextExtent(tmp).cy / 2;
	c += pDC->GetTextExtent("100").cx;
	pDC->TextOut(xt0 - c - 1, yt0 - y / 2 - d, tmp);
	pDC->TextOut(xa0 - c - 1, ya0 - y / 2 - d, tmp);
	pDC->TextOut(xb0 - c - 1, yb0 - y / 2 - d, tmp);
	pDC->TextOut(xc0 - c - 1, yc0 - y / 2 - d, tmp);
	pDC->TextOut(xd0 - c - 1, yd0 - y / 2 - d, tmp);
	//ok, graph backgrounds complete
	pDC->SetTextAlign(TA_CENTER | TA_TOP);
	pDC->SelectObject(&penBlack1);
	for(i = 0; i < pDoc->nDP; i++)
	{
		tmp.Format("%d", i + 1);
		//all days
		pDC->SelectObject(&adBrush);
		a = xt0 + bw / 2 + i * 5 * bw;
		if(pDoc->nd > 0)
			b = (int)(((double)pDoc->cd[i]) / ((double)pDoc->nd) * (double) y);
		else
			b = 0;
		pDC->Rectangle(a, yt0 - b, a + bw, yt0);
		a = xa0 + bw / 2 + i * 2 * bw;
		pDC->Rectangle(a, ya0 - b, a + bw, ya0);
		a += bw / 2;
		pDC->MoveTo(a, ya0 + 1);
		pDC->LineTo(a, ya0 + y / 25);
		pDC->TextOut(a, ya0 + y / 25, tmp);
		//fire days
		pDC->SelectObject(&fdBrush);
		a = xt0 + bw / 2 + i * 5 * bw + bw;
		if(pDoc->nfd > 0)
			b = (int)(((double)pDoc->cfd[i]) / ((double)pDoc->nfd) * (double) y);
		else
			b = 0;
		pDC->Rectangle(a, yt0 - b, a + bw, yt0);
		a = xb0 + bw / 2 + i * 2 * bw;
		pDC->Rectangle(a, yb0 - b, a + bw, yb0);
		a += bw / 2;
		pDC->MoveTo(a, yb0 + 1);
		pDC->LineTo(a, yb0 + y / 25);
		pDC->TextOut(a, yb0 + y / 25, tmp);
		//large fire days
		pDC->SelectObject(&lfdBrush);
		a = xt0 + bw / 2 + i * 5 * bw + 2 * bw;
		if(pDoc->nlfd > 0)
			b = (int)(((double)pDoc->clfd[i]) / ((double)pDoc->nlfd) * (double) y);
		else
			b = 0;
		pDC->Rectangle(a, yt0 - b, a + bw, yt0);
		//top graph class label
		pDC->MoveTo(a, yt0 + 1);
		pDC->LineTo(a, yt0 + y / 25);
		pDC->TextOut(a, yt0 + y / 25, tmp);
		a = xc0 + bw / 2 + i * 2 * bw;
		pDC->Rectangle(a, yc0 - b, a + bw, yc0);
		a += bw / 2;
		pDC->MoveTo(a, yc0 + 1);
		pDC->LineTo(a, yc0 + y / 25);
		pDC->TextOut(a, yc0 + y / 25, tmp);
		//multi fire days
		pDC->SelectObject(&mfdBrush);
		a = xt0 + bw / 2 + i * 5 * bw + 3 * bw;
		if(pDoc->nmfd > 0)
			b = (int)(((double)pDoc->cmfd[i]) / ((double)pDoc->nmfd) * (double) y);
		else
			b = 0;
		pDC->Rectangle(a, yt0 - b, a + bw, yt0);
		a = xd0 + bw / 2 + i * 2 * bw;
		pDC->Rectangle(a, yd0 - b, a + bw, yd0);
		a += bw / 2;
		pDC->MoveTo(a, yd0 + 1);
		pDC->LineTo(a, yd0 + y / 25);
		pDC->TextOut(a, yd0 + y / 25, tmp);
	}
	//label small graphs
	pDC->SetTextAlign(TA_LEFT | TA_TOP);
	tmp.Format("All Days (%ld)", pDoc->fires->nv);
	pDC->TextOut(xa0 + 1, ya1 + 1, tmp);
	tmp.Format("Fire-Days (%ld)", pDoc->fires->nfd);
	pDC->TextOut(xb0 + 1, yb1 + 1, tmp);
	d = pDC->GetTextExtent(tmp).cy;
	if(pDoc->fires->cause == 0)
		tmp.Format("Cause = All");
	else if(pDoc->fires->cause == 1)
		tmp.Format("Cause = Lightning");
	else
		tmp.Format("Cause = Human");
	pDC->TextOut(xb0 + 1, yb1 + d, tmp);
	tmp.Format("Large Fire-Days (%ld)", pDoc->fires->nlfd);
	pDC->TextOut(xc0 + 1, yc1 + 1, tmp);
	tmp.Format("%d Acres", pDoc->fires->lfAcres);
	pDC->TextOut(xc0 + 1, yc1 + d, tmp);
	tmp.Format("Multi Fire-Days (%ld)", pDoc->fires->nmfd);
	pDC->TextOut(xd0 + 1, yd1 + 1, tmp);
	tmp.Format("%d Fires", pDoc->fires->mfdFires);
	pDC->TextOut(xd0 + 1, yd1 + d, tmp);
	//output info text
	pDC->SetTextAlign(TA_CENTER | TA_TOP);
	a = rect.left + rect.Width() / 6;
	b = yd0 + y / 5;
	tmp.Format("%s", pDoc->fires->staStr);
	pDC->TextOut(a, b, tmp);
	CSize ext = pDC->GetTextExtent(tmp);
	b += ext.cy + 1;
	if(pDoc->fires->graphModelString.GetLength() > 0)
	{	//output model code in lower right corner
		tmp.Format("%s", pDoc->fires->graphModelString);
		pDC->TextOut(a, b, tmp);
		b += ext.cy + 1;
	}
	tmp.Format("%d/%d - %d/%d",
			pDoc->fires->start.GetMonth(),
			pDoc->fires->start.GetDay(),
			pDoc->fires->end.GetMonth(),
			pDoc->fires->end.GetDay());
	pDC->TextOut(a, b, tmp);
	b += ext.cy + 1;
	tmp.Format("%d - %d",
			pDoc->fires->start.GetYear(),
			pDoc->fires->end.GetYear());
	pDC->TextOut(a, b, tmp);
	a += rect.Width() / 4;
	b = yd0 + y / 5;
	tmp.Format("Class %s Ranges", pDoc->fires->shortName);
	ext = pDC->GetTextExtent(tmp);
	pDC->TextOut(a, b, tmp);
	//b += ext.cy + 1;
	double maxVal = 1000.0;
	if(pDoc->fires->nv > 0)
		maxVal = pDoc->fires->allVals[pDoc->fires->nv - 1];
	if(pDoc->goUp)
		maxVal = 0.0;
	for(i = 0; i < pDoc->nDP; i++)
	{
		if(i == 3 || i == 6)
		{
			b = yd0 + y / 5;
			a += ext.cx;
		}
		b += ext.cy + 1;
		tmp.Format("%d %5.1f - %5.1f", i + 1, pDoc->lowers[i],
			(i < pDoc->nDP - 1) ? pDoc->lowers[i + 1] : maxVal);
		pDC->TextOut(a, b, tmp);
	}	
	CString dStr = pDoc->fires->dateStamp.Format("%m/%d/%Y-%H:%M");
	//tmp.Format("FF+%s %s", verStr, dStr);
	tmp.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	//tmp = pDoc->fires->dateStamp.Format("FF+3.0 %m/%d/%Y-%H:%M");
	pDC->SetTextAlign(TA_RIGHT | TA_BASELINE);
	ext = pDC->GetTextExtent(tmp);
	pDC->TextOut(rect.right - 2, rect.bottom - ext.cy / 2, tmp);
	//restore gdi  objects
	pDC->SelectObject( pOldPen );    
    pDC->SelectObject( pOldFont );    
	pDC->SelectObject( pOldBrush );    
}


BOOL CDPFormView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return DoPreparePrinting(pInfo);
}

void CDPFormView::PrintGraph()
{
	theApp.SetLandscape();
	CView::OnFilePrint();
}

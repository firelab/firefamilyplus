// WindRoseView.cpp : implementation file
//
 
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "wxSet.h"
#include "WindRoseDoc.h"
#include "WindRoseView.h"
#include <math.h>
#include <direct.h>
#include<shlwapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define PI 3.141592654

/////////////////////////////////////////////////////////////////////////////
// CWindRoseView
extern CFireplusApp theApp;

IMPLEMENT_DYNCREATE(CWindRoseView, CScrollView)

CWindRoseView::CWindRoseView()
{
}

CWindRoseView::~CWindRoseView()
{
}


BEGIN_MESSAGE_MAP(CWindRoseView, CScrollView)
	//{{AFX_MSG_MAP(CWindRoseView)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindRoseView drawing

void CWindRoseView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	
	CWindRoseDoc* pDoc = (CWindRoseDoc*)GetDocument();
    
	CWnd *parent = GetParent();
    parent->SetWindowPos(&wndTop,5,5,520,490,SWP_SHOWWINDOW);
	
    
     CSize sizeTotal;
	
	sizeTotal.cx = 450; // set to the minimum graph size
	sizeTotal.cy = 450; 

	SetScrollSizes(MM_TEXT, sizeTotal);
    //SetScaleToFitSize(sizeTotal);
}

void CWindRoseView::OnDraw(CDC* pDC)
{
	CRect rect;
    GetClientRect(&rect);

	if(!pDC->IsPrinting())
	{
	//rect.left = 0;
	//	rect.top = 0;
	//	rect.bottom = 640;
	//	rect.right = 640;
		
		rect.NormalizeRect();
	}
	else
	{
		pDC->GetBoundsRect(&rect, DCB_RESET);
		//rect.NormalizeRect();
	}
//	if(rect.Width() == 0 || rect.Height() == 0)
//		return;
	DrawToDCRegion(pDC, rect);
}

void CWindRoseView::DrawToDCRegion(CDC * pDC, CRect rect)
{

	
	CWindRoseDoc* pDoc = (CWindRoseDoc*)GetDocument();
	
	bool isPrint = false;

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
	
	if ((rect.bottom - rect.top) > 1600)
		isPrint = true;
   
	int factor = 1;
	if (isPrint){
		// kludges for printer font size etc.
		factor = 3;
        if  ((rect.bottom - rect.top) > 3000)
			factor = (rect.bottom - rect.top) / 1000;
		if (factor > 5)
			factor = 5;

	}
	
	CPen blackPen, 
		 greyPen,
		 *pOldPen, 
		 yellowPen, 
		 redPen,
		 orangePen,
		 redPen2, 
		 bluePen,
		 bluePen2, 
		 greenPen, 
		 purplePen,
		 darkPen;

	
	   if(!blackPen.CreatePen( PS_SOLID , factor, RGB(0,0,0))	
		  || !greyPen.CreatePen( PS_SOLID, factor, RGB(0xA0,0xA0,0xA0)) 
          || !yellowPen.CreatePen( PS_SOLID , factor, RGB(0xC0,0xC0,0))
		  || !redPen.CreatePen( PS_SOLID , factor, RGB(0xCC,0,0))
		  || !redPen2.CreatePen( PS_SOLID , factor, RGB(0x60,0,0))
		  || !orangePen.CreatePen( PS_SOLID , factor, RGB(0xA0,0xA0,0))
		  || !bluePen.CreatePen( PS_SOLID , factor, RGB(0,0,0xCC))
		  || !bluePen2.CreatePen( PS_SOLID , factor, RGB(0,0,0x60))
		  || !greenPen.CreatePen( PS_SOLID , factor, RGB(0,0xCC,0))
	      || !purplePen.CreatePen( PS_SOLID , factor, RGB(0x66,0x66,0)) 
		  || !darkPen.CreatePen( PS_SOLID , factor, RGB(0x20,0x20,0x20)))

	     {
	   	  CString strMsg;
		  strMsg.Format(
			_T("ERROR: Resources too low for creating pens.\n\n"));
		  AfxMessageBox(strMsg);
		  return;       
	   }
	

	// establish origin
	int centerX = 0, centerY = 0;
	//centerX = 300;
	//centerX = (rect.right-rect.left)/2;
  //  centerY = (rect.bottom - rect.top)/2; 
	//centerY = 300;
	// establish rose radius
	int totRadius = 250;
	if (isPrint){
		// if printing in portrait mode, use width as max radius
         if ((rect.bottom - rect.top) > (rect.right - rect.left))
			  totRadius = (rect.right - rect.left - 400) / 2;
		 else
              totRadius = (rect.bottom-rect.top - 400)/2;
	} else
       totRadius = (rect.bottom-rect.top-100)/2;

	if (totRadius < 150)
		totRadius = 150;    

    // hi-res printer kludge
	if (factor > 3)
		totRadius = totRadius - 500;


    centerX = totRadius + 50 * factor; 
	centerY = totRadius + 50 * factor;

	CFont normalFont,
		  largeFont,
		  smallFont;

	
      if(!smallFont.CreateFont(15 * factor, 6 * factor, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		   OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")  ||
		   !normalFont.CreateFont(20 * factor, 8 * factor, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		   OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")  ||
		   !largeFont.CreateFont(25 * factor, 10 * factor, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, 
		   OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, VARIABLE_PITCH, "helvetica")  
		   )
	   {
		   CString strMsg;
		   strMsg.Format(
		   	_T("ERROR: Unable to create font.\n\n"));
		   AfxMessageBox(strMsg);
		   return;       
	   }

	



	CBrush whiteBrush, blackBrush;
	if(!whiteBrush.CreateSolidBrush(0x00FFFFFF) || !blackBrush.CreateSolidBrush(0x00000000))
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Unable to create brushes.\n\n"));
		AfxMessageBox(strMsg);
		return;       
	}
	CBrush *pOldBrush = pDC->SelectObject(&whiteBrush);
	pOldPen = pDC->SelectObject(&blackPen);

	

	
	
	CFont *pOldFont = pDC->SelectObject(&largeFont);

	
	// draw outer rect
	//modified by Stu 7/15/2006, scrolling will draw line across windrose
	//if(isPrint)
		//pDC->Rectangle(rect);
	pDC->FillSolidRect(rect, RGB(255, 255, 255));

	pDC->SetTextColor(0x00000000);

	pDC->SetTextAlign(TA_BASELINE | TA_RIGHT);
	
    int calmRadius = 25 * factor;
  
   
	int radius = totRadius - calmRadius;  // working radius
    CString temp;

	temp.Format("%s",pDoc->hourStr);

	double maxPerc = pDoc->maxPerc;



	//label cardinal directions
	
	pDC->SetTextAlign(TA_BASELINE | TA_CENTER);
	pDC->SetTextColor(0x00000000);
	pDC->SetBkMode(TRANSPARENT);
	CString tStr;
	pDC->TextOut(centerX, 35 * factor, "N");
	pDC->TextOut(25 * factor, centerY + 5 * factor, "W");
	pDC->TextOut(centerX, centerY + totRadius + 30 * factor,"S");
	pDC->TextOut(centerX+totRadius + 15 * factor,centerY + 5 * factor,"E");

    // label other directions
	pDC->SelectObject(&normalFont);
	for (int i=2;i<WRDIRS;i+=4){
	  double angle = ((double) (i)/ (double) (WRDIRS)) * PI * 2.0;
      int startX = (int) (sin(angle) * totRadius * 1.05 + centerX);
      int startY = (int) (cos(angle) * totRadius * 1.05 + centerY);
	

      pDC->MoveTo(startX,startY);
	  switch (i){
	     case 2: pDC->TextOut(startX + 3 * factor,startY + 12 * factor,"SE");break;
		 case 6: pDC->TextOut(startX  + 3 * factor ,startY ,"NE");break;
		 case 10: pDC->TextOut(startX - 6 * factor,startY,"NW");break;
		 case 14: pDC->TextOut(startX - 6 * factor,startY + 12 * factor,"SW");break;
	  }
	}

	pDC->SelectObject(&smallFont);
	for (int i=1;i<WRDIRS;i+=2){
	  double angle = ((double) (i)/ (double) (WRDIRS)) * PI * 2.0;
      int startX = (int) (sin(angle) * totRadius * 1.05 + centerX);
      int startY = (int) (cos(angle) * totRadius * 1.05 + centerY);
	

      pDC->MoveTo(startX,startY);
	  switch (i){
	     case 1: pDC->TextOut(startX + 3 * factor,startY + 9 * factor,"SSE");break;
		 case 3: pDC->TextOut(startX + 6 * factor,startY + 12 * factor,"ESE");break;
		 case 5: pDC->TextOut(startX  + 6 * factor,startY ,"ENE");break;
		 case 7: pDC->TextOut(startX + 3 * factor,startY - 3 * factor,"NNE");break;
		 case 9: pDC->TextOut(startX - 6 * factor,startY,"NNW");break;
		 case 11: pDC->TextOut(startX - 9 * factor,startY,"WNW");break;
		 case 13: pDC->TextOut(startX - 9 * factor,startY + 9 * factor,"WSW");break;
		 case 15: // SSW label gets in the way of the text on small graphs
			 if (totRadius > 250)
				 pDC->TextOut(startX - 6 * factor ,startY + 12 * factor,"SSW");
			 break;
	  }
	}
     // draw outer bound
	pDC->Ellipse(centerX-totRadius,centerY-totRadius,centerX+totRadius,centerY+totRadius);
   

	// now figure how many concentric circles we need
	// based on the maximum percentage
	//  circles * circleSize = whole number slightly larger than maxPerc
	int circles = 0;
	double circleSize = 0;
	if (maxPerc <= 6.0){
         circles = 6;
		 circleSize = 1.0;
	} else if (maxPerc <= 8.0){
		 circles = 4;
		 circleSize = 2.0;
	} else if (maxPerc <= 10.0){
		 circles = 5;
		 circleSize = 2.0;
	} else if (maxPerc <= 12.0){
         circles = 6;
		 circleSize = 2.0;
	} else if (maxPerc <= 15.0){
		circles = 5;
		circleSize = 3.0;
	} else if (maxPerc <= 18.0){
		circles = 6;
		circleSize = 3.0;
	} else if (maxPerc <= 20.0){
		circles = 5;
		circleSize = 4.0;
	} else if (maxPerc <= 25.0){
		circles = 5;
		circleSize = 5.0;
	} else if (maxPerc <= 30.0){
		circles = 6;
		circleSize = 5.0;
	} else if (maxPerc <= 36.0){
		circles = 6;
		circleSize = 6.0;
	} else if (maxPerc <= 50.0){
		circles = 5;
		circleSize = 10.0;
	} else {
		circles = 10;
		circleSize = 10.0;
	}
    // now draw inner circles
    pDC->SelectObject(&greyPen);

	for (int i=(circles-1);i>=0;i--){
	   int currRadius = calmRadius + (int) ((double) (i) * (radius/circles));
       pDC->Ellipse(centerX-currRadius,centerY-currRadius,centerX+currRadius,centerY+currRadius);
	}
    pDC->SelectObject(&blackPen);
    // draw calm circle
	pDC->Ellipse(centerX-calmRadius,centerY-calmRadius,centerX+calmRadius,centerY+calmRadius);
    // draw outer hash marks
	for (int i=0;i<WRDIRS;i++){
	  double angle = ((double) (i)/ (double) (WRDIRS)) * PI * 2.0;
      int startX = (int) (sin(angle) * totRadius * 0.98 + centerX);
      int startY = (int) (cos(angle) * totRadius * 0.98 + centerY);
      int endX = (int) (sin(angle) * totRadius * 1.02 + centerX);
      int endY = (int) (cos(angle) * totRadius * 1.02 + centerY);
      pDC->MoveTo(startX,startY);
	  pDC->LineTo(endX,endY);

	}
    // label inner percentage circles
	pDC->SelectObject(&smallFont);
	for (int i=1; i <= circles;i++){
		int currRadius = calmRadius + (int) ((double) (i) * (radius/circles));
		CString strOut;
		strOut.Format("%2d%%", (int) (circleSize) * i);
		pDC->TextOut(centerX + 15 * factor, centerY - currRadius - 2 * factor - i/3, strOut);
	}

	// now create "rose petals"
	// loop through directions and speeds
	
	int lastX[WRDIRS],
	    lastY[WRDIRS];
	  
	   int dX1 = 0,
		   dX2 = 0,
		   dY1 = 0,
		   dY2 = 0;
    double totPercs = 0.0;

	for (int i=0;i<WRDIRS;i++){
		lastX[i] = 0;
		lastY[i] = 0;
		
		for (int j=1;j<WRSPEEDS;j++){
             // select a color 
		
			switch(j){
				case 1: pDC->SelectObject(&redPen); break;
				case 2: pDC->SelectObject(&redPen2); break;
				case 3: pDC->SelectObject(&orangePen); break;
				case 4: pDC->SelectObject(&yellowPen); break;
				case 5: pDC->SelectObject(&greenPen); break;
				case 6: pDC->SelectObject(&bluePen); break;
				case 7: pDC->SelectObject(&bluePen2); break;
				case 8: pDC->SelectObject(&purplePen); break;
				default: pDC->SelectObject(&darkPen); break;
			}
			if (pDoc->percs[i][j] > 0){
			   double angle = ((double) (i)/ (double) (WRDIRS)) * PI * 2.0 - PI/2; 

                            

			   // coordinates for 4 corners of 'rectangle'
               int startX1,startY1,
				   startX2,startY2,
				   endX1,endY1,
				   endX2,endY2;


			   // coordinates for "baseline" which bisects the rectangle
			   int baseX=0, baseY = 0,
				   endX=0,endY=0;

			

			   if (lastX[i])
			      baseX = lastX[i];
			   else
			      baseX= (int) (cos(angle) * (calmRadius) + centerX);
    
			   if (lastY[i])
			      baseY = lastY[i];
			   else
			      baseY= (int) (sin(angle) * (calmRadius) + centerY);
			 
			   // find vectors to corners of 'rectangle' from start & endpoints
               double widthFactor = 1.0 * (double) (factor);
			 

               dX1 = (int) (Round((float) (0.0 -  (sin(angle) * (widthFactor + j ))),0));
			   dY1 = (int) (Round((float) ( cos(angle) * (widthFactor + j  )),0));

			   dX2 = (int) (Round((float) ( sin(angle) * (widthFactor + j  )),0));
			   dY2 = (int) (Round((float) (0.0 -  (cos(angle) *  (widthFactor + j  ))),0));

               endX = baseX + (int) (cos(angle) * (pDoc->percs[i][j] / circleSize * (radius / circles)) );
               endY = baseY + (int) (sin(angle) * (pDoc->percs[i][j] / circleSize * (radius / circles)) );
			 
               startX1 =  (baseX + dX1);
			   startY1 =  (baseY + dY1);

			   startX2 =  (baseX + dX2);
			   startY2 =  (baseY + dY2);

               endX1 =  (endX + dX1);
			   endY1 = (endY + dY1);

			   endX2 =  (endX + dX2);
			   endY2 =  (endY + dY2);

			   lastX[i] = endX;
               lastY[i] = endY;
              
                totPercs += pDoc->percs[i][j];

		        // now draw 'rectangle'
               pDC->MoveTo(startX1,startY1);
	           pDC->LineTo(endX1,endY1);
			   pDC->LineTo(endX2,endY2);
			   pDC->LineTo(startX2,startY2);
			   pDC->LineTo(startX1,startY1);

			   // baselines
			   //pDC->MoveTo(baseX,baseY);
			   //pDC->LineTo(endX,endY);


			}
		}
	}
 
   // label inner percentage circles
	pDC->SelectObject(&normalFont);
	int rectBaseX = centerX + totRadius + 15,
		rectBaseY = centerY + totRadius - 78 * factor;

	pDC->TextOut(rectBaseX + 42 * factor,rectBaseY-15 * factor,"MPH");

	double dX = 0.5;

	for (int i=1; i <WRSPEEDS;i++){
		switch(i){
			case 1: pDC->SelectObject(&redPen); break;
			case 2: pDC->SelectObject(&redPen2); break;
			case 3: pDC->SelectObject(&orangePen); break;
			case 4: pDC->SelectObject(&yellowPen); break;
			case 5: pDC->SelectObject(&greenPen); break;
			case 6: pDC->SelectObject(&bluePen); break;
			case 7: pDC->SelectObject(&bluePen2); break;
			case 8: pDC->SelectObject(&purplePen); break;
			default: pDC->SelectObject(&darkPen); break;
		}
        dX += 1.0 * (double) (factor);
		
        pDC->MoveTo((int) (rectBaseX+dX),rectBaseY);
	    pDC->LineTo((int) (rectBaseX+dX),rectBaseY- 15 * factor);
		pDC->LineTo((int) (rectBaseX-dX),rectBaseY- 15 * factor);
		pDC->LineTo((int) (rectBaseX-dX),rectBaseY);
		pDC->LineTo((int) (rectBaseX+dX),rectBaseY);
        
		pDC->SelectObject(&smallFont);
		CString strOut;
		switch(i){
			case 1: strOut.Format("  1 -  4"); break;
			case 2: strOut.Format("  4 -  8"); break;
			case 3: strOut.Format("  8 - 13"); break;
			case 4: strOut.Format(" 13 - 19"); break;
			case 5: strOut.Format(" 19 - 25"); break;
			case 6: strOut.Format(" 25 - 32"); break;
			case 7: strOut.Format(" 32 - 39"); break;
			case 8: strOut.Format(" 39 - 47"); break;
			case 9: strOut.Format("  47 +"); break;
			
		}
		
		pDC->TextOut(rectBaseX + 40 * factor, rectBaseY - factor, strOut);

		rectBaseY += 15 * factor;
		
	}


	// label inner calm circle
    pDC->SelectObject(&blackPen);
	double calmPerc = 100.0 - totPercs;
	pDC->TextOut(centerX,centerY-3,"calm");

	CString strCalm;
	strCalm.Format("%3.1f%%",calmPerc);
	pDC->TextOut(centerX ,centerY + 12 * factor,strCalm);
    pDC->SetTextAlign(TA_BASELINE | TA_LEFT);
    
	
	// general labels
    pDC->TextOut(10 * factor,15 * factor,pDoc->staStr);
	CString strObs;
	pDC->TextOut(10 * factor,32 * factor ,pDoc->hourStr);
	if (pDoc->extraStr && pDoc->extraStr.GetLength() > 0)
		pDC->TextOut(10 * factor, 49 * factor, pDoc->extraStr);

	//strObs.Format("Observations: %d (%d%%)", pDoc->recs, (int) ((float)(pDoc->recs)/(float) (pDoc->days*24)* (float) 100));
	// new 2014: if number of observations exceeds the number of hours, assume that both gusts and average speeds
	// are used (and thus there are double the number of possible observations)
	if (pDoc->recs <= pDoc->days * 24)
	   strObs.Format("Observations: %d (%d%%)", pDoc->recs, 
		   (pDoc->days > 0) ? (int) ((float)(pDoc->recs)/(float) (pDoc->days*24)* (float) 100) : 0);
	else
	   strObs.Format("Observations: %d (%d%%)", pDoc->recs, 
		   (pDoc->days > 0) ? (int) ((float)(pDoc->recs)/(float) (pDoc->days*24 * 2)* (float) 100) : 0);

	pDC->TextOut(10 * factor,centerY + totRadius + 8 * factor, strObs);

    CString strDays;
	//strDays.Format("Days: %d of %d (%d%%)", pDoc->days16, pDoc->days, (int) ((float) (pDoc->days16)/(float) (pDoc->days) * 100));
	strDays.Format("Days: %d of %d (%d%%)", pDoc->days16, pDoc->days, 
		(pDoc->days > 0) ? (int) ((float) (pDoc->days16)/(float) (pDoc->days) * 100) : 0);
	pDC->TextOut(10 * factor,centerY + totRadius - 9 * factor, strDays);

	pDC->TextOut(10 * factor,centerY + totRadius + 25 * factor,pDoc->dateStr);
	pDC->TextOut(10 * factor,centerY + totRadius + 42 * factor,pDoc->perStr);
	

	//restore drawing tools to original state
    pDC->SelectObject( pOldPen );    
    pDC->SelectObject( pOldFont );    
    pDC->SelectObject( pOldBrush );    
}




float CWindRoseView::Round(const float &number, const int num_digits)
{
    float doComplete5i, doComplete5(number * powf(10.0f, (float) (num_digits + 1)));
    
    if(number < 0.0f)
        doComplete5 -= 5.0f;
    else
        doComplete5 += 5.0f;
    
    doComplete5 /= 10.0f;
    modff(doComplete5, &doComplete5i);
    
    return doComplete5i / powf(10.0f, (float) num_digits);
}
/////////////////////////////////////////////////////////////////////////////
// CWindRoseView diagnostics

#ifdef _DEBUG
void CWindRoseView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CWindRoseView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG



/*void CWindRoseView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CScrollView::OnPrint(pDC, pInfo);
}*/

/*void CWindRoseView::OnFilePrint() 
{
	// TODO: Add your command handler code here
	PRINTDLG pd;
	theApp.GetPrinterDeviceDefaults(&pd);
	CPrintDialog pdlg(false);
	pdlg.m_pd.hDevNames = pd.hDevNames;
	pdlg.m_pd.hDevMode = pd.hDevMode;
	if(pdlg.DoModal() == IDOK)
	{
		//theApp.GetPrinterDeviceDefaults(&pd);
		//theApp.SelectPrinter(NULL, NULL);
		//theApp.SelectPrinter(pdlg.m_pd.hDevNames, pdlg.m_pd.hDevMode);
		CDC pDC;
		theApp.CreatePrinterDC(pDC);
		if(pDC)
		{
			DOCINFO docinfo;
			docinfo.cbSize = sizeof(DOCINFO);
			docinfo.lpszDocName = "FireFamily Plus Pocket Card";
			docinfo.lpszOutput = NULL;
			pDC.StartDoc(&docinfo);
			pDC.StartPage();
			int lx, ly;
			ly = pDC.GetDeviceCaps(VERTRES);
			lx = pDC.GetDeviceCaps(HORZRES);
			CRect bnd(1, 1,
				lx, ly);
			DrawToDCRegion(&pDC, bnd);
			pDC.EndPage();
			pDC.EndDoc();
		}
		HDC tDC = pDC.Detach();
		DeleteDC(tDC);
	}
	
}*/



void CWindRoseView::OnFileSaveAs() 
{
	chdir(theApp.userDir);
	CFileDialog fd(false, theApp.GetGraphicsExt(), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		theApp.GetGraphicsFileStr(), this);
	//int saveWidth = 640, saveHeight = 480;
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
		SelectObject(memhdc.m_hDC, Old);
		char fName[256];
		strcpy(fName, fd.GetPathName());
		SaveBitmapFile(memhdc.m_hDC, picture, fName); 
	//	SelectObject(memhdc.m_hDC, Old);
		ReleaseDC(whdc);
		memhdc.DeleteDC();
		DeleteObject(picture);
		theApp.SetUserDir(fd.GetPathName());
		theApp.SetGraphicsExt(fd.GetFileExt());
	}
	chdir(theApp.dbDir);
}

/*BOOL CWindRoseView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	DoPreparePrinting(pInfo);
	return CScrollView::OnPreparePrinting(pInfo);
}*/

void CWindRoseView::OnFilePrint() 
{
	// TODO: Add your command handler code here
	//CPrintInfo printInfo;
	//OnPreparePrinting(&printInfo);
	CView::OnFilePrint();
}

BOOL CWindRoseView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	return DoPreparePrinting(pInfo);
	
	//return CScrollView::OnPreparePrinting(pInfo);
}

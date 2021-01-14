// RichCtrlView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "RichCtrlView.h"
#include ".\richctrlview.h"

extern CFireplusApp theApp;

// CRichCtrlView

IMPLEMENT_DYNCREATE(CRichCtrlView, CFormView)

CRichCtrlView::CRichCtrlView()
: CFormView(IDD_RICHCTRLVIEW)
{
	isTemp = true;
}

CRichCtrlView::~CRichCtrlView()
{
	if(isTemp)
		unlink(m_fileName);
	m_strText = m_fileName = "";
}

void CRichCtrlView::DoDataExchange(CDataExchange* pDX)
{
	CView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_richCtrl);
	DDX_Text(pDX, IDC_RICHEDIT21, m_strText);
}

   void CRichCtrlView::OnEditCopy()
   {
      m_richCtrl.Copy();
   }

   void CRichCtrlView::OnEditCut()
   {
      m_richCtrl.Cut();
   }

   void CRichCtrlView::OnEditPaste()
   {
      m_richCtrl.Paste();
   }

BEGIN_MESSAGE_MAP(CRichCtrlView, CView)
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)

END_MESSAGE_MAP()

// CRichCtrlView diagnostics

#ifdef _DEBUG
void CRichCtrlView::AssertValid() const
{
	CView::AssertValid();
}

void CRichCtrlView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

// CRichCtrlView message handlers

void CRichCtrlView::LoadFromFile(CString _fileName)
{
	m_fileName = _fileName;
	if(m_fileName.GetLength() > 0)
	{
		FILE *stream = fopen(m_fileName, "rt");
		char buf[256];

		while(fgets(buf, 255, stream) != 0)
			m_strText += _T(buf);
		fclose(stream);
		UpdateData(FALSE);
	}
	CHARFORMAT cf;
	CString strDefFont = "courier new";
	//VERIFY(strDefFont.LoadString(nFontNameID));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_STRIKEOUT|CFM_SIZE|
		CFM_COLOR|CFM_OFFSET|CFM_PROTECTED;
	cf.dwEffects = CFE_AUTOCOLOR;
	cf.yHeight = 160;//200; //10pt
	cf.yOffset = 0;
	cf.crTextColor = RGB(0, 0, 0);
	cf.bCharSet = 0;
	cf.bPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	ASSERT(strDefFont.GetLength() < LF_FACESIZE);
	//lstrcpynA(cf.szFaceName, T2A((LPTSTR) (LPCTSTR) strDefFont), LF_FACESIZE);
	strncpy(cf.szFaceName, strDefFont, LF_FACESIZE);
	cf.dwMask |= CFM_FACE;

	m_richCtrl.SetSel(0,-1);
	m_richCtrl.SetDefaultCharFormat(cf);
	m_richCtrl.SetSelectionCharFormat(cf);
	m_richCtrl.SetSel(0,0);
	m_richCtrl.SetOptions(ECOOP_OR, ECO_SAVESEL);
}

void CRichCtrlView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CRect rect;
	GetClientRect(&rect);
	m_richCtrl.MoveWindow(&rect);
	SIZE sz;
	sz.cx = rect.Width();
	sz.cy = rect.Height();
	SetScaleToFitSize(sz);
}

void CRichCtrlView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if(m_richCtrl.m_hWnd)
	{
		m_richCtrl.MoveWindow(0, 0, cx, cy);
		SIZE sz;
		sz.cx = cx;
		sz.cy = cy;
		SetScaleToFitSize(sz);
	}
}

void CRichCtrlView::SaveToFile(CString _fileName)
{
	FILE * in, * out;
	in = fopen(this->m_fileName, "rt");
	out = fopen(_fileName, "wt");
	if(!in)
	{
		AfxMessageBox("Error opening input file");
		fclose(out);
		return;
	}
	if(!out)
	{
		AfxMessageBox("Error opening target file");
		fclose(in);
		return;
	}
	while(!feof(in))
	{
		int c = fgetc(in);
		fputc(c, out);
	}
	fclose(in);
	fclose(out);
}

void CRichCtrlView::Print()
{
    CDC dc;
	CPrintDialog pd(FALSE);
	if(pd.DoModal() == IDCANCEL)
		return;
	dc.Attach(pd.GetPrinterDC());         // Attach a printer DC
    dc.m_bPrinting = TRUE;
    CString strTitle;                           // Get the application title
    strTitle.LoadString(AFX_IDS_APP_TITLE);
 	FORMATRANGE fr;
	int	      nHorizRes = dc.GetDeviceCaps(HORZRES),
		      nVertRes = dc.GetDeviceCaps(VERTRES),
			  nLogPixelsX = dc.GetDeviceCaps(LOGPIXELSX),
			  nLogPixelsY = dc.GetDeviceCaps(LOGPIXELSY);
	LONG	      lTextLength;   // Length of document.
	LONG	      lTextPrinted;  // Amount of document printed.

	// Ensure the printer DC is in MM_TEXT mode.
	dc.SetMapMode(MM_TEXT);

	// Rendering to the same DC we are measuring.
	ZeroMemory(&fr, sizeof(fr));
	fr.hdc = fr.hdcTarget = dc.GetSafeHdc();

	// Set up the page.
	fr.rcPage.left     = fr.rcPage.top = 0;
	fr.rcPage.right    = (nHorizRes/nLogPixelsX) * 1440;
	fr.rcPage.bottom   = (nVertRes/nLogPixelsY) * 1440;

	// Set up 1" margins all around.
	fr.rc.left   = fr.rcPage.left + 1440;  // 1440 TWIPS = 1 inch.
	fr.rc.top    = fr.rcPage.top + 1440;
	fr.rc.right  = fr.rcPage.right - 1440;
	fr.rc.bottom = fr.rcPage.bottom - 1440;

	// Default the range of text to print as the entire document.
	fr.chrg.cpMin = 0;
	fr.chrg.cpMax = -1;

	// Set up the print job (standard printing stuff here).
	DOCINFO di;                                 // Initialise print document details
    ::ZeroMemory (&di, sizeof (DOCINFO));
	di.cbSize = sizeof (DOCINFO);
    di.lpszDocName = "FFP Import Error Log";
	// Do not print to file.
	di.lpszOutput = NULL;

	lTextLength = m_richCtrl.GetTextLength() - m_richCtrl.GetLineCount();
//	lTextLength = ::SendMessage ( editControl.m_hWnd, WM_GETTEXTLENGTH, 0, 0 );
    BOOL bPrintingOK = dc.StartDoc(&di);        // Begin a new print job

	do
	{
		// Start the page.
		dc.StartPage();//StartPage(hPrinterDC);

		// Print as much text as can fit on a page. The return value is
		// the index of the first character on the next page. Using TRUE
		// for the wParam parameter causes the text to be printed.

		//#ifdef USE_BANDING

		lTextPrinted = m_richCtrl.FormatRange(&fr, FALSE);
		m_richCtrl.DisplayBand(&fr.rc);
		/*SendMessage(/*hRTFWnd,
		EM_FORMATRANGE,
		FALSE,
		(LPARAM)&fr);
		SendMessage(hRTFWnd, EM_DISPLAYBAND, 0, (LPARAM)&fr.rc);*/

		// Print last page.
		dc.EndPage();//EndPage(hPrinterDC);

		// If there is more text to print, adjust the range of characters
		// to start printing at the first character of the next page.
		if (lTextPrinted < lTextLength)
		{
			fr.chrg.cpMin = lTextPrinted;
			fr.chrg.cpMax = -1;
		}
	}
	while (lTextPrinted < lTextLength);

	// Tell the control to release cached information.
	m_richCtrl.FormatRange(NULL, FALSE);

	//dc.EndDoc();//EndDoc (hPrinterDC);

	if (bPrintingOK)
        dc.EndDoc();                            // end a print job
	else
        dc.AbortDoc();                          // abort job.
    dc.Detach();                                // detach the printer DC
}
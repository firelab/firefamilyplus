// RichDialog.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "RichDialog.h"
#include "direct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRichDialog dialog


CRichDialog::CRichDialog(CWnd* pParent /*=NULL*/, CString _fileName)
	: CDialog(CRichDialog::IDD, pParent), fileName(_fileName)
{
	//{{AFX_DATA_INIT(CRichDialog)
	richString = _T("");
	//}}AFX_DATA_INIT
}


void CRichDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRichDialog)
	DDX_Control(pDX, IDC_RICHEDIT1, editControl);
	DDX_Text(pDX, IDC_RICHEDIT1, richString);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRichDialog, CDialog)
	//{{AFX_MSG_MAP(CRichDialog)
	ON_BN_CLICKED(IDC_PRINTBUTTON, OnPrintbutton)
	ON_BN_CLICKED(IDC_SAVEBUTTON, OnSavebutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichDialog message handlers

BOOL CRichDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if(fileName.GetLength() > 0)
	{
		FILE *stream = fopen(fileName, "rt");
		char buf[256];

		while(fgets(buf, 255, stream) != 0)
			richString += _T(buf);
		fclose(stream);
		UpdateData(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRichDialog::OnPrintbutton() 
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
	
	lTextLength = editControl.GetTextLength() - editControl.GetLineCount();
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
		
		lTextPrinted = editControl.FormatRange(&fr, FALSE);
		editControl.DisplayBand(&fr.rc);
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
	editControl.FormatRange(NULL, FALSE);
	
	//dc.EndDoc();//EndDoc (hPrinterDC);
	
	
	if (bPrintingOK)
        dc.EndDoc();                            // end a print job    
	else
        dc.AbortDoc();                          // abort job.
    dc.Detach();                                // detach the printer DC
}

void CRichDialog::OnSavebutton() 
{
	if(fileName.GetLength() > 0)
	{
		chdir(theApp.userDir);
		CFileDialog fd(FALSE, "txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", this);
		if(IDOK == fd.DoModal())
		{
			CWaitCursor wait;
			FILE *stream = fopen(fileName, "rt");
			FILE *out = fopen(fd.GetPathName(), "wt");
			int ch = fgetc(stream);
			while(!feof(stream))
			{
				fputc(ch, out);
				ch = fgetc(stream);
			}
			fclose(stream);
			fclose(out);
			theApp.SetUserDir(fd.GetPathName());
		}
		chdir(theApp.dbDir);
	}
}

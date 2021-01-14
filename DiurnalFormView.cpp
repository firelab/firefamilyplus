// DiurnalFormView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DiurnalFormView.h"
#include "DiurnalDoc.h"
#include "DiurnalFrame.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern char *varFormats[];
extern CFireplusApp theApp;

static DWORD CALLBACK MyStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
   CFile* pFile = (CFile*) dwCookie;

   *pcb = pFile->Read(pbBuff, cb);

   return 0;
}
static DWORD CALLBACK MyStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CFile* pFile = (CFile*) dwCookie;

	pFile->Write(pbBuff, cb);
	*pcb = cb;
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDiurnalFormView

IMPLEMENT_DYNCREATE(CDiurnalFormView, CFormView)

CDiurnalFormView::CDiurnalFormView()
	: CFormView(CDiurnalFormView::IDD)
{
	//{{AFX_DATA_INIT(CDiurnalFormView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	fName = NULL;
}

CDiurnalFormView::~CDiurnalFormView()
{
	if(fName)
	{
		unlink(fName);
		free(fName);
	}
}

void CDiurnalFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDiurnalFormView)
	DDX_Control(pDX, IDC_RICHEDIT1, richEditCtl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiurnalFormView, CFormView)
	//{{AFX_MSG_MAP(CDiurnalFormView)
	ON_WM_SIZE()
	ON_NOTIFY(EN_MSGFILTER, IDC_RICHEDIT1, OnMsgfilterRichedit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiurnalFormView diagnostics

#ifdef _DEBUG
void CDiurnalFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDiurnalFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDiurnalFormView message handlers
 
void CDiurnalFormView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	EnableScrollBarCtrl(SB_HORZ, FALSE);
	EnableScrollBarCtrl(SB_VERT, FALSE);
	//dump data to a file then stream it in
	char buf[512];
	fName = GetTempFileName();
	CFile tFile(fName, CFile::modeCreate | CFile::modeReadWrite);
	CDiurnalDoc* pDoc = (CDiurnalDoc *)GetDocument();
	CDiurnalFrame *frame = (CDiurnalFrame *)GetParent()->GetParent();
	//varID = frame->varID;
	//CString temp, header;
	strcpy(buf, "FireFamily Plus Diurnal Report\r\n");
	tFile.Write(buf, strlen(buf));
	sprintf(buf, "%s\r\n", frame->varName);
	//temp.Format("%s\r\nVariable: %s\r\n\r\n", pDoc->stationName,
	//		frame->varName);
	tFile.Write(buf, strlen(buf));
	if(OptionFromVarID(frame->varID) == 1 && pDoc->model.GetLength() > 0)
	//if(pDoc->model.GetLength() > 0)
		sprintf(buf, "Station: %s   Model:%s\r\n\r\n", pDoc->stationName, pDoc->model);
	else
		sprintf(buf, "Station: %s\r\n\r\n", pDoc->stationName);
	tFile.Write(buf, strlen(buf));
	sprintf(buf, " Time Frame: %d/%d - %d/%d\r\n",
			pDoc->start.GetMonth(),
			pDoc->start.GetDay(),
			pDoc->end.GetMonth(),
			pDoc->end.GetDay());
	tFile.Write(buf, strlen(buf));
	sprintf(buf, " Data Years: %d - %d\r\n",
			pDoc->start.GetYear(),
			pDoc->end.GetYear());
	tFile.Write(buf, strlen(buf));
	strcpy(buf, "\r\n");
	tFile.Write(buf, strlen(buf));
	if(frame->varID == 23)
		strcpy(buf, 
			"               Mode Values by Hour of Day by Date\r\n                           Hour\r\n");
	else
		strcpy(buf, 
			"                Avg Values by Hour of Day by Date\r\n                           Hour\r\n");
	tFile.Write(buf, strlen(buf));
	strcpy(buf, 
		"Date    0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16    17    18    19    20    21    22    23\r\n");
	tFile.Write(buf, strlen(buf));
	strcpy(buf, 
		"-----------------------------------------------------------------------------------------------------------------------------------------------------\r\n");
	tFile.Write(buf, strlen(buf));
	COleDateTime rptStart, rptEnd, rdate;
	rptStart.SetDate(2000, pDoc->start.GetMonth(), pDoc->start.GetDay());
	rptEnd.SetDate(2000, pDoc->end.GetMonth(), pDoc->end.GetDay());
	rdate = rptStart;
	COleDateTimeSpan span(1, 0, 0, 0);
	int end = 366;
	char frmt[16];
	if(rptStart.GetDayOfYear() < rptEnd.GetDayOfYear())
		end = rptEnd.GetDayOfYear();
	int i;
	for(i = rptStart.GetDayOfYear() - 1; i < end; i++)
	{
		sprintf(buf, "%2d/%2d", rdate.GetMonth(), rdate.GetDay());
		tFile.Write(buf, strlen(buf));
		for(int h = 0; h < 24; h++)
		{
			if(pDoc->da->days[frame->varID][i].hours[h].N() > 0)
			{
				sprintf(frmt, "%%%s ", varFormats[frame->varID]);
				sprintf(buf, frmt, (frame->varID != 23) ? pDoc->da->days[frame->varID][i].hours[h].Mean() :
					pDoc->da->days[frame->varID][i].hours[h].mode);
			}
			else
				sprintf(buf, "      ");
			tFile.Write(buf, strlen(buf));
		}
		strcpy(buf, "\r\n");
		tFile.Write(buf, strlen(buf));
		rdate += span;
	}
	if(rptStart.GetDayOfYear() >= rptEnd.GetDayOfYear())
	{
		rdate.SetDate(2000, 1, 1);
		end = rptEnd.GetDayOfYear();
		for(i = 0; i <= end; i++)
		{
			sprintf(buf, "%2d/%2d", rdate.GetMonth(), rdate.GetDay());
			tFile.Write(buf, strlen(buf));
			for(int h = 0; h < 24; h++)
			{
				if(pDoc->da->days[frame->varID][i].hours[h].N() > 0)
				{
					sprintf(frmt, "%%%s ", varFormats[frame->varID]);
					sprintf(buf, frmt, (frame->varID != 23) ? pDoc->da->days[frame->varID][i].hours[h].Mean() :
					pDoc->da->days[frame->varID][i].hours[h].mode);
				}
					//temp.Format("%6.2f", pDoc->da->days[frame->varID][i].hours[h].Mean());
				else
					strcpy(buf, "      ");
				tFile.Write(buf, strlen(buf));
			}
			strcpy(buf, "\r\n");
			tFile.Write(buf, strlen(buf));
			rdate += span;
		}
	}
	sprintf(buf, "\r\n%ld weather observations used.\r\n%s\r\n", pDoc->nWxObs, pDoc->stamp);
	tFile.Write(buf, strlen(buf));
	tFile.SeekToBegin();

	EDITSTREAM es;

	es.dwCookie = (DWORD) &tFile;
	es.pfnCallback = MyStreamInCallback; 
	richEditCtl.StreamIn(SF_TEXT, es);

	tFile.Close();
	richEditCtl.SetEventMask(richEditCtl.GetEventMask() | ENM_MOUSEEVENTS);
	CRect tRect;
	GetClientRect(&tRect);
	OnSize(SIZE_RESTORED, tRect.Width(), tRect.Height());
	richEditCtl.SetOptions(ECOOP_SET, ECO_SAVESEL);
	richEditCtl.SetSel(0,0);
}

void CDiurnalFormView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	CWaitCursor wait;
	if(richEditCtl.m_hWnd)
	{
	//	richEditCtl.MoveWindow(1,1,cx - 2, cy - 2);	
	//	richEditCtl.MoveWindow(0,0,cx - 1, cy - 1);	
		richEditCtl.SetWindowPos(&wndTop,0,0,cx - 1, cy - 1, SWP_SHOWWINDOW | SWP_NOZORDER);	
	}
	/*{
		CRect trect;
		printButton.GetWindowRect(&trect);
		int a = trect.Height() + 1, b = cy - 2 - trect.Height();
		if(b <= 0)
			b = 1;
		table.MoveWindow(1, a, cx - 2, b);
	}*/
}

void CDiurnalFormView::OnMsgfilterRichedit1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	// TODO: The control will not send this notification unless you override the
	// CFormView::OnInitDialog() function to send the EM_SETEVENTMASK message
	// to the control with either the ENM_KEYEVENTS or ENM_MOUSEEVENTS flag 
	// ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
//	if(pMsgFilter->msg == NM_DBLCLK)
	if(pMsgFilter->msg == WM_LBUTTONDBLCLK)
	{
		CDiurnalFrame *frame = (CDiurnalFrame *)GetParent()->GetParent();
		frame->MaximizePane(this);
	}

	*pResult = 0;
}

void CDiurnalFormView::PrintTheDamnThing()
{
	//m_rich is the CRichEditCtrl you are printing from 
	//m_NamePath is the name of the document (anything will work) 
	long iFormatted = 0; 
	FORMATRANGE FormatRange; //structure for defining a printer page 

	// Use a CPrintDialog to get the default printer information 
	CPrintDialog dlg(false);
	if(theApp.DoPrintDialog(&dlg) != IDOK)
		return;
	//CPrintDialog dlg(FALSE); 
	//dlg.m_pd.Flags |= PD_RETURNDEFAULT; 

	// Ask for defaults from CPrintDialog 
//	if (!dlg.GetDefaults()) 
//	{ 
//		AfxMessageBox("No printer is selected.\nPlease select a default printer."); 
//		return; 
//	} 
	// Create a CDC object for the device context we got 
	CDC dcPrinter; 
	dcPrinter.Attach(dlg.m_pd.hDC); 

	// Describe print job to print spooler 
	DOCINFO docInfo; 
	docInfo.cbSize = sizeof(docInfo); 
	docInfo.fwType = 0; 
	docInfo.lpszDocName = GetDocument()->GetTitle(); //Any string you like here for document name 
	docInfo.lpszOutput = NULL; 
	docInfo.lpszDatatype = NULL; 

	// Start a document 
	if (!dcPrinter.StartDoc(&docInfo)) 
	{ 
		AfxMessageBox("Couldn't print document."); 
		return; 
	} 

	int iOldMapMode = dcPrinter.GetMapMode(); //save old mode to restore 
	dcPrinter.SetMapMode(MM_TWIPS); //map to twips for rich edit control's measurement 

	// measure the printable page 
	int nOffsetX = dcPrinter.GetDeviceCaps(PHYSICALOFFSETX); 
	int nOffsetY = -dcPrinter.GetDeviceCaps(PHYSICALOFFSETY); 

	int nHorzRes = dcPrinter.GetDeviceCaps(HORZRES); 
	int nVertRes = -dcPrinter.GetDeviceCaps(VERTRES); 

	// create a rect for the whole printable page 
	CRect rect(nOffsetX, nOffsetY, nHorzRes, nVertRes); 
	//CRect rect(nOffsetX, nVertRes, nHorzRes, nOffsetY); 
	dcPrinter.DPtoLP(&rect); 
	dcPrinter.SetMapMode(iOldMapMode); //reset mapping mode or nothing prints... 

	// set up the formatting parameters for the rich edit control 
	FormatRange.hdc = dcPrinter.GetSafeHdc(); 
	FormatRange.hdcTarget = dcPrinter.GetSafeHdc(); 
	FormatRange.rc = rect; 
	FormatRange.rcPage = rect; 

	do //print the pages one at a time in this loop 
	{ 
		if (!dcPrinter.StartPage()) 
		{ 
			AfxMessageBox("Error starting new page!"); 
			break; 
		} 

		FormatRange.chrg.cpMin = iFormatted; //start from where last page ended 
		FormatRange.chrg.cpMax = -1; //try to format all remaining text 

		iFormatted = richEditCtl.FormatRange(&FormatRange, TRUE); //Create the image to be printed 
		richEditCtl.DisplayBand(&rect); //Print the page 

		dcPrinter.EndPage(); // close the page 
		if (iFormatted < 0) 
			break; //error with formatting 
		} 
	while(iFormatted < richEditCtl.GetTextLength()); //print until all text has been processed 

	richEditCtl.FormatRange(NULL, TRUE); //free cached information about target printer 
	// close the document. Printing actually starts now! 
	dcPrinter.EndDoc(); 

}

void CDiurnalFormView::SaveTheDamnThing()
{
	chdir(theApp.userDir);
	CFileDialog fd(FALSE, "txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", this);
	if(fd.DoModal() == IDOK)
	{	
		CWaitCursor wait;
		CFile tFile(fd.GetPathName(), CFile::modeCreate | CFile::modeWrite);
		EDITSTREAM es;
		es.dwCookie = (DWORD) &tFile;
		es.pfnCallback = MyStreamOutCallback; 
		richEditCtl.StreamOut(SF_TEXT, es);
		tFile.Close();
		theApp.SetUserDir(fd.GetPathName());
	}
	chdir(theApp.dbDir);//theApp.workDir);
}

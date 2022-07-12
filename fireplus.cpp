// fireplus.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "afxadv.h"
#include "fireplus.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "RichFrame.h"
#include "fireplusSet.h"
#include "fireplusDoc.h"
#include "fireplusView.h"
#include "ClimDoc.h"
#include "ClimateView.h"
#include "wxset.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "SplitterFrame.h"
#include "GraphView.h"
#include "float.h"
#include <afxrich.h>
#include "richView.h"
#include "richDoc.h"
#include "PocketCardDoc.h"
#include "PocketCardFrame.h"
#include "PocketCardView.h"
#include "FireSummary.h"
#include "FireSumDoc.h"
#include "FireSumWnd.h"
#include "FireSumView.h"
#include "SIGStationSet.h"
#include "Firestats.h"
#include "firestatdoc.h"
#include "richfiredoc.h"
#include "firestatframe.h"
//#include "firestatreport.h"
#include "cumulativebars.h"
#include "cumulativeframe.h"
//#include "PrecipGraphView.h"
#include "DPDoc.h"
#include "DPFrame.h"
#include "DPReportView.h"
#include "DiurnalDoc.h"
#include "DiurnalFrame.h"
//#include "DiurnalReportView.h"
#include "DiurnalFormView.h"
#include "MergedFrame.h"
#include "MergedView.h"
#include "BatchOutputDoc.h"
#include "BatchOutputFrame.h"
#include "BatchOutputTreeView.h"
#include "WindRoseDoc.h"
#include "WindRoseFrame.h"
#include "WindRoseView.h"
#include "richtermdoc.h"
#include "TermFrame.h"
#include "eventlocatordoc.h"
#include "WeatherDoc.h"
#include "WeatherView.h"
#include "WeatherFrame.h"
#include "FiresDoc.h"
#include "FiresView.h"
#include "FiresFrame.h"
#include "EventFrame.h"
#include "CandidatesDoc.h"
#include "CandidatesView.h"
#include "CandidatesFrame.h"
#include <iostream>
#include <windows.h>
#include <winspool.h>
#include "NFDRS2016WxDoc.h"
#include "NFDRS2016WxFrame.h"
#include "NFDRS2016WxView.h"

#include <fstream>



using namespace std;

#include <direct.h>
#include <atlimage.h>
#include <math.h>
//#include "HtmlEditCtrlEx.h"
#include "CAcroAXDocShim.h"
#include "explorer1.h"
#include "afxwin.h"
//#include "CDNFDRS4.h"
//#include "CDNFDRS4.h"
//#include "CDNFDRS4.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int ScreenPixelsToPrinterPixels(CDC *screenDC, CDC *printerDC, int screenPixels)
{
	int ret = screenPixels;
	double ratio = ((double)printerDC->GetDeviceCaps(LOGPIXELSY)) / ((double)screenDC->GetDeviceCaps(LOGPIXELSY));
	if(ratio > 1.0)
		ret = (int)(screenPixels * ratio);
	return ret;
}

STDAPI MyDaoStringAllocCallback(DWORD dwLen, DWORD pData, void** ppv)
{
	LPTSTR lpsz;
	CString* pstr = (CString*)pData;

	dwLen++;

	TRY
	{
		//Allocate twice the space needed so that DAO does not overwrite the buffer
		lpsz = pstr->GetBufferSetLength(2*dwLen/sizeof(TCHAR));
		*ppv = (void*)(dwLen > 0 ? lpsz : NULL);
	}
	CATCH_ALL(e)
	{
		e->Delete();
		return E_OUTOFMEMORY;
	}
	END_CATCH_ALL

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CFireplusApp

BEGIN_MESSAGE_MAP(CFireplusApp, CWinApp)
	//{{AFX_MSG_MAP(CFireplusApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//ON_COMMAND(ID_APP_NASF, OnAppNASF)
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpContents)
	ON_COMMAND(ID_HELP_SEARCH, OnHelpSearch)
	ON_COMMAND(ID_FILE_NEW_FFP, OnFileNewFfp)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireplusApp construction

CFireplusApp::CFireplusApp() : CWinApp()
, m_buildDateStr(_T(__DATE__))

{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	char yrBuild[5], moBuild[3], dayBuild[3], monthCh[4];
	COleDateTime dt;// (m_buildDateStr);
	dt.ParseDateTime(m_buildDateStr);
	if (dt.GetStatus() != COleDateTime::invalid)
	{
		m_buildDateStrPacked = dt.Format("%Y%m%d");
	}
	else
	{
		yrBuild[0] = m_buildDateStr[7];
		yrBuild[1] = m_buildDateStr[8];
		yrBuild[2] = m_buildDateStr[9];
		yrBuild[3] = m_buildDateStr[10];
		yrBuild[4] = 0;
		dayBuild[0] = m_buildDateStr[4];
		dayBuild[1] = m_buildDateStr[5];
		dayBuild[2] = 0;
		monthCh[0] = m_buildDateStr[0];
		monthCh[1] = m_buildDateStr[1];
		monthCh[2] = m_buildDateStr[2];
		monthCh[3] = 0;
		int mn = MonthChtoInt(monthCh);
		m_buildDateStrPacked.Format("%s%02d%s", yrBuild, mn, dayBuild);
	}
	pCardGYDIB = NULL;
	pCardRYDIB = NULL;
	pCardYDIB = NULL;
	stoplightDIB = NULL;
	checkDIB = NULL;
//	pcGyVal = 20;
//	pcRyVal = 70;
//	pcYVal = 10;
	rerapVarID = 12;//IC
	m_OverallCP1Color = RGB(0, 0, 0);
	m_OverallCP2Color = RGB(0, 0, 0);
	m_OverallCP2 = TRUE;
	m_OverallCP1 = TRUE;
	m_enableFuture = FALSE;
	//eventDays = 1;
	//eventHours = 1;
	int i;
	for(i = 0; i < OVERLAYS; i++)
	{
		overLayYears[i] = 0;
		overLayColors[i] = RGB(0, 0, 0);
		lineStyles[i] = PS_SOLID;
		lineWidths[i] = 1;
	}
	maxMissingWx = 5;
	lastGraphicFormat = BMP;
	for(i = 0; i < MAXMERGES; i++)
	{
		mergeColors[i] = 0;
		mergeWidths[i] = 1;
		mergeStyles[i] = 0;
	}
	m_ForceNFDRS2016Recompute = FALSE;
	m_UseDailyExtremes = FALSE;
	//hourlyQueryRows = NULL;
	//queryRows = NULL;
	m_OnlineHelpURL = "https://www.firelab.org/document/ffp-tech-tips";
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFireplusApp object

CFireplusApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFireplusApp initialization

BOOL CFireplusApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
	//AfxGetModuleState()->m_dwVersion = 0x0601;
//	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	AfxOleInit();
	AfxEnableControlContainer();
	AfxInitRichEdit( );
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

//	Enable3dControls();			// Call this when using MFC in a shared DLL
/*#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif*/

	// Change the registry key under which our settings are stored.
	// You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("FireFamilyPlus5"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_FIREPLTYPE,
		RUNTIME_CLASS(CFireplusDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CFireplusView));
	AddDocTemplate(pDocTemplate);

	AddDocTemplate(new CMultiDocTemplate(IDR_TEXTTYPE,
		RUNTIME_CLASS(CRichDoc), RUNTIME_CLASS(CRichFrame),
		RUNTIME_CLASS(CRichView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_CLIMTYPE,
		RUNTIME_CLASS(CFireplusDoc), RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CClimateView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_SPLITGRAPHTYPE,
		RUNTIME_CLASS(CPeriodDoc), RUNTIME_CLASS(CSplitterFrame),
		RUNTIME_CLASS(CGraphView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_MERGEDGRAPH,
		RUNTIME_CLASS(CPeriodDoc), RUNTIME_CLASS(CMergedFrame),
		RUNTIME_CLASS(CMergedView)));
//	AddDocTemplate(new CMultiDocTemplate(IDR_PRECIPGRAPHTYPE,
//		RUNTIME_CLASS(CPeriodDoc), RUNTIME_CLASS(CSplitterFrame),
//		RUNTIME_CLASS(CPrecipGraphView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_POCKETCARDTYPE,
		RUNTIME_CLASS(CPocketCardDoc), RUNTIME_CLASS(CPocketCardFrame),
		RUNTIME_CLASS(CPocketCardView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_FIRESUMMENU,
		RUNTIME_CLASS(CFireSumDoc), RUNTIME_CLASS(CFireSumWnd),
		RUNTIME_CLASS(CFireSumView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_FIRESTATMENU,
		RUNTIME_CLASS(CRichFireDoc), RUNTIME_CLASS(CFireStatFrame),
	//	RUNTIME_CLASS(CRichFireView)));
		RUNTIME_CLASS(CRichView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_DPID,
		RUNTIME_CLASS(CDPDoc), RUNTIME_CLASS(CDPFrame),
		RUNTIME_CLASS(CDPReportView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_DIURNALMENU,
		RUNTIME_CLASS(CDiurnalDoc), RUNTIME_CLASS(CDiurnalFrame),
		RUNTIME_CLASS(CDiurnalFormView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_CUMULATIVE,
		RUNTIME_CLASS(CFireStatDoc), RUNTIME_CLASS(CCumulativeFrame),
		RUNTIME_CLASS(CCumulativeBars)));
	AddDocTemplate(new CMultiDocTemplate(IDR_BATCHOUTPUT,
		RUNTIME_CLASS(CQuickBatch), RUNTIME_CLASS(CBatchOutputFrame),
		RUNTIME_CLASS(CBatchOutputTreeView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_WINDROSETYPE,
		RUNTIME_CLASS(CWindRoseDoc), RUNTIME_CLASS(CWindRoseFrame),
		RUNTIME_CLASS(CWindRoseView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_TERMMENU,
		RUNTIME_CLASS(CRichTermDoc), RUNTIME_CLASS(CTermFrame),
		RUNTIME_CLASS(CRichView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_EVENTLOCATORTYPE,
		RUNTIME_CLASS(CEventLocatorDoc), RUNTIME_CLASS(CEventFrame),
		RUNTIME_CLASS(CRichView)));
	//AddDocTemplate(new CMultiDocTemplate(IDR_MODELCOMPARETYPE,
	//	RUNTIME_CLASS(CModelCompareDoc), RUNTIME_CLASS(CModelCompareFrame),
	//	RUNTIME_CLASS(CModelCompareView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_WEATHERVIEW_TMPL,
		RUNTIME_CLASS(CWeatherDoc), RUNTIME_CLASS(CWeatherFrame),
		RUNTIME_CLASS(CWeatherView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_FIRESVIEW_TMPL,
		RUNTIME_CLASS(CFiresDoc), RUNTIME_CLASS(CFiresFrame),
		RUNTIME_CLASS(CFiresView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_WEATHERVIEW_TMPL1,
		RUNTIME_CLASS(CCandidatesDoc), RUNTIME_CLASS(CCandidatesFrame),
		RUNTIME_CLASS(CCandidatesView)));
	AddDocTemplate(new CMultiDocTemplate(IDR_NFDRS2016WXVIEW_TMPL,
		RUNTIME_CLASS(CNFDRS2016WxDoc), RUNTIME_CLASS(CNFDRS2016WxFrame),
		RUNTIME_CLASS(CNFDRS2016WxView)));


	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	char wd[256];
	exeDir.Format("%s", _getcwd(wd, 255));
	if (strlen(m_lpCmdLine) > 0)
	{
		workDir = m_lpCmdLine;
		if (workDir.ReverseFind('\\') == workDir.GetLength() - 1
			|| workDir.ReverseFind('/') == workDir.GetLength() - 1)//last character trailing backslash or slash?
			workDir.SetAt(workDir.GetLength() - 1, 0);
		chdir(workDir);
	}
	else
		workDir = exeDir;
	userDir = workDir;

	pCardRYDIB = LoadDIB("ry.bmp");//ReadDIBFile(bmpFile);
	pCardGYDIB = LoadDIB("gy.bmp");//ReadDIBFile(bmpFile);
	pCardYRDIB = LoadDIB("yr.bmp");//ReadDIBFile(bmpFile);
	pCardYGDIB = LoadDIB("yg.bmp");//ReadDIBFile(bmpFile);
	pCardYDIB = LoadDIB("y.bmp");//ReadDIBFile(bmpFile);
	stoplightDIB = LoadDIB("stoplght.bmp");//ReadDIBFile(bmpFile);
	checkDIB = LoadDIB("check.bmp");//ReadDIBFile(bmpFile);
	//set up DIBrects
	LPSTR lpDIB = (LPSTR)GlobalLock((HGLOBAL)pCardGYDIB);
	pCardGYRect.left = 0;
	pCardGYRect.top = 0;
	pCardGYRect.right = DIBWidth(lpDIB);
	pCardGYRect.bottom = DIBHeight(lpDIB);
	GlobalUnlock((HGLOBAL) pCardGYDIB);
	lpDIB = (LPSTR)GlobalLock((HGLOBAL)pCardRYDIB);
	pCardRYRect.left = 0;
	pCardRYRect.top = 0;
	pCardRYRect.right = DIBWidth(lpDIB);
	pCardRYRect.bottom = DIBHeight(lpDIB);
	GlobalUnlock((HGLOBAL) pCardRYDIB);
	lpDIB = (LPSTR)GlobalLock((HGLOBAL)pCardYRDIB);
	pCardYRRect.left = 0;
	pCardYRRect.top = 0;
	pCardYRRect.right = DIBWidth(lpDIB);
	pCardYRRect.bottom = DIBHeight(lpDIB);
	GlobalUnlock((HGLOBAL) pCardYRDIB);
	lpDIB = (LPSTR)GlobalLock((HGLOBAL)pCardYGDIB);
	pCardYGRect.left = 0;
	pCardYGRect.top = 0;
	pCardYGRect.right = DIBWidth(lpDIB);
	pCardYGRect.bottom = DIBHeight(lpDIB);
	GlobalUnlock((HGLOBAL) pCardYGDIB);
	lpDIB = (LPSTR)GlobalLock((HGLOBAL)pCardYDIB);
	pCardYRect.left = 0;
	pCardYRect.top = 0;
	pCardYRect.right = DIBWidth(lpDIB);
	pCardYRect.bottom = DIBHeight(lpDIB);
	GlobalUnlock((HGLOBAL) pCardYDIB);
	lpDIB = (LPSTR)GlobalLock((HGLOBAL)stoplightDIB);
	stoplightRect.left = 0;
	stoplightRect.top = 0;
	stoplightRect.right = DIBWidth(lpDIB);
	stoplightRect.bottom = DIBHeight(lpDIB);
	GlobalUnlock((HGLOBAL) stoplightDIB);
	lpDIB = (LPSTR)GlobalLock((HGLOBAL)checkDIB);
	checkRect.left = 0;
	checkRect.top = 0;
	checkRect.right = DIBWidth(lpDIB);
	checkRect.bottom = DIBHeight(lpDIB);
	GlobalUnlock((HGLOBAL) checkDIB);
	//ReadDocOpts();
	/*nQrows = 1;
	queryRows = new QueryRow[1];
	queryRows[0].cat = Sum;
	queryRows[0].rowOp = AND;
	queryRows[0].valOp = GE;
	queryRows[0].valType = Value;
	queryRows[0].value = 0.0;
	queryRows[0].varID = 8;//precip - 1
	queryRows[0].varString = "Precipitation Amount";*/

	//ReadDocOpts();
/*	dllHandle = LoadLibrary("logit.dll");
	if(!dllHandle)
	{
		AfxMessageBox("Error loading logit.dll. Bye Bye");
		return TRUE;
	}*/
	/*logReg = (LPLogReg)GetProcAddress(dllHandle, "LogRegs");
	if(!logReg)
	{
		AfxMessageBox("Error loading LogRegs function from logit.dll. Bye Bye");
		return TRUE;
	}*/
	//readMe.Open("readme.txt", CFile::modeRead | CFile::shareDenyNone);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line
//	if (!ProcessShellCommand(cmdInfo))
//		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	if(m_pRecentFileList)
	{
		if( m_pRecentFileList->GetSize() > 0)
		{
			CString fStr = (*m_pRecentFileList)[0];
			if(fStr.GetLength() > 0){
			    // 08/2006: first check if the file exists
                FILE *temp;
				temp = fopen(fStr,"r");
				if (temp != NULL){
				   fclose(temp);
				   OpenDocumentFile(fStr);
				}
			}
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	
	
//	CEdit	textBox;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
	

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	//CExplorer1 m_WebBrowser;
	CString m_strCredits;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{

	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	m_strCredits =
		"Who's Responsible?\r\n\r\nTechnical Contact: \r\n\tMatt Jolly (matt.jolly@usda.gov) at the\r\n\tRocky Mountain Research Station's Missoula Fire Sciences Laboratory.\r\n" \
		"\r\nNational Support is provided by\r\n\tFire and Aviation Management, National Information Systems Group, USDA, Forest Service -- \r\n" \
		"\tPhone: 866 - 224 - 7677\r\n" \
		"\tEmail: iia-helpdesk@usda.gov\r\n\r\n" \
		"Sponsorship:\r\n\tUSDA, Forest Service, Fire and Aviation Management, Washington, DC.\r\n" \
		"\tUSDA, Forest Service, Rocky Mountain Research Station, Fort Collins, CO\r\n\r\n" \
		"Programming:\r\n\tStuart Brittain, Alturas Solutions LLC, Missoula, Montana\r\n\r\n";

}


BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
/*
	// TODO: Add extra initialization here
	CString fStr;
	char pbuf[101];
	//UINT nBytesRead = cfile.Read( pbuf, 100 );
	theApp.readMe.SeekToBegin();
	int n;
	while((n = theApp.readMe.Read(pbuf, 100)) > 0)
	{
		pbuf[n] = 0;
		fStr += pbuf;
	}
	textBox.SetWindowText(fStr);
*/
	/*CString pdfURL;
	
	pdfURL.Format("%s\\FFP_42.pdf", theApp.workDir);
	
	m_WebBrowser.Navigate(pdfURL,NULL,NULL,NULL,NULL);*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//DDX_Control(pDX, IDC_EDIT1, textBox);
	CString tStr;
	//tStr.Format("%s",verStr);
	tStr.Format("FF+%s %s", verStr, theApp.m_buildDateStr);
	DDX_Text(pDX, IDC_VERSION_STRING, tStr);

	//}}AFX_DATA_MAP
	//DDX_Control(pDX, IDC_EXPLORER1, m_WebBrowser);
	DDX_Text(pDX, IDC_EDIT1, m_strCredits);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CFireplusApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}




/////////////////////////////////////////////////////////////////////////////
// CNASFDlg dialog used for App About
/*
class CNASFDlg : public CDialog
{
public:
	CNASFDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_NASFBOX };
	
	
//	CEdit	textBox;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
	

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CExplorer1 m_WebBrowser;
};

CNASFDlg::CNASFDlg() : CDialog(CNASFDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	

}


BOOL CNASFDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString pdfURL;
	
	pdfURL.Format("%s\\NASFFire.pdf", theApp.workDir);
	
	m_WebBrowser.Navigate(pdfURL,NULL,NULL,NULL,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNASFDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//DDX_Control(pDX, IDC_EDIT1, textBox);
//	CString tStr;
//	tStr.Format("%s",verStr);
//	DDX_Text(pDX, IDC_VERSION_STRING, tStr);

	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EXPLORER1, m_WebBrowser);
}

BEGIN_MESSAGE_MAP(CNASFDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



// App command to run the dialog
void CFireplusApp::OnAppNASF()
{
	CNASFDlg nasfDlg;
	nasfDlg.DoModal();
}

*/
/////////////////////////////////////////////////////////////////////////////
// CFireplusApp commands

void DisplayDBException(CDBException* e)
{
	CString strMsg;
	//if (e->m_pErrorInfo!=NULL)
	//{
       AfxMessageBox( e->m_strError,
                      MB_ICONEXCLAMATION );
         e->Delete();
		//AfxMessageBox(strMsg);
	//}
}

void CFireplusApp::AddToRecentFileList(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
	int i = strlen(lpszPathName);
	if(i > 7)
	{//only addd mdb's to MRU file list
		if(strnicmp(&lpszPathName[i - 3], "mdb", 3) != 0)
			return;
	}
	CWinApp::AddToRecentFileList(lpszPathName);
}

void CFireplusApp::OnHelpContents()
{
	// TODO: Add your command handler code here
	CString src;
	src.Format("%s\\ffp4.chm", workDir);
	src += "::/FireFamilyPlus Help/Introduction/Welcome to FireFamilyPlus.htm";
	::HtmlHelp(AfxGetMainWnd()->m_hWnd, src, HH_DISPLAY_TOC, NULL);//(DWORD)topic);
	//if (!::WinHelp(m_pMainWnd->m_hWnd, src, HELP_FINDER, 0L))
	//	AfxMessageBox(AFX_IDP_FAILED_TO_LAUNCH_HELP);
}

void CFireplusApp::OnHelpSearch()
{
	CA2T pszTMP(m_OnlineHelpURL);
	ShellExecute(NULL, NULL, pszTMP, NULL, NULL, SW_SHOWNORMAL);
	//OnHelpFinder();
	/*HH_FTS_QUERY q;
	q.cbStruct         = sizeof(HH_FTS_QUERY) ;
	q.fUniCodeStrings  = FALSE ;
	q.pszSearchQuery   = NULL;
	q.iProximity       = HH_FTS_DEFAULT_PROXIMITY ;
	q.fStemmedSearch   = FALSE ;
	q.fTitleOnly       = FALSE ;
	q.fExecute         = FALSE ;
	q.pszWindow        = NULL ;*/
	//CString src;
	//src.Format("%s\\ffp4.chm", workDir);
	//src += "::Help for FireFamily Plus 4.0/Introduction/Welcome to FireFamily Plus.htm";
	//::HtmlHelp(AfxGetMainWnd()->m_hWnd, src, HH_DISPLAY_TOPIC, NULL);

	//if (!::WinHelp(m_pMainWnd->m_hWnd, src, HELP_CONTENTS, 0L))
	//if (!::WinHelp(m_pMainWnd->m_hWnd, src, HELP_FINDER, 0L))
		//AfxMessageBox(AFX_IDP_FAILED_TO_LAUNCH_HELP);
}


int CFireplusApp::ExitInstance()
{
//	if(dllHandle)
//		FreeLibrary(dllHandle);
	//WriteDocOpts();
	//if(queryRows)
		//delete[] queryRows;Sum
	//if(hourlyQueryRows)
		//delete[] hourlyQueryRows;
	if(	pCardYDIB != NULL)
		::GlobalFree((HGLOBAL) pCardYDIB);
	if(	pCardGYDIB != NULL)
		::GlobalFree((HGLOBAL) pCardGYDIB);
	if(	pCardRYDIB != NULL)
		::GlobalFree((HGLOBAL) pCardRYDIB);
	if(	pCardYGDIB != NULL)
		::GlobalFree((HGLOBAL) pCardYGDIB);
	if(	pCardYRDIB != NULL)
		::GlobalFree((HGLOBAL) pCardYRDIB);
	if(	stoplightDIB != NULL)
		::GlobalFree((HGLOBAL) stoplightDIB);
	if(	checkDIB != NULL)
		::GlobalFree((HGLOBAL) checkDIB);
	//readMe.Close();
	//delete state files
	CString findStr;
	findStr.Format("%s\\*.nfd", dbDir);
	CFileFind ff;
	BOOL more = ff.FindFile(findStr);
	while(more)
	{
		more = ff.FindNextFile();
		unlink(ff.GetFilePath());
	}

	return CWinApp::ExitInstance();
}

void CFireplusApp::OnFileNewFfp()
{
	CString src, dest, filter;
	chdir(userDir);
	filter = "FireFamily Plus Files (*.mdb)|*.mdb||";
	CFileDialog fd(FALSE, "mdb", "untitled.mdb", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		filter, NULL);
	fd.m_ofn.lpstrTitle = "New FireFamily Plus Database";
	if(IDOK == fd.DoModal())
	{
		CWaitCursor wait;
		//CloseAllDocuments(FALSE);
		src.Format("%s\\ffplus5.ffp", workDir);
		dest = fd.GetPathName();
		unlink(dest);
		//CDaoWorkspace workSpace;
		//workSpace.Open();
		//CompactDatabase(src, dest);
		CopyFile(src,dest,false); // To copy a file to another location and 
        // overwrite the destination file if it already exists.
      
		//workSpace.Close();
		//now open the puppy
		OpenDocumentFile(dest);
		//userDir automatically updated in OpenDocumentFile()
	}
	chdir(theApp.dbDir);//workDir);
	//AfxMessageBox("Unimplemented! (Database engine change...)");
}

void CFireplusApp::SetUserDir(CString fullPath)
{
	CString tDir = fullPath;
	int eloc = tDir.ReverseFind('\\');
	if(eloc >= 0)
		tDir.SetAt(eloc, 0);
	userDir = tDir;
}

void CFireplusApp::OnFileOpen()
{
	// TODO: Add your command handler code here
	chdir(userDir);
	CWinApp::OnFileOpen();
	chdir(workDir);
}

CString CFireplusApp::GetGraphicsFileStr()
{
	CString ret = "Bitmap Files (*.bmp)|*.bmp|PNG Files (*.png)|*.png|JPEG Files (*.jpg)|*.jpg|GIF Files (*.gif)|*.gif|TIFF Files (*.tif)|*.tif|All Files (*.*)|*.*||";
	/*CString strFilter;
	CSimpleArray<GUID> aguidFileTypes;
	HRESULT hResult;

	hResult = CImage::GetExporterFilterString(strFilter,aguidFileTypes);
	if (FAILED(hResult)) {
		CString fmt;
		//fmt.Format("GetExporterFilter failed:\n%x - %s", hResult, _com_error(hResult).ErrorMessage());
		AfxMessageBox("GetExporterFilter failed");//fmt);
		return "";
	}*/
	switch(lastGraphicFormat)
	{
	case BMP:
		ret = "Bitmap Files (*.bmp)|*.bmp|PNG Files (*.png)|*.png|JPEG Files (*.jpg)|*.jpg|GIF Files (*.gif)|*.gif|TIFF Files (*.tif)|*.tif|All Files (*.*)|*.*||";
		break;
	case PNG:
		ret = "PNG Files (*.png)|*.png|Bitmap Files (*.bmp)|*.bmp|JPEG Files (*.jpg)|*.jpg|GIF Files (*.gif)|*.gif|TIFF Files (*.tif)|*.tif|All Files (*.*)|*.*||";
		break;
	case JPG:
		ret = "JPEG Files (*.jpg)|*.jpg|PNG Files (*.png)|*.png|Bitmap Files (*.bmp)|*.bmp|GIF Files (*.gif)|*.gif|TIFF Files (*.tif)|*.tif|All Files (*.*)|*.*||";
		break;
	case TIF:
		ret = "TIFF Files (*.tif)|*.tif|PNG Files (*.png)|*.png|Bitmap Files (*.bmp)|*.bmp|JPEG Files (*.jpg)|*.jpg|GIF Files (*.gif)|*.gif|All Files (*.*)|*.*||";
		break;
	case GIF:
		ret = "GIF Files (*.gif)|*.gif|Bitmap Files (*.bmp)|*.bmp|PNG Files (*.png)|*.png|JPEG Files (*.jpg)|*.jpg|TIFF Files (*.tif)|*.tif|All Files (*.*)|*.*||";
		break;
	}
	return ret;//strFilter;
}

CString CFireplusApp::GetGraphicsExt()
{
	CString ret = "bmp";
	switch(lastGraphicFormat)
	{
	case BMP:
		ret = "bmp";
		break;
	case PNG:
		ret = "png";
		break;
	case JPG:
		ret = "jpg";
		break;
	case TIF:
		ret = "tif";
		break;
	case GIF:
		ret = "gif";
		break;
	}
	return ret;
}

void CFireplusApp::SetGraphicsExt(CString ext)
{
	ext.MakeLower();
	if(ext.Compare("png") == 0)
		lastGraphicFormat = PNG;
	else if(ext.Compare("jpg") == 0)
		lastGraphicFormat = JPG;
	else if(ext.Compare("gif") == 0)
		lastGraphicFormat = GIF;
	else if(ext.Compare("tif") == 0)
		lastGraphicFormat = TIF;
	else
		lastGraphicFormat = BMP;
}

/*void CFireplusApp::ReadDocOpts()
{
	char buf[256];
	CString fName;
	fName.Format("%s\\ffpopts.dat", workDir);
	FILE *stream = fopen(fName, "rt");
	if(!stream)
		return;
	fgets(buf, 255, stream);
	//pcRyVal = atoi(buf);
	fgets(buf, 255, stream);
	//pcGyVal = atoi(buf);
	fgets(buf, 255, stream);
	//eventHours = atoi(buf);
	//if(eventHours <= 0)
	//	eventHours = 1;
	fgets(buf, 255, stream);
	//rerapVarID = atoi(buf);
	fgets(buf, 255, stream);
	//m_OverallCP1 = atoi(buf);
	fgets(buf, 255, stream);
	//m_OverallCP1Color = atol(buf);
	fgets(buf, 255, stream);
	//m_OverallCP2 = atoi(buf);
	fgets(buf, 255, stream);
	//m_OverallCP2Color = atol(buf);
	fgets(buf, 255, stream);
	//eventDays = atoi(buf);
	fgets(buf, 255, stream);
	//maxMissingWx = atoi(buf);
	fgets(buf, 255, stream);
	//lastGraphicFormat = (GFile)atoi(buf);
	int i;
	for(i = 0; i < OVERLAYS; i++)
	{
		fgets(buf, 255, stream);
		overLayYears[i] = atoi(buf);
	}
	for(i = 0; i < OVERLAYS; i++)
	{
		fgets(buf, 255, stream);
		overLayColors[i] = atol(buf);
	}
	for(i = 0; i < OVERLAYS; i++)
	{
		fgets(buf, 255, stream);
		lineWidths[i] = atoi(buf);
	}
	for(i = 0; i < OVERLAYS; i++)
	{
		fgets(buf, 255, stream);
		lineStyles[i] = atoi(buf);
	}
	for(i = 0; i < MAXMERGES; i++)
	{
		fgets(buf, 255, stream);
		mergeColors[i] = atol(buf);
	}
	for(i = 0; i < MAXMERGES; i++)
	{
		fgets(buf, 255, stream);
		mergeStyles[i] = atoi(buf);
	}
	for(i = 0; i < MAXMERGES; i++)
	{
		fgets(buf, 255, stream);
		mergeWidths[i] = atoi(buf);
	}
	fgets(buf, 255, stream);
	if(queryRows)
	{
		delete[] queryRows;
		queryRows = NULL;
	}
	nQrows = atoi(buf);
	if(nQrows > 0)
		queryRows = new QueryRow[nQrows];
	for(i = 0; i < nQrows; i++)
	{
		fgets(buf, 255, stream);
		queryRows[i].cat = (Category)atoi(buf);
		fgets(buf, 255, stream);
		queryRows[i].rowOp = (RowOp)atoi(buf);
		fgets(buf, 255, stream);
		queryRows[i].valOp = (ValOp)atoi(buf);
		fgets(buf, 255, stream);
		queryRows[i].valType = (ValType)atoi(buf);
		fgets(buf, 255, stream);
		queryRows[i].value = atof(buf);
		fgets(buf, 255, stream);
		queryRows[i].varID = atoi(buf);
		fgets(buf, 255, stream);
		queryRows[i].varString = buf;
		queryRows[i].varString.TrimRight();
	}
	fclose(stream);
}*/

/*void CFireplusApp::WriteDocOpts()
{
	CString fName;
	fName.Format("%s\\ffpopts.dat", theApp.workDir);
	FILE *stream = fopen(fName, "wt");
	fprintf(stream, "%d\n", 0);
	fprintf(stream, "%d\n", 0);
	//fprintf(stream, "%d\n", 0);
	fprintf(stream, "%d\n", 1);
	fprintf(stream, "%d\n", rerapVarID);
	fprintf(stream, "%d\n", m_OverallCP1);
	fprintf(stream, "%ld\n", m_OverallCP1Color);
	fprintf(stream, "%d\n", m_OverallCP2);
	fprintf(stream, "%ld\n", m_OverallCP2Color);
	fprintf(stream, "%d\n", 1);
	fprintf(stream, "%d\n", maxMissingWx);
	fprintf(stream, "%d\n", lastGraphicFormat);
	int i;
	for(i = 0; i < OVERLAYS; i++)
		fprintf(stream, "%d\n", overLayYears[i]);
	for(i = 0; i < OVERLAYS; i++)
		fprintf(stream, "%ld\n", overLayColors[i]);
	for(i = 0; i < OVERLAYS; i++)
		fprintf(stream, "%d\n", lineWidths[i]);
	for(i = 0; i < OVERLAYS; i++)
		fprintf(stream, "%d\n", lineStyles[i]);
	for(i = 0; i < MAXMERGES; i++)
		fprintf(stream, "%ld\n", mergeColors[i]);
	for(i = 0; i < MAXMERGES; i++)
		fprintf(stream, "%d\n", mergeStyles[i]);
	for(i = 0; i < MAXMERGES; i++)
		fprintf(stream, "%d\n", mergeWidths[i]);
	fprintf(stream, "%d\n", nQrows);
	for(i = 0; i < nQrows; i++)
	{
		fprintf(stream, "%d\n", queryRows[i].cat);
		fprintf(stream, "%d\n", queryRows[i].rowOp);
		fprintf(stream, "%d\n", queryRows[i].valOp);
		fprintf(stream, "%d\n", queryRows[i].valType);
		fprintf(stream, "%f\n", queryRows[i].value);
		fprintf(stream, "%d\n", queryRows[i].varID);
		fprintf(stream, "%s\n", queryRows[i].varString);
	}
	fclose(stream);
}*/

void DecimalDegree2DDMMSS(double decimalDegree, int *degrees, int *minutes, int *seconds)
{
	double d, m, s, t;
	if(decimalDegree >= 0)
		d = floor(decimalDegree);
	else
		d = ceil(decimalDegree);
	t = fabs((decimalDegree - d) * 60.0);
	m = floor(t);
	s = (t - m) * 60.0;
	s = Round(s);
	if(s >= 60.0)
	{
		m++;
		s = 0.0;
	}
	if(m >= 60.0)
	{
		if(decimalDegree >= 0)
			d++;
		else
			d--;
		m = 0.0;
	}
	*degrees = (int)d;
	*minutes = (int)m;
	*seconds = (int)s;
}

void DDMMSS2DecimalDegree(int degrees, int minutes, int seconds, double *decimalDegree)
{
	double dd = degrees;
	if(degrees >= 0)
	{
		dd += ((double)minutes) / 60.0;
		dd += ((double)seconds) / 3600.0;
	}
	else
	{
		dd -= ((double)minutes) / 60.0;
		dd -= ((double)seconds) / 3600.0;
	}
	*decimalDegree = dd;
}

// //gets pointer to an active CCandidatesFrame for a database
CCandidatesFrame * CFireplusApp::GetCandidatesFrame(CDatabase * pDB)
{
	CMDIFrameWnd *mainFrame = (CMDIFrameWnd *)AfxGetMainWnd();
	CWnd *child = mainFrame->MDIGetActive();//, *topMost = NULL;
	//CMainFrame *mainFrame = AfxGetMainWnd();
	//CWnd *next = AfxGetMainWnd();
	//CWnd *next = mainFrame->GetActiveWindow();

	while(child)
	{
		CRuntimeClass* prt = child->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CCandidatesFrame" )  == 0)
		{
			CCandidatesFrame *pCand = (CCandidatesFrame *)child;
			if(pDB == pCand->m_pDB)
			//next->BringWindowToTop();
				return pCand;//(CCandidatesFrame *)child;
		}
		child = child->GetNextWindow();
	}
	return NULL;
}

// set default printer setting to landscape
void CFireplusApp::SetLandscape()
    {
    // Get default printer settings.
    PRINTDLG   pd;

    pd.lStructSize = (DWORD) sizeof(PRINTDLG);
    if (GetPrinterDeviceDefaults(&pd))
        {
        // Lock memory handle.
        DEVMODE FAR* pDevMode =
            (DEVMODE FAR*)::GlobalLock(m_hDevMode);
        LPDEVNAMES lpDevNames;
        LPTSTR lpszDriverName, lpszDeviceName, lpszPortName;
        HANDLE hPrinter;

        if (pDevMode)
            {
            // Change printer settings in here.
            pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
           // Unlock memory handle.
       lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
       lpszDriverName = (LPTSTR )lpDevNames + lpDevNames->wDriverOffset;
       lpszDeviceName = (LPTSTR )lpDevNames + lpDevNames->wDeviceOffset;
       lpszPortName   = (LPTSTR )lpDevNames + lpDevNames->wOutputOffset;

       ::OpenPrinter(lpszDeviceName, &hPrinter, NULL);
       ::DocumentProperties(NULL,hPrinter,lpszDeviceName,pDevMode,
                           pDevMode, DM_IN_BUFFER|DM_OUT_BUFFER);

       // Sync the pDevMode.
       // See SDK help for DocumentProperties for more info.
       ::ClosePrinter(hPrinter);
       ::GlobalUnlock(m_hDevNames);
       ::GlobalUnlock(m_hDevMode);
       }
    }
}

CString CFireplusApp::getVarSortOrderStr(int i)
{
    switch (i)
	{
	case 1: return _T("Current variable sort order: Alphabetical"); break;
	case 2: return _T("Current variable sort order: Group (WX/NFDRS/CFDR)"); break;
    default: return _T("Current variable sort order: ID (ffpClimateOptions table)"); break;
	}
}

void CFireplusApp::UpdateCandidatesFrame(CDatabase * pDB)
{
	CCandidatesFrame *pFrame = GetCandidatesFrame(pDB);
	if(pFrame)
	{
		CCandidatesView * pView = (CCandidatesView *)pFrame->GetActiveView();
		pView->records->Requery();
		pView->ResetRecords(pView->records);
		pView->GetDocument()->UpdateAllViews(NULL);
		pFrame->BringWindowToTop();
		//pFrame->m
	}
	else
	{
		CCandidateSet *candSet = new CCandidateSet(pDB);
		candSet->Open();
		/*if(candSet->IsBOF() || candSet->IsEOF())
		{
			AfxMessageBox("No fire business candidates saved");
			candSet->Close();
			delete candSet;
			return;
		}*/
		//create a document to view the results....
		POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate =
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("CandidatesView"))
			{
				CCandidatesDoc *pCandDoc = (CCandidatesDoc *)curTemplate->CreateNewDocument();
				pCandDoc->records = candSet;
				CCandidatesFrame* pCandFrame = (CCandidatesFrame*)curTemplate->CreateNewFrame(pCandDoc, NULL);
				curTemplate->InitialUpdateFrame(pCandFrame, pCandDoc);
				pCandFrame->m_pDB = pDB;
				POSITION pos = pCandDoc->GetFirstViewPosition();
				CCandidatesView *pView = (CCandidatesView *)pCandDoc->GetNextView(pos);
				pView->SetRecords(pCandDoc->records);
				CString tTitle;
				tTitle = "Fire Business Candidates";
				pCandDoc->SetTitle(tTitle);
			}
		}
	}
}

void CFireplusApp::OverrideMXD(CSIGStationSet* staSet)
{
	if (!staSet->IsFieldNull(&staSet->m_MXD_Override) && staSet->m_MXD_Override > 0)
	{
		int iSA1, iSA10, iSA100, iSA1000, iSAWOOD, iSAHERB;
		double fW1, fW10, fW100, fW1000, fWWOOD, fWHERB, fWDROUGHT, fDEPTH;
		int iMXD, iHD, iSCM;
		double fWNDFC, fWNDFCMin, fWNDFCMax;
		m_NFDRS.iGetFuelBed(&iSA1, &iSA10, &iSA100, &iSA1000,
			&iSAWOOD, &iSAHERB, &fW1, &fW10, &fW100,
			&fW1000, &fWWOOD, &fWHERB, &fWDROUGHT, &fDEPTH,
			&iMXD, &iHD, &iSCM, &fWNDFC, &fWNDFCMin, &fWNDFCMax);
		m_NFDRS.iSetFuelBed(iSA1, iSA10, iSA100, iSA1000,
			iSAWOOD, iSAHERB, fW1, fW10, fW100,
			fW1000, fWWOOD, fWHERB, fWDROUGHT, fDEPTH,
			staSet->m_MXD_Override, iHD, iSCM, fWNDFC, fWNDFCMin, fWNDFCMax);
	}
}

// MainFrm.cpp : implementation of the CMainFrame class
//
 
#include "stdafx.h"
#include <afxpriv.h>
//#include "dibapi.h"
#include "fireplus.h"
//#include "datatbl.h"
#include "graphset.h"
#include "reportoptionsset.h"
//#include "ForecastDialog.h"
#include "MainFrm.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "SplitterFrame.h"
#include "LineGraphProp.h"
#include "BargraphProp.h"
#include "GeneralGraphProp.h"
#include "GraphFiresPage.h"
#include "GraphAutoSaveProp.h"
#include "FDProjPage.h"
//#include "MergedVarsPropPage.h"
#include "GraphPropSheet.h"
#include "ReportPropSheet.h"
#include "BatchPropSheet.h"
#include "fireplusSet.h"
#include "fireplusDoc.h"
#include "fireplusView.h"
#include "CompactDialog.h"
#include "SIGStationSet.h"
#include "StatCauseSet.h"

#include <direct.h>
#include <oledb.h>
//#include "msjetoledb.h"
#include "jetoledb.h"	// for IJetCompact interface
#include "msjetoledb.h"	// for IJetCompact interface
#include <atldbcli.h>
#include "SnowFlagDlg.h"
#include "FillSnowflagDlg.h"
#include "DailyObsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//compact support functions, gotta use OLE DB...

long GetJetEngineType( LPCTSTR src );

class OLEINITIALIZE
{
	bool m_bOleInit;
public:
	OLEINITIALIZE()
	{
		m_bOleInit= (CoInitialize(NULL)==S_OK);
	}
	~OLEINITIALIZE()
	{
		if (m_bOleInit)
			CoUninitialize();
	}
};

HRESULT CompactDatabase(LPCTSTR src, LPCTSTR dest)
{
	// Initialize environment must be the first line in your function
	OLEINITIALIZE oleinit;

	CDataSource ds;
	CComPtr<IJetCompact> spJetCompact =NULL;
	CComPtr<IDBCreateSession> spSession =NULL;
	HRESULT	hr=0;

	//Specify the source DSO
	ds.Open(CLSID_JETOLEDB_4_00, src);

	CDBPropSet propset1(DBPROPSET_DBINIT);
	propset1.AddProperty(DBPROP_INIT_DATASOURCE, dest);

	long x = GetJetEngineType( src );
	CDBPropSet propset2(DBPROPSET_JETOLEDB_DBINIT);
	propset2.AddProperty(DBPROP_JETOLEDB_ENGINE, x);

	CDBPropSet dbsets[2] = { propset1, propset2 };

	// Have we connected to the database?
	ATLASSERT(ds.m_spInit != NULL);
	hr = ds.m_spInit->QueryInterface(IID_IDBCreateSession, (void**)&spSession);
	if (FAILED(hr))
		return hr;

	//IJetCompact only supported in Jet 4.0 and above
	hr = spSession->QueryInterface( __uuidof(IJetCompact), (void**)&spJetCompact);
	if (FAILED(hr))
		return hr;

	//Delete the destination file if it exists
	remove(dest);

	//Ok compact
	//hr = spJetCompact->Compact(1, &propset);
	hr = spJetCompact->Compact(1, dbsets);

	if (FAILED(hr))
		return hr;

	return hr;
}

long GetJetEngineType( LPCTSTR src )
{
	HRESULT hr;
	CDataSource ds;
	CComBSTR bstrSource;
	VARIANT vPropValue;

	// Initialize our variant to VT_I4 and 0.
	vPropValue.vt   = VT_I4;
	vPropValue.lVal = 0L;

	// Exit now if source database is null.
	if( NULL == src ) return 0;

	// Build connection string for source.
	bstrSource = L"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=";
	bstrSource += src;
	bstrSource += L";";

	hr = ds.OpenFromInitializationString( bstrSource );
	hr = ds.GetProperty( DBPROPSET_JETOLEDB_DBINIT, DBPROP_JETOLEDB_ENGINE, &vPropValue );

	// Version returned will be one of these values:
	//
	// #define JETDBENGINETYPE_UNKNOWN	0x00
	// #define JETDBENGINETYPE_JET10	0x01
	// #define JETDBENGINETYPE_JET11	0x02
	// #define JETDBENGINETYPE_JET2X	0x03
	// #define JETDBENGINETYPE_JET3X	0x04
	// #define JETDBENGINETYPE_JET4X	0x05

	return vPropValue.lVal;
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_CLOSEALL_REPORTSANDGRAPHS, OnWindowCloseallReportsandgraphs)
	ON_COMMAND(ID_WINDOW_CLOSEALL_WINDOWS, OnWindowCloseallWindows)
	ON_COMMAND(ID_OPTIONS_DISPLAYOPTONS, OnOptionsDisplayoptons)
	ON_COMMAND(ID_OPTIONS_REPORTOPTIONS, OnOptionsReportOptions)
	ON_COMMAND(ID_OPTIONS_BATCH, OnOptionsBatchOptions1)
	ON_COMMAND(ID_OPTIONS_BATCH2, OnOptionsBatchOptions2)
	ON_WM_CLOSE()
	ON_COMMAND(ID_DATA_COMPACT, OnDataCompact)
	ON_COMMAND(ID_FILE_SAVE_AS_FFP, OnFileSaveAsFfp)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_DISPLAYOPTONS, OnUpdateOptionsDisplayoptons)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_REPORTOPTIONS, OnUpdateOptionsReportOptions)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_BATCH, OnUpdateOptionsBatchOptions)
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_SNOWFLAG_SETBYFILTER, &CMainFrame::OnUtilitiesSnowflag)
	ON_COMMAND(ID_SNOWFLAG_FILLHOURLYFROMDAILY, &CMainFrame::OnUtilitiesFillSnowflag)
	//ID_UTILITIES_SETDAILYOBS
	ON_COMMAND(ID_UTILITIES_SETDAILYOBS, &CMainFrame::OnUtilitiesSetDailyObs)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

extern CFireplusApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	graphProperties = NULL;
	reportProperties = NULL;
	batchProperties = NULL;
	//	forecastDialog = NULL;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndMDIClient.SubclassWindow (m_hWndMDIClient))
	{
		TRACE ("Failed to subclass MDI client window\n");
		return (-1);
	}

	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	/*CRect nRect(0, 0, 1, 1);
	theApp.m_NFDRS.Create(NULL, WS_CHILD,//WS_DISABLED,
	nRect, this, IDC_NFDRS);*/

	if (theApp)
		m_wndStatusBar.SetPaneText(0, theApp.getVarSortOrderStr(theApp.varSortOrder));

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnWindowCloseallReportsandgraphs()
{
	// TODO: Add your command handler code here
	CWnd *next;
	CWnd *hWndChild = next = MDIGetActive();
	while(next)
	{
		hWndChild = next;
		next = hWndChild->GetNextWindow();
		CRuntimeClass* prt = hWndChild->GetRuntimeClass();
		/*if(strcmp( prt->m_lpszClassName, "CRichFrame" )  == 0
		|| strcmp( prt->m_lpszClassName, "CSplitterFrame" ) == 0
		|| strcmp( prt->m_lpszClassName, "CFireSumWnd" ) == 0
		|| strcmp( prt->m_lpszClassName, "CFireStatFrame" ) == 0
		|| strcmp( prt->m_lpszClassName, "CCumulativeFrame" ) == 0
		|| strcmp( prt->m_lpszClassName, "CPocketCardFrame" ) == 0)*/
		if(strcmp( prt->m_lpszClassName, "CMainFrame" )  != 0
			&& strcmp( prt->m_lpszClassName, "CMDIChildWnd" ) != 0
			&& strcmp( prt->m_lpszClassName, "CChildFrame" ) != 0)
			hWndChild->DestroyWindow();
	}
}

void CMainFrame::OnWindowCloseallWindows()
{
	// TODO: Add your command handler code here
	//check for any windows that depend on FirePlus Document and database!
	
	CWnd *next;
	CWnd *hWndChild = next = MDIGetActive();
	while(next)
	{
		hWndChild = next;
		next = hWndChild->GetNextWindow();
		CRuntimeClass* prt = hWndChild->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CChildFrame" )  != 0 )
			hWndChild->DestroyWindow();
	}
	theApp.CloseAllDocuments(false);
}

void CMainFrame::OnOptionsDisplayoptons()
{
	// TODO: Add your command handler code here
	CDatabase *pDB = NULL;
	if(!graphProperties)
	{
		CMDIChildWnd *next;
		CFireplusView *fpView;
		CMDIChildWnd *hWndChild = next = MDIGetActive();
		while(next)
		{
			hWndChild = next;
			next = (CMDIChildWnd *) hWndChild->GetNextWindow();
			CRuntimeClass* prt = hWndChild->GetRuntimeClass();
			if(strcmp( prt->m_lpszClassName, "CChildFrame" )  == 0)
			{
				fpView = (CFireplusView *)hWndChild->GetActiveView();
				if(fpView)
					pDB = fpView->m_pSet->m_pDatabase;
				break;
			}
		}
		graphProperties = new CGraphPropSheet("Graph Display Options", fpView, 0, pDB, this);
		graphProperties->Create();
		//	CGraphPropSheet gps("Graph Display Options", fpView, 0, pDB, this);
		//gps.DoModal();
	}
	else
		graphProperties->SetActiveWindow();
}

// added for batch
void CMainFrame::OnOptionsReportOptions()
{
	// TODO: Add your command handler code here
	CDatabase *pDB = NULL;
	if(!reportProperties)
	{
		CMDIChildWnd *next;
		CFireplusView *fpView;
		CMDIChildWnd *hWndChild = next = MDIGetActive();
		while(next)
		{
			hWndChild = next;
			next = (CMDIChildWnd *) hWndChild->GetNextWindow();
			CRuntimeClass* prt = hWndChild->GetRuntimeClass();
			if(strcmp( prt->m_lpszClassName, "CChildFrame" )  == 0)
			{
				fpView = (CFireplusView *)hWndChild->GetActiveView();
				if(fpView)
					pDB = fpView->m_pSet->m_pDatabase;
				break;
			}
		}
		reportProperties = new CReportPropSheet("Report Detail Options", fpView, 0, pDB, this);
		reportProperties->Create();
	}
	else
		reportProperties->SetActiveWindow();
}
// end added for batch

// added for batch3
void CMainFrame::OnOptionsBatchOptions1()
{
	// TODO: Add your command handler code here
	CDatabase *pDB = NULL;
	if(!batchProperties)
	{
		CMDIChildWnd *next;
		CFireplusView *fpView;
		CMDIChildWnd *hWndChild = next = MDIGetActive();
		while(next)
		{
			hWndChild = next;
			next = (CMDIChildWnd *) hWndChild->GetNextWindow();
			CRuntimeClass* prt = hWndChild->GetRuntimeClass();
			if(strcmp( prt->m_lpszClassName, "CChildFrame" )  == 0)
			{
				fpView = (CFireplusView *)hWndChild->GetActiveView();
				if(fpView)
					pDB = fpView->m_pSet->m_pDatabase;
				break;
			}
		}
		batchProperties = new CBatchPropSheet("Batch / Run Options", fpView,0, pDB, this);
		batchProperties->Create();
	}
	else
		batchProperties->SetActiveWindow();
}
// end added for batch3

// added 09/2012
void CMainFrame::OnOptionsBatchOptions2()
{
	// TODO: Add your command handler code here
	CDatabase *pDB = NULL;
	if(!batchProperties)
	{
		CMDIChildWnd *next;
		CFireplusView *fpView;
		CMDIChildWnd *hWndChild = next = MDIGetActive();
		while(next)
		{
			hWndChild = next;
			next = (CMDIChildWnd *) hWndChild->GetNextWindow();
			CRuntimeClass* prt = hWndChild->GetRuntimeClass();
			if(strcmp( prt->m_lpszClassName, "CChildFrame" )  == 0)
			{
				fpView = (CFireplusView *)hWndChild->GetActiveView();
				if(fpView)
					pDB = fpView->m_pSet->m_pDatabase;
				break;
			}
		}
		batchProperties = new CBatchPropSheet("Batch / Run Options", fpView, 1, pDB, this);
		batchProperties->Create();
	}
	else
		batchProperties->SetActiveWindow();
}
//


void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if(graphProperties)
	{
		graphProperties->OnMyCancel();
		graphProperties = NULL;
	}
	if(batchProperties)
	{
		batchProperties->OnMyCancel();
		batchProperties = NULL;
	}
	if(reportProperties)
	{
		reportProperties->OnMyCancel();
		reportProperties = NULL;
	}
	OnWindowCloseallReportsandgraphs();
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnDataCompact()
{
	// TODO: Add your command handler code here
	//AfxMessageBox("Not yet implemented due to database engine change");
	/*CCompactDialog cd(this);
	if(cd.DoModal() == IDOK)
	{
	if(cd.doDupCheck == TRUE)
	{
	if(RemoveWxDuplicates() != 0)
	return;
	}
	//close the database, open a daoworkspace, and call compact,
	//then rename the file back to original
	CWaitCursor wait;
	CMDIChildWnd *pChild = MDIGetActive();
	CFireplusView *tview = (CFireplusView *)pChild->GetActiveView();
	CString fileName = tview->GetDocument()->GetPathName();
	OnWindowCloseallWindows();
	CDaoWorkspace workSpace;
	workSpace.Open();
	workSpace.CompactDatabase(fileName, "$temp$.mdb");
	workSpace.Close();
	unlink(fileName);
	rename("$temp$.mdb", fileName);
	unlink("$temp$.mdb");
	//now open the puppy
	theApp.OpenDocumentFile(fileName);
	}*/
	CCompactDialog cd(this);
	if(cd.DoModal() == IDOK)
	{
		/*if(cd.doDupCheck == TRUE)
		{
		if(RemoveWxDuplicates() != 0)
		return;
		}*/
		//close the database, open a daoworkspace, and call compact,
		//then rename the file back to original
		CWaitCursor wait;
		CMDIChildWnd *pChild = MDIGetActive();
		CFireplusView *tview = (CFireplusView *)pChild->GetActiveView();
		CString fileName = tview->GetDocument()->GetPathName();
		OnWindowCloseallWindows();
		CompactDatabase(fileName, "$temp$.mdb");
		unlink(fileName);
		rename("$temp$.mdb", fileName);
		unlink("$temp$.mdb");
		//now open the puppy
		theApp.OpenDocumentFile(fileName);
	}
}

void CMainFrame::SaveItAs()
{
	OnFileSaveAsFfp();
}

void CMainFrame::OnFileSaveAsFfp()
{
	// TODO: Add your command handler code here
	//AfxMessageBox("Not yet implemented due to database engine change");
	CString src, dest;
	CMDIChildWnd *pChild = MDIGetActive();
	CFireplusView *tview = (CFireplusView *)pChild->GetActiveView();
	src = tview->GetDocument()->GetPathName();
	chdir(theApp.userDir);
	CFileDialog fd(FALSE, "mdb", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"FireFamily Plus Files (*.mdb)|*.mdb||", this);
	if(IDOK == fd.DoModal())
	{
		CWaitCursor wait;
		dest = fd.GetPathName();
		unlink(dest);
		OnWindowCloseallWindows();
		CompactDatabase(src, dest);
		//now open the puppy
		theApp.OpenDocumentFile(dest);
		theApp.SetUserDir(fd.GetPathName());
	}
	chdir(theApp.dbDir);
}

int CMainFrame::RemoveWxDuplicates()
{
	//AfxMessageBox("Not yet implemented due to database engine change");
	CWaitCursor wait;
	CMDIChildWnd *pChild = MDIGetActive();
	CFireplusView *tview = (CFireplusView *)pChild->GetActiveView();
	CFireplusDoc *tDoc = (CFireplusDoc *)tview->GetDocument();
	CWxSet wxSet(tDoc->m_pDB);
	wxSet.m_strSort = "[ObsDate]";
	CSIGStationSet stns(tDoc->m_pDB);
	stns.Open();
	COleDateTime tDate, nextDate;
	EnableWindow(FALSE);

	/*CCancelDialog *cd = NULL;

	cd = new CCancelDialog();

	if (cd != NULL)
	{
		BOOL ret = cd->Create(IDD_CANCELDIALOG,AfxGetMainWnd());

      if (!ret)   //Create failed.
      {
       
		 cd = 0;
      } else
	  {
	     cd->Reset();
         cd->ShowWindow(SW_SHOW);
	     cd->SetStatusString("Removing Duplicate Weather Records");
		 cd->SetTitle("Removing Duplicates");
	  }
	}

	*/
	
	//	wxSet.m_pDatabase->m_pWorkspace->BeginTrans();
	while(!stns.IsEOF())// && (!cd || !cd->Abort()))
	{
		/*if (cd)
		{
		   cd->Reset();
		   cd->SetStatusString(CString("Checking Station: " + stns.m_StationID));
		}*/
		//wxSet.m_strFilter.Format("[StationID] = '%s' AND [ObsType] = 'O'", stns.m_StationID);
		wxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [DailyObs] = 1", stns.m_StationID);
		wxSet.Open();
		tDate.SetDateTime(1901, 1, 1, 0, 0, 0);
		tDoc->m_pDB->BeginTrans();
		while(!wxSet.IsEOF())// && (!cd || !cd->Abort()))
		{
			nextDate.SetDateTime(wxSet.m_ObsDate.GetYear(), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay(),
				wxSet.m_ObsDate.GetHour(), wxSet.m_ObsDate.GetMinute(), wxSet.m_ObsDate.GetSecond());
			if(tDate.GetYear() == nextDate.GetYear()
				&& tDate.GetMonth() == nextDate.GetMonth()
				&& tDate.GetDay() == nextDate.GetDay())
				wxSet.Delete();
			else
				tDate = nextDate;
			wxSet.MoveNext();
			//if (cd)
			//	cd->Increment();
		}
		//if(!cd || !cd->Abort())
		//{
			tDoc->m_pDB->CommitTrans();
			wxSet.Close();
			wxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [ObsType] = 'R'", stns.m_StationID);
			wxSet.Open();
			tDate.SetDateTime(1901, 1, 1, 0, 0, 0);
			tDoc->m_pDB->BeginTrans();
			while(!wxSet.IsEOF())// && (!cd || !cd->Abort()))
			{
				//nextDate = wxSet.m_ObsDate;
				nextDate.SetDateTime(wxSet.m_ObsDate.GetYear(), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay(),
					wxSet.m_ObsDate.GetHour(), wxSet.m_ObsDate.GetMinute(), wxSet.m_ObsDate.GetSecond());
				if(tDate.GetYear() == nextDate.GetYear()
					&& tDate.GetMonth() == nextDate.GetMonth()
					&& tDate.GetDay() == nextDate.GetDay()
					&& tDate.GetHour() == nextDate.GetHour()
					&& tDate.GetMinute() == nextDate.GetMinute())
					wxSet.Delete();
				else
					tDate = nextDate;
				wxSet.MoveNext();
				//if (cd)
				 //  cd->Increment();
			}
			//if(!cd || !cd->Abort())
				tDoc->m_pDB->CommitTrans();
			//else
			//	tDoc->m_pDB->Rollback();
		//}
		//else
		//	tDoc->m_pDB->Rollback();
		wxSet.Close();
		stns.MoveNext();
	}
	stns.Close();
	BOOL ret = 0;
	int retCode = ret;
	//if(!cd || cd->Abort())
		//retCode = -1;
	//else
	   EnableWindow(TRUE);
	//if (cd)
	//	cd->DestroyWindow();
	//	wxSet.Close();
	//	BOOL ret = 0;
	return retCode;
}

void CMainFrame::OnUpdateOptionsDisplayoptons(CCmdUI* pCmdUI)
{
	CMDIChildWnd *hWndChild = MDIGetActive();
	if(hWndChild)
	{
		CRuntimeClass* prt = hWndChild->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CChildFrame" )  == 0
			|| strcmp( prt->m_lpszClassName, "CMDIChildWnd" )  == 0
			|| strcmp( prt->m_lpszClassName, "CSplitterFrame" )  == 0)
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}

// added for batch
void CMainFrame::OnUpdateOptionsReportOptions(CCmdUI* pCmdUI)
{
	CMDIChildWnd *hWndChild = MDIGetActive();
	if(hWndChild)
	{
		CRuntimeClass* prt = hWndChild->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CChildFrame" )  == 0
			|| strcmp( prt->m_lpszClassName, "CMDIChildWnd" )  == 0
			|| strcmp( prt->m_lpszClassName, "CSplitterFrame" )  == 0)
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}
// end added for batch

// added for batch
void CMainFrame::OnUpdateOptionsBatchOptions(CCmdUI* pCmdUI)
{
	CMDIChildWnd *hWndChild = MDIGetActive();
	if(hWndChild)
	{
		CRuntimeClass* prt = hWndChild->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CChildFrame" )  == 0
			|| strcmp( prt->m_lpszClassName, "CMDIChildWnd" )  == 0
			|| strcmp( prt->m_lpszClassName, "CSplitterFrame" )  == 0)
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}
	pCmdUI->Enable(FALSE);
}
// end added for batch

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	CMDIFrameWnd::OnSetFocus(pOldWnd);

	/*	CMDIChildWnd *next;
	CFireplusView *fpView;
	CMDIChildWnd *hWndChild = next = MDIGetActive();
	while(next)
	{
	hWndChild = next;
	next = (CMDIChildWnd *) hWndChild->GetNextWindow();
	CRuntimeClass* prt = hWndChild->GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, "CChildFrame" )  == 0)
	{
	fpView = (CFireplusView *)hWndChild->GetActiveView();
	if(fpView)
	{
	if(fpView->forecastDialog)
	fpView->forecastDialog->ShowWindow(SW_HIDE);
	}
	//pDB = fpView->m_pSet->m_pDatabase;
	//break;
	}
	}
	*/
}

//*************************************************************************
// OnSetMessageString
//*************************************************************************
LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	UINT nIDLast = m_nIDLastMessage;
	m_nFlags &= ~WF_NOPOPMSG;

	CWnd* pMessageBar = GetMessageBar();
	if (pMessageBar != NULL)
	{
		CString sMsg;
		CString strMessage;

		// set the message bar text
		if (lParam != 0)
		{
			ASSERT(wParam == 0);    // can't have both an ID and a string
			m_sStatusBarString = (LPCTSTR)lParam;
			sMsg = m_sStatusBarString;
		}
		else if (wParam != 0)
		{
			// map SC_CLOSE to PREVIEW_CLOSE when in print preview mode
			if (wParam == AFX_IDS_SCCLOSE && m_lpfnCloseProc != NULL)
				wParam = AFX_IDS_PREVIEW_CLOSE;

			// get message associated with the ID indicated by wParam
			if (wParam == AFX_IDS_IDLEMESSAGE)
				sMsg = m_sStatusBarString;
			else
			{
				GetMessageString(wParam, strMessage);
				sMsg = strMessage;
			}
		}

		pMessageBar->SetWindowText(sMsg);

		// update owner of the bar in terms of last message selected
		CFrameWnd* pFrameWnd = pMessageBar->GetParentFrame();
		if (pFrameWnd != NULL)
		{
			m_nIDLastMessage = (UINT)wParam;
			m_nIDTracking = (UINT)wParam;
		}
	}

	m_nIDLastMessage = (UINT)wParam;    	// new ID (or 0)
	m_nIDTracking = (UINT)wParam;       	// so F1 on toolbar buttons work

	return nIDLast;
}
//*************************************************************************
// StatusBarMessage
//*************************************************************************
void CMainFrame::StatusBarMessage(CString msg)
{
	CStatusBar* pStatus = (CStatusBar*)
		GetDescendantWindow(AFX_IDW_STATUS_BAR);

	m_sStatusBarString = msg;

	SetMessageText((LPCTSTR)m_sStatusBarString);
	return;
}

void CMainFrame::OnUtilitiesSnowflag()
{
	CMDIChildWnd *pChild = MDIGetActive();
	CFireplusView *tview = (CFireplusView *)pChild->GetActiveView();
	CFireplusDoc *tDoc = (CFireplusDoc *)tview->GetDocument();
	CSnowFlagDlg sfd;
	sfd.m_pDoc = tDoc;
	sfd.DoModal();
}

void CMainFrame::OnUtilitiesFillSnowflag()
{
	CMDIChildWnd *pChild = MDIGetActive();
	CFireplusView *tview = (CFireplusView *)pChild->GetActiveView();
	CFireplusDoc *tDoc = (CFireplusDoc *)tview->GetDocument();
	CFillSnowflagDlg sfd;
	sfd.m_pDoc = tDoc;
	sfd.DoModal();
}

void CMainFrame::OnUtilitiesSetDailyObs()
{
	CMDIChildWnd *pChild = MDIGetActive();
	CFireplusView *tview = (CFireplusView *)pChild->GetActiveView();
	CFireplusDoc *tDoc = (CFireplusDoc *)tview->GetDocument();
	CDailyObsDialog dod;
	dod.m_pDoc = tDoc;
	dod.DoModal();
}

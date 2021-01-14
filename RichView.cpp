// RichView.cpp : implementation file
//

#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include <afxrich.h>
#include "RichView.h"
#include<direct.h>
#include "richdoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
//EXTERN_PROCESS_LOCAL(_AFX_WIN_STATE, _afxWinState)
/////////////////////////////////////////////////////////////////////////////
// Printing Dialog
bool m_bUserAbort;

class CPrintingDialog : public CDialog
{
public:
	//{{AFX_DATA(CPrintingDialog)
	enum { IDD = AFX_IDD_PRINTDLG };
	//}}AFX_DATA
	CPrintingDialog::CPrintingDialog(CWnd* pParent)
		{
			Create(CPrintingDialog::IDD, pParent);      // modeless !
			m_bUserAbort = FALSE;
		}
	virtual ~CPrintingDialog() { }

	//virtual BOOL OnInitDialog();
	//virtual void OnCancel();
};

/*BOOL CALLBACK _AfxAbortProc(HDC, int)
{
//	_AFX_WIN_STATE* pWinState = _afxWinState;
	MSG msg;
	while (!m_bUserAbort &&
		::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage())
			return FALSE;   // terminate if WM_QUIT received
	}
	return !m_bUserAbort;
}

BOOL CPrintingDialog::OnInitDialog()
{
	SetWindowText(AfxGetAppName());
	CenterWindow();
	return CDialog::OnInitDialog();
}

void CPrintingDialog::OnCancel()
{
	m_bUserAbort = TRUE;  // flag that user aborted print
	CDialog::OnCancel();
}

*/

/////////////////////////////////////////////////////////////////////////////
// CRichView

IMPLEMENT_DYNCREATE(CRichView, CRichEditView)

CRichView::CRichView()
{
	m_nWordWrap = WrapNone;
	m_nLastPage = 0;
}

CRichView::~CRichView()
{
}

BEGIN_MESSAGE_MAP(CRichView, CRichEditView)
	//{{AFX_MSG_MAP(CRichView)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CRichView::OnFilePrint)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichView diagnostics

#ifdef _DEBUG
void CRichView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CRichView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRichView message handlers

void CRichView::OnInitialUpdate() 
{

	CRichEditView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
}

int CRichView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	//USES_CONVERSION;
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

	GetRichEditCtrl().SetSel(0,-1);
	GetRichEditCtrl().SetDefaultCharFormat(cf);
	GetRichEditCtrl().SetSelectionCharFormat(cf);

	CRect mrgns(720, 720, -20000, 720);
	SetMargins(mrgns);
	m_nWordWrap = WrapNone;
	WrapChanged();
	return 0;
}



/*BOOL CRichView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	
	return CRichEditView::OnPreparePrinting(pInfo);
}

void CRichView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CRichEditView::OnPrint(pDC, pInfo);
}*/

/*void CRichView::OnFilePrint() 
{
	// TODO: Add your command handler code here
	//CPrintInfo pInfo;
	//pInfo.m_pPD->m_pd.Flags |= PD_RETURNDC; 

	//DoPreparePrinting(&pInfo);
	CRichEditView::OnFilePrint();
	
}*/

BOOL CRichView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box
	BOOL ret =  DoPreparePrinting(pInfo);
	m_nWordWrap = WrapNone;
	WrapChanged();
	return ret;
	//return CRichEditView::OnPreparePrinting(pInfo);
}

void CRichView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	if(pInfo->m_bPreview || pInfo->m_nCurPage != m_nLastPage)
	{
		m_nLastPage = pInfo->m_nCurPage;
		CRichEditView::OnPrint(pDC, pInfo);
	}
//	if (pInfo != NULL && pInfo->m_bPreview)
//		DrawMargins(pDC);
	// TODO: Add your specialized code here and/or call the base class
	
	//CRichEditView::OnPrint(pDC, pInfo);
}

void CRichView::OnPrinterChanged( const CDC& dcPrinter )
{
	CRichEditView::OnPrinterChanged(dcPrinter);
	CRect mrgns(720, 720, -20000, 720);
	SetMargins(mrgns);
	m_nWordWrap = WrapNone;
	WrapChanged();

}

/*int CRichView::GetNumPages(CDC* pDC, CPrintInfo* pInfo)
{
	int ret = 1;
	FORMATRANGE fr;

	// offset by printing offset
	pDC->SetViewportOrg(-pDC->GetDeviceCaps(PHYSICALOFFSETX),
		-pDC->GetDeviceCaps(PHYSICALOFFSETY));
	// adjust DC because richedit doesn't do things like MFC
	if (::GetDeviceCaps(pDC->m_hDC, TECHNOLOGY) != DT_METAFILE && pDC->m_hAttribDC != NULL)
	{
		::ScaleWindowExtEx(pDC->m_hDC,
			::GetDeviceCaps(pDC->m_hDC, LOGPIXELSX),
			::GetDeviceCaps(pDC->m_hAttribDC, LOGPIXELSX),
			::GetDeviceCaps(pDC->m_hDC, LOGPIXELSY),
			::GetDeviceCaps(pDC->m_hAttribDC, LOGPIXELSY), NULL);
	}

	fr.hdcTarget = pDC->m_hAttribDC;
	fr.hdc = pDC->m_hDC;
	fr.rcPage = GetPageRect();
	fr.rc = GetPrintRect();

	fr.chrg.cpMin = 0;//nIndexStart;
	fr.chrg.cpMax = -1;//nIndexStop;
	GetRichEditCtrl().FormatRange(NULL, FALSE); // required by RichEdit to clear out cache
	long lRes = GetRichEditCtrl().FormatRange(&fr,FALSE);
	while(lRes < GetTextLength())
	{
		fr.chrg.cpMin = lRes;
		GetRichEditCtrl().FormatRange(NULL, FALSE); // required by RichEdit to clear out cache
		lRes = GetRichEditCtrl().FormatRange(&fr,FALSE);
		ret++;
	}
	GetRichEditCtrl().FormatRange(NULL, FALSE); // required by RichEdit to clear out cache

	return ret;
}*/

void CRichView::OnFilePrint()
{
	//m_bInPrint = TRUE;
	m_nLastPage = 0;
	CRichEditView::OnFilePrint();
	// printer may have change
//	theApp.NotifyPrinterChanged(); // this will cause a GetDocument()->PrinterChanged();
	//m_bInPrint = FALSE;

	// get default print info
/*	CPrintInfo printInfo;
	ASSERT(printInfo.m_pPD != NULL);    // must be set

	if (LOWORD(GetCurrentMessage()->wParam) == ID_FILE_PRINT_DIRECT)
	{
		CCommandLineInfo* pCmdInfo = AfxGetApp()->m_pCmdInfo;

		if (pCmdInfo != NULL)
		{
			if (pCmdInfo->m_nShellCommand == CCommandLineInfo::FilePrintTo)
			{
				printInfo.m_pPD->m_pd.hDC = ::CreateDC(pCmdInfo->m_strDriverName,
					pCmdInfo->m_strPrinterName, pCmdInfo->m_strPortName, NULL);
				if (printInfo.m_pPD->m_pd.hDC == NULL)
				{
					AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
					return;
				}
			}
		}

		printInfo.m_bDirect = TRUE;
	}

	if (OnPreparePrinting(&printInfo))
	{
		// hDC must be set (did you remember to call DoPreparePrinting?)
		ASSERT(printInfo.m_pPD->m_pd.hDC != NULL);

		// gather file to print to if print-to-file selected
		CString strOutput;
		if (printInfo.m_pPD->m_pd.Flags & PD_PRINTTOFILE && !printInfo.m_bDocObject)
		{
			// construct CFileDialog for browsing
			CString strDef(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULTEXT));
			CString strPrintDef(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULT));
			CString strFilter(MAKEINTRESOURCE(AFX_IDS_PRINTFILTER));
			CString strCaption(MAKEINTRESOURCE(AFX_IDS_PRINTCAPTION));
			CFileDialog dlg(FALSE, strDef, strPrintDef,
				OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter);
			dlg.m_ofn.lpstrTitle = strCaption;

			if (dlg.DoModal() != IDOK)
				return;

			// set output device to resulting path name
			strOutput = dlg.GetPathName();
		}

		// set up document info and start the document printing process
		CString strTitle;
		CDocument* pDoc = GetDocument();
		if (pDoc != NULL)
			strTitle = pDoc->GetTitle();
		else
			GetParentFrame()->GetWindowText(strTitle);
		if (strTitle.GetLength() > 31)
			strTitle.ReleaseBuffer(31);
		DOCINFO docInfo;
		memset(&docInfo, 0, sizeof(DOCINFO));
		docInfo.cbSize = sizeof(DOCINFO);
		docInfo.lpszDocName = strTitle;
		CString strPortName;
		int nFormatID;
		if (strOutput.IsEmpty())
		{
			docInfo.lpszOutput = NULL;
			strPortName = printInfo.m_pPD->GetPortName();
			nFormatID = AFX_IDS_PRINTONPORT;
		}
		else
		{
			docInfo.lpszOutput = strOutput;
			//MyGetFileTitle(strOutput,
				//strPortName.GetBuffer(_MAX_PATH), _MAX_PATH);
			strPortName = pDoc->GetTitle();
			nFormatID = AFX_IDS_PRINTTOFILE;
		}

		// setup the printing DC
		CDC dcPrint;
		if (!printInfo.m_bDocObject)
		{
			dcPrint.Attach(printInfo.m_pPD->m_pd.hDC);  // attach printer dc
			dcPrint.m_bPrinting = TRUE;
		}
		OnBeginPrinting(&dcPrint, &printInfo);

		if (!printInfo.m_bDocObject)
			dcPrint.SetAbortProc(_AfxAbortProc);

		// disable main window while printing & init printing status dialog
		AfxGetMainWnd()->EnableWindow(FALSE);
		CPrintingDialog dlgPrintStatus(this);

		CString strTemp;
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_DOCNAME, strTitle);
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PRINTERNAME,
			printInfo.m_pPD->GetDeviceName());
		AfxFormatString1(strTemp, nFormatID, strPortName);
		dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PORTNAME, strTemp);
		dlgPrintStatus.ShowWindow(SW_SHOW);
		dlgPrintStatus.UpdateWindow();

	//	printInfo.m_pPD->m_pd.nToPage = printInfo.m_pPD->m_pd.nMaxPage = GetNumPages(&dcPrint, &printInfo);
		// start document printing process
		if (!printInfo.m_bDocObject && dcPrint.StartDoc(&docInfo) == SP_ERROR)
		{
			// enable main window before proceeding
			AfxGetMainWnd()->EnableWindow(TRUE);

			// cleanup and show error message
			OnEndPrinting(&dcPrint, &printInfo);
			dlgPrintStatus.DestroyWindow();
			dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
			AfxMessageBox(AFX_IDP_FAILED_TO_START_PRINT);
			return;
		}

		//printInfo.m_pPD->m_pd.nToPage = printInfo.m_pPD->m_pd.nMaxPage = GetNumPages(&dcPrint, &printInfo);
		// Guarantee values are in the valid range
		UINT nEndPage = printInfo.GetToPage();
		UINT nStartPage = printInfo.GetFromPage();

		if (nEndPage < printInfo.GetMinPage())
			nEndPage = printInfo.GetMinPage();
		if (nEndPage > printInfo.GetMaxPage())
			nEndPage = printInfo.GetMaxPage();

		if (nStartPage < printInfo.GetMinPage())
			nStartPage = printInfo.GetMinPage();
		if (nStartPage > printInfo.GetMaxPage())
			nStartPage = printInfo.GetMaxPage();

		int nStep = (nEndPage >= nStartPage) ? 1 : -1;
		nEndPage = (nEndPage == 0xffff) ? 0xffff : nEndPage + nStep;

		VERIFY(strTemp.LoadString(AFX_IDS_PRINTPAGENUM));

		// If it's a doc object, we don't loop page-by-page
		// because doc objects don't support that kind of levity.

		BOOL bError = FALSE;
		if (printInfo.m_bDocObject)
		{
			OnPrepareDC(&dcPrint, &printInfo);
			OnPrint(&dcPrint, &printInfo);
		}
		else
		{
			// begin page printing loop
			for (printInfo.m_nCurPage = nStartPage;
				printInfo.m_nCurPage != nEndPage; printInfo.m_nCurPage += nStep)
			{
				OnPrepareDC(&dcPrint, &printInfo);

				// check for end of print
				if (!printInfo.m_bContinuePrinting)
					break;

				// write current page
				TCHAR szBuf[80];
				wsprintf(szBuf, strTemp, printInfo.m_nCurPage);
				dlgPrintStatus.SetDlgItemText(AFX_IDC_PRINT_PAGENUM, szBuf);

				// set up drawing rect to entire page (in logical coordinates)
				printInfo.m_rectDraw.SetRect(0, 0,
					dcPrint.GetDeviceCaps(HORZRES),
					dcPrint.GetDeviceCaps(VERTRES));
				dcPrint.DPtoLP(&printInfo.m_rectDraw);

				// attempt to start the current page
				if (dcPrint.StartPage() < 0)
				{
					bError = TRUE;
					break;
				}

				// must call OnPrepareDC on newer versions of Windows because
				// StartPage now resets the device attributes.
			//	if (afxData.bMarked4)
					OnPrepareDC(&dcPrint, &printInfo);

				ASSERT(printInfo.m_bContinuePrinting);

				// page successfully started, so now render the page
				OnPrint(&dcPrint, &printInfo);
				if (dcPrint.EndPage() < 0 || !_AfxAbortProc(dcPrint.m_hDC, 0))
				{
					bError = TRUE;
					break;
				}
			}
		}

		// cleanup document printing process
		if (!printInfo.m_bDocObject)
		{
			if (!bError)
				dcPrint.EndDoc();
			else
				dcPrint.AbortDoc();
		}

		AfxGetMainWnd()->EnableWindow();    // enable main window

		OnEndPrinting(&dcPrint, &printInfo);    // clean up after printing
		dlgPrintStatus.DestroyWindow();

		dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
	}*/
}





void CRichView::OnFileSaveAs() 
{
	chdir(theApp.userDir);
	CFileDialog fd(false, "txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", this);
	if(fd.DoModal() == IDOK)
	{
		CRichDoc *pDoc = (CRichDoc *)GetDocument();
		char trg[MAX_PATH];
		strcpy(trg, fd.GetPathName());
		if(pDoc)
			pDoc->OnSaveDocument(trg);
		theApp.SetUserDir(fd.GetPathName());
		//theApp.SetGraphicsExt(fd.GetFileExt());
	}
	chdir(theApp.dbDir);

	
}

void CRichView::PrintTheDamnThing()
{
	OnFilePrint();
}

void CRichView::SaveTheDamnThing()
{
	OnFileSaveAs();
}

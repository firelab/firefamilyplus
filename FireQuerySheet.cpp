// FireQuerySheet.cpp : implementation file
//
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "fireplusset.h"
#include "FireAssocSet.h"
#include "FireAgencySet.h"
#include "FireRegionSet.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"
#include "FireQueryPage.h"
#include "YearsPage.h"
#include "FireQuerySheet.h"
#include "FireEditSet.h"
#include "FiresDoc.h"
#include "FiresView.h"
#include "FiresFrame.h"
#include "TextDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CFireQuerySheet

IMPLEMENT_DYNAMIC(CFireQuerySheet, CPropertySheet)

CFireQuerySheet::CFireQuerySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage,
		CDatabase *_pDB, CString _staStr, int *start, int *end, CFireplusSet *_fpSet, CFireplusSet *_fpSet2)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage),
	associations(_pDB), agencies(_pDB), regions(_pDB), units(_pDB), subUnits(_pDB), editButton(), viewButton()
{
	fpSet = _fpSet;
	fpSet2 = _fpSet2;

	

	

	agencyPages = 0;
	associations.Open();
	agencies.Open();
	regions.Open();
	units.Open();
	subUnits.Open();
	if(_staStr.GetLength() > 6)//is a SIG
	{
		char temp[64];
		strcpy(temp, _staStr);
		staStr.Format("%s", &temp[6]);
	}
	else
		staStr = _staStr;
	
	

	while(!agencies.IsEOF())
	{
		CFireQueryPage *page = new CFireQueryPage(&associations, &agencies, &regions, &units, &subUnits, staStr);
	
		AddPage(page);
		agencyPages++;
		agencies.MoveNext();
	}
	if(start && end)
	{
		yearsPage = new CYearsPage(start, end);
		AddPage(yearsPage);
	}
	else
		yearsPage = NULL;
}

CFireQuerySheet::~CFireQuerySheet()
{
	//remove and delete the pages
	for(int i = GetPageCount() - 1; i >= 0; i--)
	{
		CPropertyPage *page = GetPage(i);
		RemovePage(page);
		delete page;
	}
	agencies.Close();
	regions.Close();
	units.Close();
	subUnits.Close();
}


BEGIN_MESSAGE_MAP(CFireQuerySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CFireQuerySheet)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_EDITFIRES, OnEditButton)
	ON_BN_CLICKED(IDC_VIEWUNITS, OnViewButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireQuerySheet message handlers

int CFireQuerySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	//CButton *ok = (CButton *)GetDlgItem(IDOK);
	//if(ok)
//	{
		CRect rect, brect, crect;
		GetWindowRect(&rect);
		GetClientRect(&crect);
		brect.bottom = rect.Height() - (rect.Height() - crect.Height()) + 1;
		brect.top = brect.bottom - 23;
		//brect.right = crect.Width();// - crect.Width() / 20;
		brect.right = crect.Width() + 70;
		brect.left = brect.right - 100;
		//rect.bottom += 30;
		//MoveWindow(rect);
	//	editButton.SetFont(GetFont());
		editButton.Create("View Fires", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 
			brect, this, IDC_EDITFIRES);
		int shift = brect.Width() + 15;
		brect.left -= shift + 25;
		brect.right -= shift;
		viewButton.Create("View Selections", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 
			brect, this, IDC_VIEWUNITS);

		shift = brect.Width() + 15;
		brect.left -= (shift + 60);
		brect.right -= shift;
		
		// DOI Button removed 8/2006
		//DOIbutton.SetFont(GetFont());
		//DOIbutton.Create("Show NWCG units", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX, 
		//	brect, this, IDC_DOIBUTTON);
      

//	}
	return 0;
}

BOOL CFireQuerySheet::OnInitDialog() 
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	
	// TODO: Add your specialized code here
		CWnd *tWin = GetWindow(GW_CHILD);
		if(tWin)
		{
			CFont *tFont = tWin->GetFont();
			if(tFont)
			{
				editButton.SetFont(tFont);
				viewButton.SetFont(tFont);
			}
		}
		CWnd *cancelButton = GetDlgItem(IDCANCEL);
		if(cancelButton->m_hWnd)
		{
			CRect rect, anchor;
			cancelButton->GetWindowRect(&anchor);
			ScreenToClient(&anchor);
			editButton.GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.top = anchor.top;
			rect.bottom = anchor.bottom;
			editButton.MoveWindow(&rect);
			viewButton.GetWindowRect(&rect);
			ScreenToClient(&rect);
			rect.top = anchor.top;
			rect.bottom = anchor.bottom;
			viewButton.MoveWindow(&rect);
		}
	
	return bResult;
}

void CFireQuerySheet::OnEditButton() 
{
	CWaitCursor wait;
	//save data from pages
	for(int i = 0; i < agencyPages; i++)
	{
		CFireQueryPage *page = (CFireQueryPage *)GetPage(i);
		if(page && page->m_hWnd != NULL)
			page->SaveData();
	}
	if(yearsPage && yearsPage->m_hWnd != NULL)
		yearsPage->SaveData();
	//now see if any associations...
	associations.m_strFilter.Format("[SIG/Station] = '%s'", staStr);
	associations.Requery();
	if(associations.IsBOF() || associations.IsEOF())
	{
		AfxMessageBox("ERROR:No associations specified.");
		return;
	}
	//have associations, now build fire edit set and make sure there's fires
	CString query, tQuery, temp, baseQuery;
	int count = 0;
	if(fpSet)
		fpSet->BuildBaseQuery(baseQuery, "Discovery", false);
	else if(yearsPage)
		baseQuery.Format("(Year([Discovery]) >= %d and Year([Discovery]) <= %d)", *yearsPage->iStart, *yearsPage->iEnd);	
	while(!associations.IsEOF())
	{
		temp = "";
		if(!associations.IsFieldNull(&associations.m_SubunitID))
			temp.Format("[SubunitID] = %ld", associations.m_SubunitID);
		else if(!associations.IsFieldNull(&associations.m_UnitID))
			temp.Format("[UnitID] = %ld", associations.m_UnitID);
		else if(!associations.IsFieldNull(&associations.m_RegionID))
			temp.Format("[RegionID] = %ld", associations.m_RegionID);
		if(count > 0)
			tQuery += " or ";
		tQuery += temp;
		associations.MoveNext();
		count++;
	}
	if(tQuery.GetLength() > 0)
		query.Format("(%s) and (%s)", tQuery, baseQuery);
	else
		query = baseQuery;
	CFireEditSet *fireEditSet = new CFireEditSet(associations.m_pDatabase);
	fireEditSet->m_strFilter = query;
	//fireEditSet.m_strFilter.Format("(%s) and (%s)", tQuery, query);
	fireEditSet->m_strSort = "[Discovery]";
	fireEditSet->Open();
	if(fireEditSet->IsBOF() || fireEditSet->IsEOF())
	{
		AfxMessageBox("Error: No fires found for specified associations.\nCheck your working set data years and annual filter.");
		fireEditSet->Close();
		delete fireEditSet;
		return;
	}
		//create a document to view the results....
		POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate = 
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("FiresView"))
			{
				CFiresDoc *pDoc = (CFiresDoc *)curTemplate->CreateNewDocument();
				pDoc->records = fireEditSet;
				CFiresFrame* pFrame = (CFiresFrame*)curTemplate->CreateNewFrame(pDoc, NULL);
				CFiresView *pView = (CFiresView *)pFrame->GetWindow(GW_CHILD);
				//pView->records = pDoc->records;
				//pView->fpSet = fpSet;
				curTemplate->InitialUpdateFrame(pFrame, pDoc);
				POSITION pos = pDoc->GetFirstViewPosition();
				pView = (CFiresView *)pDoc->GetNextView(pos);
				pView->SetRecords(pDoc->records, fpSet);
				CString tTitle;
				tTitle = "Fires";
				pDoc->SetTitle(tTitle);
				EndDialog(IDCANCEL);

			}
		}
}

void CFireQuerySheet::OnViewButton() 
{
	CWaitCursor wait;
	for(int i = 0; i < agencyPages; i++)
	{
		CFireQueryPage *page = (CFireQueryPage *)GetPage(i);
		if(page && page->m_hWnd != NULL)
			page->SaveData();
	}
	if(yearsPage && yearsPage->m_hWnd != NULL)
		yearsPage->SaveData();
	//now see if any associations...
	associations.m_strFilter.Format("[SIG/Station] = '%s'", staStr);
	associations.Requery();
	if(associations.IsBOF() || associations.IsEOF())
	{
		AfxMessageBox("ERROR:No associations specified.");
		return;
	}
	CString msg, temp;
	msg.Format("Fire Associations for %s%s:\r\n\r\n",
		(staStr.Compare("######") || staStr.GetLength() > 6) ? "" : "station ", staStr);
	while(!associations.IsEOF())
	{
		agencies.m_strFilter.Format("[AgencyID] = %ld", associations.m_AgencyID);
		agencies.Requery();
		regions.m_strFilter.Format("[RegionID] = %ld", associations.m_RegionID);
		regions.Requery();
		temp.Format("\t%s, %s", agencies.m_Name, regions.m_Name);
		msg += temp;
		if(!associations.IsFieldNull(&associations.m_UnitID))
		{
			units.m_strFilter.Format("[UnitID] = %ld", associations.m_UnitID);
			units.Requery();
			temp.Format(", %s", units.m_Name);
			msg += temp;
			if(!associations.IsFieldNull(&associations.m_SubunitID))
			{
				subUnits.m_strFilter.Format("[SubunitID] = %ld", associations.m_SubunitID);
				subUnits.Requery();
				temp.Format(", %s", subUnits.m_Name);
				msg += temp;
			}
		}
		msg += "\r\n";
		associations.MoveNext();
	}
	//now display in a text dialog
	CTextDialog td(this);
	td.textString = msg;
	td.DoModal();
}
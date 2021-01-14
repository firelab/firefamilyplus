// BatchGeneralProp.cpp : implementation file
//
#include "stdafx.h"

#include "fireplus.h"
#include "BatchOptionsSet.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "SplitterFrame.h"




#include "BatchGeneralProp.h"
#include "BatchPropSheet.h"

#include "MainFrm.h"
#include <windows.h>
#include <shlobj.h>

#include "DirDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CBatchGeneralProp property page

IMPLEMENT_DYNCREATE(CBatchGeneralProp, CPropertyPage)

CBatchGeneralProp::CBatchGeneralProp() : CPropertyPage(CBatchGeneralProp::IDD)
{
	//{{AFX_DATA_INIT(CBatchGeneralProp)
	promptDesc = FALSE;
	

	//}}AFX_DATA_INIT
}

CBatchGeneralProp::~CBatchGeneralProp()
{
}

void CBatchGeneralProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchGeneralProp)
	
	
	DDX_Check(pDX, IDC_PROMPT_DESC, promptDesc);
	
	DDX_Control(pDX, IDC_EDIT_RUN_DESC, runDesc);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchGeneralProp, CPropertyPage)
	//{{AFX_MSG_MAP(CBatchGeneralProp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_PROMPT_DESC, OnBnClickedPromptDesc)
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchGeneralProp message handlers


BOOL CBatchGeneralProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
	CWnd *child;
	CWnd *topMost = NULL;
	
	child = ((CBatchPropSheet *)GetParent())->mainFrame->MDIGetActive(); 
	CRuntimeClass* prt = child->GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, "CSplitterFrame" ) == 0)
		topMost = child;
	ReadData();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBatchGeneralProp::SetBatchOptionsSet(CBatchOptionsSet * set)
{
	boptSet = set;
}



void CBatchGeneralProp::SaveData()
{
	UpdateData();
	//just set boptSet members, Edit and Update called by property sheet
	if (boptSet->IsOpen()){
		boptSet->Edit();	
		boptSet->m_PromptDesc = promptDesc;
		CString str;

		runDesc.GetWindowText(str);
		str.Trim();
		boptSet->m_runDesc = str;
		boptSet->Update();
	}

	
}

void CBatchGeneralProp::ReadData()
{
	
		if(boptSet->IsOpen())
		{
			CString str;
			
			runDesc.SetLimitText(70);
			
			runDesc.SetWindowText(boptSet->m_runDesc);
			promptDesc = boptSet->m_PromptDesc;
			
		}
		

}

void CBatchGeneralProp::WriteData()
{
			UpdateData();
			if (boptSet->IsOpen()){
				boptSet->Edit();

			
				CString str;
				
				runDesc.GetWindowText(str);
				boptSet->m_runDesc = str;
                boptSet->m_PromptDesc = promptDesc;
				boptSet->Update();
			}
		
}

void CBatchGeneralProp::OnBnClickedPromptDesc()
{
	

	// TODO: Add your control notification handler code here
}





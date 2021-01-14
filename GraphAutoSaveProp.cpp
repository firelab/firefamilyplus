// GraphAutoSaveProp.cpp : implementation file
//
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "GraphSet.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "SplitterFrame.h"
#include "LineGraphProp.h"
#include "BargraphProp.h"
#include "GeneralGraphProp.h"
#include "GraphAutoSaveProp.h"
#include "GraphFiresPage.h"
#include "FDProjPage.h"
//#include "MergedVarsPropPage.h"
#include "GraphPropSheet.h"


#include "MainFrm.h"
#include <windows.h>
#include <shlobj.h>
#include ".\graphautosaveprop.h"
#include "DirDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CGraphAutoSaveProp property page

IMPLEMENT_DYNCREATE(CGraphAutoSaveProp, CPropertyPage)

CGraphAutoSaveProp::CGraphAutoSaveProp() : CPropertyPage(CGraphAutoSaveProp::IDD)
{
	//{{AFX_DATA_INIT(CGraphAutoSaveProp)
	//autoSave = FALSE;
	

	//}}AFX_DATA_INIT
}

CGraphAutoSaveProp::~CGraphAutoSaveProp()
{
}

void CGraphAutoSaveProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGraphAutoSaveProp)
	
	
	
	DDX_Control(pDX, IDC_FILETYPE, fileType);

	DDX_Control(pDX, IDC_EDITGWIDTH, Width);
	DDX_Control(pDX, IDC_EDITGHEIGHT, Height);
	DDX_Control(pDX, IDC_EDITLOCATION, Location);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGraphAutoSaveProp, CPropertyPage)
	//{{AFX_MSG_MAP(CGraphAutoSaveProp)
	//}}AFX_MSG_MAP
	//ON_BN_CLICKED(IDC_AUTOSAVE_GRAPH, OnBnClickedAutosaveGraph)
	ON_BN_CLICKED(IDC_BROWSE_FOLDER, OnBrowseButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphAutoSaveProp message handlers


BOOL CGraphAutoSaveProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
//	CWnd *child;
//	CWnd *topMost = NULL;
	
	/*child = ((CGraphPropSheet *)GetParent())->mainFrame->MDIGetActive(); 
	CRuntimeClass* prt = child->GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, "CSplitterFrame" ) == 0)
		topMost = child;*/
	ReadData();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGraphAutoSaveProp::SetGraphSet(CGraphSet * set)
{
	graphSet = set;
}



void CGraphAutoSaveProp::SaveData()
{
	UpdateData();
	//just set graphset members, Edit and Update called by property sheet
	
		//graphSet->m_AutoSave = autoSave;
		CString str;

		Width.GetWindowText(str);
		if (str && atoi(str))
			graphSet->m_AutoSaveWidth = atoi(str);
		else
			graphSet->m_AutoSaveWidth = 640;

		Height.GetWindowText(str);
		if (str && atoi(str))
			graphSet->m_AutoSaveHeight = atoi(str);
		else
			graphSet->m_AutoSaveHeight = 480;

		graphSet->m_AutoSaveFileType = fileType.GetCurSel();

		Location.GetWindowText(str);
		graphSet->m_AutoSaveLocation = str;
		
	
}

void CGraphAutoSaveProp::ReadData()
{
	
		if(graphSet->IsOpen())
		{
			CString str;
			//autoSave = graphSet->m_AutoSave;

			CString str2;
			if (!graphSet->m_AutoSaveWidth){
				graphSet->Edit();
				graphSet->m_AutoSaveWidth = 640;
				graphSet->Update();
			}
			str2.Format("%d",graphSet->m_AutoSaveWidth);
			Width.SetLimitText(4);
			Width.SetWindowText(str2);
			

			if (! graphSet->m_AutoSaveHeight){
				graphSet->Edit();
				graphSet->m_AutoSaveHeight = 480;
				graphSet->Update();

			}
			str.Format("%d",graphSet->m_AutoSaveHeight);
			Height.SetLimitText(4);
			Height.SetWindowText(str);

			fileType.SetCurSel(graphSet->m_AutoSaveFileType);
			Location.SetLimitText(100);
			
			Location.SetWindowText(graphSet->m_AutoSaveLocation);

			
		}
		

}

void CGraphAutoSaveProp::WriteData()
{
			UpdateData();
			//graphSet->m_AutoSave = autoSave;
			CString str;
			Width.GetWindowText(str);
			graphSet->m_AutoSaveWidth = atoi(str);

			Height.GetWindowText(str);
			graphSet->m_AutoSaveHeight = atoi(str);

			graphSet->m_AutoSaveFileType = fileType.GetCurSel();

			Location.GetWindowText(str);
			graphSet->m_AutoSaveLocation = str;
			
		
}





/*void CGraphAutoSaveProp::OnBnClickedAutosaveGraph()
{
	// TODO: Add your control notification handler code here
}*/


void CGraphAutoSaveProp::OnBrowseButton(){
// browse for folder location. update Location field.
   UpdateData(TRUE);    
   CDirDialog dirDlg;
    if(dirDlg.DoBrowse()){
		UpdateData(FALSE);
		CString DirName = dirDlg.m_strPath; 
		Location.SetWindowText(DirName);
	} 
}
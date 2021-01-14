// BatchAutoSaveProp.cpp : implementation file
//
//

#include "stdafx.h"

#include "fireplus.h"
#include "GraphSet.h"
#include "wxset.h"
#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "SplitterFrame.h"



#include "BatchAutoSaveProp.h"
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
// CBatchAutoSaveProp property page

IMPLEMENT_DYNCREATE(CBatchAutoSaveProp, CPropertyPage)

CBatchAutoSaveProp::CBatchAutoSaveProp() : CPropertyPage(CBatchAutoSaveProp::IDD)
{
	//{{AFX_DATA_INIT(CBatchAutoSaveProp)
	//autoSave = FALSE;
	

	//}}AFX_DATA_INIT
}

CBatchAutoSaveProp::~CBatchAutoSaveProp()
{
}

void CBatchAutoSaveProp::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBatchAutoSaveProp)
	
	
	
	DDX_Control(pDX, IDC_FILETYPE, fileType);
    //DDX_Check(pDX, IDC_AUTOSAVE_GRAPH, autoSave);
	DDX_Control(pDX, IDC_EDITGWIDTH, Width);
	DDX_Control(pDX, IDC_EDITGHEIGHT, Height);
	DDX_Control(pDX, IDC_EDITLOCATION, Location);
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBatchAutoSaveProp, CPropertyPage)
	//{{AFX_MSG_MAP(CBatchAutoSaveProp)
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_AUTOSAVE_GRAPH, OnBnClickedAutosaveBatch)
	ON_BN_CLICKED(IDC_BROWSE_FOLDER, OnBrowseButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchAutoSaveProp message handlers


BOOL CBatchAutoSaveProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	// TODO: Add extra initialization here
	ReadData();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBatchAutoSaveProp::SetGraphSet(CGraphSet * set)
{
	graphSet = set;
}



void CBatchAutoSaveProp::SaveData()
{
	UpdateData();
	//just set graphset members, Edit and Update called by property sheet
	if (graphSet->IsOpen()){
		graphSet->Edit();	
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

		if (str.GetLength() > 0)
		{
			int len = str.GetLength();
			if (str[len-1] != '\\')
				str += "\\";
		}
		graphSet->m_AutoSaveLocation = str;
		graphSet->Update();
	}

	
}

void CBatchAutoSaveProp::ReadData()
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

void CBatchAutoSaveProp::WriteData()
{
			UpdateData();
			if (graphSet->IsOpen()){
				graphSet->Edit();

				//graphSet->m_AutoSave = autoSave;
				CString str;
				Width.GetWindowText(str);
				graphSet->m_AutoSaveWidth = atoi(str);

				Height.GetWindowText(str);
				graphSet->m_AutoSaveHeight = atoi(str);

				graphSet->m_AutoSaveFileType = fileType.GetCurSel();

				Location.GetWindowText(str);
				graphSet->m_AutoSaveLocation = str;
				graphSet->Update();
			}
		
}





/*void CBatchAutoSaveProp::OnBnClickedAutosaveBatch()
{
	// TODO: Add your control notification handler code here
}*/

void CBatchAutoSaveProp::OnBrowseButton(){
// browse for folder location. update Location field.
   UpdateData(TRUE);    
   CDirDialog dirDlg;
    if(dirDlg.DoBrowse()){
		UpdateData(FALSE);
		CString DirName = dirDlg.m_strPath; 
		Location.SetWindowText(DirName);
	} 
}
// ForcastGraphsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "ForcastGraphsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int VTColToVarID(int vtCol);

/////////////////////////////////////////////////////////////////////////////
// CForcastGraphsDialog dialog


CForcastGraphsDialog::CForcastGraphsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CForcastGraphsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CForcastGraphsDialog)
	fd = NULL;
	bReport = FALSE;
	bCloseForecast = FALSE;
	//}}AFX_DATA_INIT
	nVars = 0;
	varIDs = NULL;
	if(pParent)
		fd = (CForecastDialog *)pParent;
	bCloseForecast = FALSE;
}


void CForcastGraphsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CForcastGraphsDialog)
	DDX_Control(pDX, IDC_SELECTEDLIST, selectedList);
	DDX_Control(pDX, IDC_FIELDSLIST, fieldsList);
	DDX_Check(pDX, IDC_REPORT, bReport);
	DDX_Check(pDX, IDC_CLOSEFORECAST, bCloseForecast);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CForcastGraphsDialog, CDialog)
	//{{AFX_MSG_MAP(CForcastGraphsDialog)
	ON_LBN_DBLCLK(IDC_FIELDSLIST, OnDblclkFieldslist)
	ON_LBN_DBLCLK(IDC_SELECTEDLIST, OnDblclkSelectedlist)
	ON_BN_CLICKED(IDC_MOVELEFT, OnMoveleft)
	ON_BN_CLICKED(IDC_MOVERIGHT, OnMoveright)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_REMOVEALL, OnRemoveall)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CForcastGraphsDialog message handlers

BOOL CForcastGraphsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(fd)
	{
		//for(int i = 1; i < fd->varTable.GetColumnSet().GetCount(); i++)
		for(int i = 1; i < fd->m_varGrid.GetNumberCols(); i++)
		{
			//if(fd->varTable.GetColumnSet().GetItem(i).GetHidden() == FALSE)
			if(fd->m_varGrid.GetColWidth(i)> 0)
			{//add to varList
				//int loc = fieldsList.AddString(fd->varTable.GetColumnSet().GetItem(i).GetHeading());
				//int loc = fieldsList.AddString(fd->m_varGrid.GetCell(0, i)->GetText());//.GetColumnSet().GetItem(i).GetHeading());
				int loc = fieldsList.AddString(fd->m_varGrid.QuickGetText(i, -1));//.GetColumnSet().GetItem(i).GetHeading());
				fieldsList.SetItemData(loc, VTColToVarID(i));//atoi(fd->varTable.GetColumnSet().GetItem(i).GetName()));
			}
		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CForcastGraphsDialog::OnOK() 
{
	UpdateData();
	nVars = selectedList.GetCount();
	if(nVars <= 0 && bReport == FALSE)
	{
		AfxMessageBox("ERROR: At least one graph variable or\nGenerate Report must be selected.");
		return;
	}
	if(nVars > 0)
	{
		varIDs = new int[nVars];
		for(int i = 0; i < nVars; i++)
		{
			int val = selectedList.GetItemData(i) - 1;
			varIDs[i] = val;
		}
	}
	CDialog::OnOK();
}

void CForcastGraphsDialog::OnDblclkFieldslist() 
{
	CString str;
	int cur = fieldsList.GetCurSel();
	fieldsList.GetText(cur, str);
	int val = fieldsList.GetItemData(cur);
	int loc = selectedList.AddString(str);
	selectedList.SetItemData(loc, val);
	fieldsList.DeleteString(cur);
}

void CForcastGraphsDialog::OnDblclkSelectedlist() 
{
	CString str;
	int cur = selectedList.GetCurSel();
	selectedList.GetText(cur, str);
	int val = selectedList.GetItemData(cur);
	int loc = fieldsList.AddString(str);
	fieldsList.SetItemData(loc, val);
	selectedList.DeleteString(cur);
}

void CForcastGraphsDialog::OnMoveleft() 
{
	if(selectedList.GetCurSel() != LB_ERR)
		OnDblclkSelectedlist();
}

void CForcastGraphsDialog::OnMoveright() 
{
	if(fieldsList.GetCurSel() != LB_ERR)
		OnDblclkFieldslist();
}

void CForcastGraphsDialog::OnSelectall() 
{
	while(fieldsList.GetCount() > 0)
	{
		fieldsList.SetCurSel(0);
		OnDblclkFieldslist();
	}
}

void CForcastGraphsDialog::OnRemoveall() 
{
	while(selectedList.GetCount() > 0)
	{
		selectedList.SetCurSel(0);
		OnDblclkSelectedlist();
	}	
}

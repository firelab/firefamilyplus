// OverlayDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "wxset.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "OverlayDialog.h"
#include "line.h"
#include "climateSet.h"
#include "SIGStationSet.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"
#include "FFPOverlaysSet.h"
#include "RunSave.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
/////////////////////////////////////////////////////////////////////////////
// COverlayDialog dialog


//COverlayDialog::COverlayDialog(CWnd* pParent /*=NULL*/, CPeriodDoc *_pDoc, int _varID)
//	: CDialog(COverlayDialog::IDD, pParent)
COverlayDialog::COverlayDialog(CWnd* pParent /*=NULL*/, CDocument *pDoc, CClimAnalysis *pAnalysis, int _varID)
	: CDialog(COverlayDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COverlayDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = pDoc;
	m_pAnalysis = pAnalysis;
	varID = _varID;
	m_nID = COverlayDialog::IDD;
	m_Parent = pParent;
}


void COverlayDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COverlayDialog)
	DDX_Control(pDX, IDC_BUTTON2, deleteButton);
	//}}AFX_DATA_MAP

}


BEGIN_MESSAGE_MAP(COverlayDialog, CDialog)
	//{{AFX_MSG_MAP(COverlayDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnNewYear)
	ON_BN_CLICKED(IDC_BUTTON2, OnDeleteYear)
	ON_BN_CLICKED(IDC_APPLY, OnApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COverlayDialog message handlers
BOOL COverlayDialog::OnInitDialog() 
{
	CString str;
	m_grid.pAnalysis = m_pAnalysis;
	m_grid.varID = varID;
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);
	for(int i = 1; i <= 10; i++)
	{
		str.Format("%d", i);
		m_widthsList.Add(str);
	}
	m_lineStyleList.Add("Solid");
	m_lineStyleList.Add("Dash");
	m_lineStyleList.Add("Dot");
	m_lineStyleList.Add("DashDot");
	m_lineStyleList.Add("DashDotDot");
	//add existing years to table
	int count = m_grid.GetNumberRows();
	if(count == 0)
		deleteButton.EnableWindow(false);
	else
		deleteButton.EnableWindow();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void COverlayDialog::OnNewYear() 
{
	// TODO: Add your control notification handler code here
	//if(table.GetRowSet().GetCount() >= OVERLAYS)
	if(m_grid.GetNumberRows() >= OVERLAYS)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Maximum of three overlays.\n\n"));
		AfxMessageBox(strMsg);
		return;
	}
	CUGCell cell;
	CString str;
	COLORREF newColor = RGB(255, 0, 255);
	if(m_grid.GetNumberRows() == 1)
		newColor = RGB(0, 255, 255);
	else if(m_grid.GetNumberRows() == 2)
		newColor = RGB(0, 255, 0);
	str.Format("%d", m_grid.GetNumberRows());
	int row = m_grid.GetNumberRows();
	m_grid.AppendRow();
	str.Format("%d", m_pAnalysis->periods[varID][0]->baseYear);
	m_grid.QuickSetText(0, row, str);
	m_grid.GetCell(1, row, &cell);
	cell.SetBackColor(newColor);
	cell.SetHBackColor(newColor);
	m_grid.SetCell(1, row, &cell);
	m_grid.QuickSetText(2, row, "1");
	m_grid.QuickSetText(3, row, m_lineStyleList.GetAt(0));
	deleteButton.EnableWindow();
	m_grid.RedrawAll();
}

void COverlayDialog::OnDeleteYear() 
{
	if(m_grid.GetNumberRows() > 0)
	{
		int row = m_grid.GetCurrentRow();
		if(row >= 0 && row < m_grid.GetNumberRows())
		{
			m_grid.DeleteRow(row);
		}
	}
	m_grid.RedrawAll();
}

void COverlayDialog::OnOK() 
{
	//replace overlay values for document
	OnApply();
	m_pAnalysis->m_opts.m_OverlayDialog = NULL;
	DestroyWindow();
}

void COverlayDialog::OnCancel() 
{
	m_pAnalysis->m_opts.m_OverlayDialog = NULL;
	DestroyWindow();
}

void COverlayDialog::OnApply() 
{
	int yr;//, clr;
	CString str;
	CUGCell cell;
	int i;
	CRunSave runSave(m_pAnalysis->fpSet);
	CFFPOverlaysSet olSet(m_pAnalysis->m_opts.pDB);
	olSet.m_strFilter.Format("[runID] = %d", m_pAnalysis->m_runID);//fpSet->m_SIG_Station);
	olSet.Open();
	if(runSave.m_saveIsOn)
	{
		while(!olSet.IsEOF())
		{
			olSet.Delete();
			olSet.MoveNext();
		}
	}
	for(i = 0; i < m_grid.GetNumberRows(); i++)
	{
		olSet.AddNew();
		olSet.m_runID = m_pAnalysis->m_runID;
		yr = atoi(m_grid.QuickGetText(0, i));
		olSet.m_Year = theApp.overLayYears[i] = m_pAnalysis->m_opts.overLayYears[i] = yr;
		m_grid.GetCell(1, i, &cell);
		olSet.m_Color = theApp.overLayColors[i] = m_pAnalysis->m_opts.overLayColors[i] = cell.GetBackColor();
		yr = atoi(m_grid.QuickGetText(2, i));
		olSet.m_Width = theApp.lineWidths[i] = m_pAnalysis->m_opts.lineWidths[i] = (int)yr;
		str = m_grid.QuickGetText(3, i);
		olSet.m_Style = theApp.lineStyles[i] = m_pAnalysis->m_opts.lineStyles[i] = StringToDashLineStyle(str);	
		if(runSave.m_saveIsOn)
			olSet.Update();
	}
	while(i < OVERLAYS)
	{
		olSet.AddNew();
		olSet.m_runID = m_pAnalysis->m_runID;
		olSet.m_Year = theApp.overLayYears[i] = m_pAnalysis->m_opts.overLayYears[i] = 0;
		if(runSave.m_saveIsOn)
			olSet.Update();
		i++;
	}
	//now the 'general overlay options
	olSet.m_strFilter = "[runID] = 0";
	olSet.Requery();
	while(!olSet.IsEOF())
	{
		olSet.Delete();
		olSet.MoveNext();
	}
	for(i = 0; i < OVERLAYS; i++)
	{
		olSet.AddNew();
		olSet.m_runID = 0;
		//yr = atoi(m_grid.QuickGetText(0, i));
		olSet.m_Year = theApp.overLayYears[i];// = m_pAnalysis->m_opts.overLayYears[i] = yr;
		//m_grid.GetCell(1, i, &cell);
		olSet.m_Color = theApp.overLayColors[i];// = m_pAnalysis->m_opts.overLayColors[i] = cell.GetBackColor();
		//yr = atoi(m_grid.QuickGetText(2, i));
		olSet.m_Width = theApp.lineWidths[i];// = m_pAnalysis->m_opts.lineWidths[i] = (int)yr;
		//str = m_grid.QuickGetText(3, i);
		olSet.m_Style = theApp.lineStyles[i];// = m_pAnalysis->m_opts.lineStyles[i] = StringToDashLineStyle(str);	
		olSet.Update();
	}
	/*while(i < OVERLAYS)
	{
		olSet.AddNew();
		olSet.m_SIG_Station = "######";
		olSet.m_Year = theApp.overLayYears[i];// = m_pAnalysis->m_opts.overLayYears[i] = 0;
		olSet.Update();
		i++;
	}*/
	olSet.Close();
	m_pDoc->UpdateAllViews(NULL);
}


void COverlayDialog::PostNcDestroy() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_pAnalysis->m_opts.m_OverlayDialog = NULL;
	
	delete this;
}


bool COverlayDialog::Create()
{
	int ret = CDialog::Create(m_nID, m_Parent);
	if(ret == 0)
		return false;
	return true;
}

int StringToLineStyle(CString str)
{
	if(str == "Dash")
		return PS_DASH;
	else if(str == "Dot")
		return PS_DOT;
	else if(str == "DashDot")
		return PS_DASHDOT;
	else if(str == "DashDotDot")
		return PS_DASHDOTDOT;
	return PS_SOLID;
}

int StringToDashLineStyle(CString str)
{
	if(str == "Dash")
		return CDashLine::DL_DASH;
	else if(str == "Dot")
		return CDashLine::DL_DOT;
	else if(str == "DashDot")
		return CDashLine::DL_DASHDOT;
	else if(str == "DashDotDot")
		return CDashLine::DL_DASHDOTDOT;
	return CDashLine::DL_SOLID;
}

CString LineStyleToString(int lineStyle)
{
	if(lineStyle == CDashLine::DL_DASH)//PS_DASH)
		return "Dash";
	else if(lineStyle == CDashLine::DL_DOT)
		return "Dot";
	else if(lineStyle == CDashLine::DL_DASHDOT)
		return "DashDot";
	else if(lineStyle == CDashLine::DL_DASHDOTDOT)
		return "DashDotDot";
	return "Solid";
}




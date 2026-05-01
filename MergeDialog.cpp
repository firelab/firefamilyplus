// MergeDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "wxset.h"
//#include "OverlayDialog.h"
#include "FireDay.h"
#include "PeriodDoc.h"
#include "MergeDialog.h"
#include "SplitterFrame.h"
//#include "GraphView.h"
//#include "fireplusSet.h"
//#include "fireplusDoc.h"
//#include "fireplusView.h"
//#include "climateset.h"
#include "MergedFrame.h"
#include "MergedView.h"
#include "UserVarSet.h"
#include "ClimateSet.h"
//#include "GridCellCombo.h"
//#include "GridCellColor.h"
#include "climateSet.h"
#include "SIGStationSet.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
//extern char *abbrevs[];
/*char *merge_abbrevs[STATICVARIDS] = 
{   "Temp", "AvgT", "MinT", "MaxT", "RH", "AvgRH", "MinRH", "MaxRH", "PptAmt", "PptDur", "Wind", 
	"SC", "ERC", "BI", "KBDI", "IC", "FM1", "FM10", "FM100", "FM1000", "FMHerb", "FMWood", 
	"X1000", "WDIR", "SOW", "FFMC", "DMC", "DC", "ISI", "BUI", "FWI", "DSR" };
*/

// Standard MergeCUG construction/destruction
MergeCUG::MergeCUG()
{
}

MergeCUG::~MergeCUG()
{
}

/////////////////////////////////////////////////////////////////////////////
//	OnSetup
//		This function is called just after the grid window 
//		is created or attached to a dialog item.
//		It can be used to initially setup the grid
void MergeCUG::OnSetup()
{
	CRect rect;
	GetClientRect(&rect);
	m_nButtonIndex = AddCellType(&m_button);
	SetCurrentCellMode(2);
	SetNumberCols(4);
	QuickSetText(0, -1, "Variable"); 
	QuickSetText(1, -1, "Color");
	QuickSetText(2, -1, "Width");
	QuickSetText(3, -1, "Line Style");
	int wid = rect.Width() / 13;
	SetColWidth(-1, wid);
	SetColWidth(0, wid * 4);
	SetColWidth(1, wid * 2);
	SetColWidth(2, wid * 2);
	SetColWidth(3, wid * 4);
	((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );
	CUGCell cell;
	GetColDefault( 0, &cell );
	cell.SetCellType(UGCT_DROPLIST); 
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText(pDlg->m_varsString);
	SetColDefault( 0, &cell );
	GetColDefault( 1, &cell );
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	SetColDefault( 1, &cell );
	CString widsStr = "1\n2\n3\n4\n5\n6\n7\n8\n9\n10\n";
	GetColDefault( 2, &cell );
	cell.SetCellType(UGCT_DROPLIST); 
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText(widsStr);
	SetColDefault( 2, &cell );
	CString styleStr = "Solid\nDash\nDot\nDashDot\nDashDotDot\n";
	GetColDefault( 3, &cell );
	cell.SetCellType(UGCT_DROPLIST); 
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText(styleStr);
	SetColDefault( 3, &cell );

	
}

int MergeCUG::OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param)
{
	//UNREFERENCED_PARAMETER(ID);
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(msg);
	UNREFERENCED_PARAMETER(param);
	if(ID == m_nButtonIndex && col == 1 && msg == UGCT_BUTTONCLICK)
	{
		CUGCell cell;
		GetCell(col,row,&cell);
		COLORREF color = cell.GetBackColor();
		CColorDialog cd(color);
		if(cd.DoModal() == IDOK)
		{
			//table.GetCellSet().GetItem(Row, Column).SetBackColor(cd.GetColor());
			cell.SetBackColor(cd.GetColor());
			cell.SetHBackColor(cd.GetColor());
			SetCell(col, row, &cell);
		}

		//return OnPushButton( ID, col, row, msg, param);
	}
	return TRUE;
}

void MergeCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	//DrawExcelFocusRect(dc,rect);
	
	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

COLORREF MergeCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

/////////////////////////////////////////////////////////////////////////////
// CMergeDialog dialog


CMergeDialog::CMergeDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMergeDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMergeDialog)
	//}}AFX_DATA_INIT
	pDoc = NULL;
	parent = pParent;
}


void CMergeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMergeDialog)
	DDX_Control(pDX, IDC_LIST1, yrsList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMergeDialog, CDialog)
	//{{AFX_MSG_MAP(CMergeDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMergeDialog message handlers

BOOL CMergeDialog::OnInitDialog() 
{
	m_grid.pDlg = this;
	int firstV = -1;
	CString temp;
	if(pDoc)
	{
		m_varsString = "(none)\n";
		int uVars = pDoc->m_pAnalysis->HasUserVars();
		CClimateSet climSet(pDoc->m_pAnalysis->m_opts.pDB);
		climSet.Open();
		CUserVarSet uvs(pDoc->m_pAnalysis->m_opts.pDB);
		uvs.Open();
		for(int v = 0; v < MAXVARIDS; v++)
		{
			if(pDoc->m_pAnalysis->periods[v])// && v != hotVar)
			{
				if(v < STATICVARIDS)
				{
					if(firstV == -1)
						firstV = v;
					climSet.m_strFilter.Format("[VarID] = %d", v + 1);
					climSet.Requery();
					if(!climSet.IsBOF() && !climSet.IsEOF())
					{
						temp.Format("%s\n", climSet.m_ShortName);
						m_varsString += temp;
					}
				}
				else
				{
					if(firstV == -1)
						firstV = v;
					uvs.m_strFilter.Format("[VarID] = %d", v + 1);
					uvs.Requery();
					if(!uvs.IsBOF() && !uvs.IsEOF())
					{
						temp.Format("%s\n", uvs.m_Abbrev);
						m_varsString += temp;
					}
				}
			}
		}
		uvs.Close();
		climSet.Close();
	}
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);
	CString str;

		//ok, now need the years....
		int loc = yrsList.AddString("Average");
		if(loc != CB_ERR)
		{
			yrsList.SetItemData(loc, 0);
		//	if(parent->IsKindOf( RUNTIME_CLASS( CSplitterFrame ) ) )
		//		yrsList.SetSel(loc);
		}
		for(int y = 0; y < pDoc->m_pAnalysis->periods[firstV][0]->years; y++)
		{
			temp.Format("%d", pDoc->m_pAnalysis->periods[firstV][0]->baseYear + y);
			loc = yrsList.AddString(temp);
			yrsList.SetItemData(loc, pDoc->m_pAnalysis->periods[firstV][0]->baseYear + y);
		}
		if(pDoc->m_pAnalysis->m_opts.tPeriods)//aux years too...
		{
			int yr = 0;
			while(pDoc->m_pAnalysis->m_opts.tPeriods[firstV][0]->baseYear + yr <  pDoc->m_pAnalysis->periods[firstV][0]->baseYear)
			{
				temp.Format("%d", pDoc->m_pAnalysis->m_opts.tPeriods[firstV][0]->baseYear + yr);
				loc = yrsList.AddString(temp);
				yrsList.SetItemData(loc, pDoc->m_pAnalysis->m_opts.tPeriods[firstV][0]->baseYear + yr);
				yr++;
			}
			int i = pDoc->m_pAnalysis->periods[firstV][0]->years;
			yr = pDoc->m_pAnalysis->periods[firstV][0]->baseYear - pDoc->m_pAnalysis->m_opts.tPeriods[firstV][0]->baseYear + i;
			while(yr <  pDoc->m_pAnalysis->m_opts.tPeriods[firstV][0]->years)
			{
				temp.Format("%d", pDoc->m_pAnalysis->m_opts.tPeriods[firstV][0]->baseYear + yr);
				loc = yrsList.AddString(temp);
				yrsList.SetItemData(loc, pDoc->m_pAnalysis->m_opts.tPeriods[firstV][0]->baseYear + yr);
				yr++;
			}
		}
		//check for existing mergedviews to get years from
		CMDIFrameWnd *mainFrame = (CMDIFrameWnd *)AfxGetMainWnd();
		CWnd *child = mainFrame->MDIGetActive();//, *topMost = NULL; 
		while(child)
		{
			CRuntimeClass* prt = child->GetRuntimeClass();
			if(strcmp( prt->m_lpszClassName, "CMergedFrame" ) == 0)
			{//select frame's view's year in yrsList
				if(((CMergedView *)((CMergedFrame *)child)->GetActiveView())->GetDocument() == pDoc)
				{
					CString yrStr;
					int trgYr = ((CMergedFrame *)child)->year;
					if(trgYr <= 0)
						yrStr = "Average";
					else
						yrStr.Format("%d", trgYr);
					loc = yrsList.FindStringExact(-1, yrStr);
					if(loc != CB_ERR)
						yrsList.SetSel(loc);
				}
			}
			child = child->GetNextWindow();
		}
		//if(parent->IsKindOf( RUNTIME_CLASS( CMergedFrame ) ) )
		//{
			/*if(((CMergedFrame *)parent)->year <= 0)
			//{
				loc = yrsList.FindStringExact(-1, "Average");
			//}
			else
			{
				CString yrStr;
				yrStr.Format("%d", ((CMergedFrame *)parent)->year);
				loc = yrsList.FindStringExact(-1, yrStr);
			}
			if(loc != CB_ERR)
				yrsList.SetSel(loc);*/
	//	}
		FillTable();
	//}
	//yrsList.SetTopIndex(yrsList.GetCount() - 1);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMergeDialog::FillTable()
{
	if(!parent)
	{
		AfxMessageBox("Oooops no parent CWnd for CMergeDialog....");
		return;
	}
	CUGCell cell;
	if(parent->IsKindOf( RUNTIME_CLASS( CSplitterFrame ) ) )
	{//new graph to be created
		CString str;
		CGraphView *pView = (CGraphView *) ((CSplitterFrame *)parent)->m_wndSplitter.GetPane(0, 0);
//		int row;
		for(int i = 0; i < MAXMERGES; i++)
		{
			str.Format("%d", i + 1);
			m_grid.AppendRow();
			m_grid.QuickSetText(-1, i, str);
			if(pDoc->m_pAnalysis->m_opts.mergedIDs[i] >= 0)
				str = pDoc->m_pAnalysis->varNames[pDoc->m_pAnalysis->m_opts.mergedIDs[i]];
			else if(i == 0)
				str = pDoc->m_pAnalysis->varNames[pView->m_graphOpts.varID];
			else
				str = "(none)";
			m_grid.QuickSetText(0, i, str);
			m_grid.GetCell(1, i, &cell);
			cell.SetBackColor(theApp.mergeColors[i]);
			cell.SetHBackColor(theApp.mergeColors[i]);
			m_grid.SetCell(1, i, &cell);
			str.Format("%d", theApp.mergeWidths[i]);
			m_grid.QuickSetText(2, i, str);
			m_grid.QuickSetText(3, i, LineStyleToString(theApp.mergeStyles[i]));
		}
		//set Variable selections to last selections made....
	}
	else if(parent->IsKindOf( RUNTIME_CLASS( CMergedFrame ) ) )
	{
		//existing view attached to the frame
		CMergedView *pView = (CMergedView *)((CMergedFrame *)parent)->GetActiveView();
		CString str;
		for(int i = 0; i < MAXMERGES; i++)
		{
			str.Format("%d", i + 1);
			m_grid.AppendRow();
			m_grid.QuickSetText(-1, i, str);
			if(pView->vars[i] != -1)
			{
				str = pDoc->m_pAnalysis->varNames[pView->vars[i]];
				//str.TrimLeft();
				//str.TrimRight();
			}
			else
				str = "(none)";
			m_grid.QuickSetText(0, i, str);
			m_grid.GetCell(1, i, &cell);
			cell.SetBackColor(pView->colors[i]);
			cell.SetHBackColor(pView->colors[i]);
			m_grid.SetCell(1, i, &cell);
			str.Format("%d", pView->widths[i]);
			m_grid.QuickSetText(2, i, str);
			m_grid.QuickSetText(3, i, LineStyleToString(pView->styles[i]));
		}

	}
	else
	{
		AfxMessageBox("Oooops unrecognized parent CWnd for CMergeDialog....");
		//return;
	}
}

void CMergeDialog::OnOK() 
{
	CString str;
	//make sure something was selected
	bool hasVar = false;
	for(int i = 0; i < m_grid.GetNumberRows(); i++)
	{
		str = m_grid.QuickGetText(0, i);
		if(str.GetLength() > 0 && str != "(none)")
		{
			hasVar = true;
			break;
		}
	}
	if(!hasVar)
	{
		AfxMessageBox("Error: No variables selected to graph.");
		return;
	}
	if(yrsList.GetSelCount() <= 0)
	{
		AfxMessageBox("ERROR: Must select a year or averages to graph.");
		return;
	}
	//get yMax's for selected vars....
	CWaitCursor wait;
	double yMax[MAXMERGES];
	double yMin[MAXMERGES];
	int varIDs[MAXMERGES];
	int v;
	for(v = 0; v < m_grid.GetNumberRows(); v++)
	{
		str = m_grid.QuickGetText(0, v);
		if(str.GetLength() > 0 && str != "(none)")//add a var to graph
			varIDs[v] = pDoc->m_pAnalysis->AbbrevToVarID(str);
		else
			varIDs[v] = -1;
	}
	for(v = 0; v < MAXMERGES; v++)
	{
		if(varIDs[v] >= 0 && varIDs[v] != 8)
		{
			yMax[v] = 0.1;
			yMin[v] = 0.0;
			for(int p = 0; p < pDoc->m_pAnalysis->m_opts.numPeriods; p++)
			{
				if(pDoc->m_pAnalysis->periods[varIDs[v]][p]->N() > 0)
				{
					yMax[v] = max(yMax[v], pDoc->m_pAnalysis->periods[varIDs[v]][p]->max);
					yMin[v] = min(yMin[v], pDoc->m_pAnalysis->periods[varIDs[v]][p]->min);
				}
				if(pDoc->m_pAnalysis->m_opts.tPeriods)
				{
					if(pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][p]->N() > 0)
					{
						yMax[v] = max(yMax[v], pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][p]->max );
						yMin[v] = min(yMin[v], pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][p]->min );
					}
				}
			}
		}
		else if(varIDs[v] == 8)//grrr dang precip
		{
			yMax[v] = 0.1;
			yMin[v] = 0.0;
			Period **pcpSums = new Period *[pDoc->m_pAnalysis->m_opts.numPeriods], **pcpAuxSums = NULL;
			int p;
			for(p = 0; p < pDoc->m_pAnalysis->m_opts.numPeriods; p++)
			{
				pcpSums[p] = new Period(pDoc->m_pAnalysis->periods[varIDs[v]][p]->start, pDoc->m_pAnalysis->periods[varIDs[v]][p]->end);
			}
			if(pDoc->m_pAnalysis->m_opts.numtPeriods > 0 && pDoc->m_pAnalysis->m_opts.tPeriods)
			{
				pcpAuxSums = new Period *[pDoc->m_pAnalysis->m_opts.numtPeriods];
				for(int a = 0; a < pDoc->m_pAnalysis->m_opts.numtPeriods; a++)
					pcpAuxSums[a] = new Period(pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][a]->start, pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][a]->end);
			}
			//accumulate sums
			long minPrecipObs = 0;//GetMinPrecipObs(pDoc->m_pAnalysis->m_opts., COleDateTime end);

			for(p = 0; p < pDoc->m_pAnalysis->m_opts.numPeriods; p++)
			{
				minPrecipObs = GetMinPrecipObs(pDoc->m_pAnalysis->periods[varIDs[v]][p]->start, pDoc->m_pAnalysis->periods[varIDs[v]][p]->end);
				for(int y = 0; y < pDoc->m_pAnalysis->periods[varIDs[v]][p]->years; y++)
				{
					if(pDoc->m_pAnalysis->periods[varIDs[v]][p]->yStats[y].N() >= minPrecipObs)
					{
						COleDateTime pDate;
						pDate.SetDate(pDoc->m_pAnalysis->periods[varIDs[v]][p]->start.GetYear() + y, pDoc->m_pAnalysis->periods[varIDs[v]][p]->start.GetMonth(),
							pDoc->m_pAnalysis->periods[varIDs[v]][p]->start.GetDay());
						pcpSums[p]->Accumulate(pDate, pDoc->m_pAnalysis->periods[varIDs[v]][p]->yStats[y].Mean() 
							* pDoc->m_pAnalysis->periods[varIDs[v]][p]->yStats[y].N());
					}
				}
				pcpSums[p]->Complete();
				yMax[v] = max(yMax[v], pcpSums[p]->max);
				delete pcpSums[p];
			}
			//accumulate auxSums
			for(p = 0; p < pDoc->m_pAnalysis->m_opts.numtPeriods; p++)
			{
				minPrecipObs = GetMinPrecipObs(pDoc->m_pAnalysis->periods[varIDs[v]][p]->start, pDoc->m_pAnalysis->periods[varIDs[v]][p]->end);
				for(int y = 0; y < pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][p]->years; y++)
				{
					if(pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][p]->yStats[y].N() >= minPrecipObs)
					{
						COleDateTime pDate;
						pDate.SetDate(pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][p]->start.GetYear() + y, pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][p]->start.GetMonth(),
							pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][p]->start.GetDay());
						pcpAuxSums[p]->Accumulate(pDate, pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][p]->yStats[y].Mean() 
							* pDoc->m_pAnalysis->m_opts.tPeriods[varIDs[v]][p]->yStats[y].N());
					}
				}
				pcpAuxSums[p]->Complete();
				yMax[v] = max(yMax[v], pcpAuxSums[p]->max);
				delete pcpAuxSums[p];
			}
			if(pcpSums)
				delete[] pcpSums;
			if(pcpAuxSums)
				delete[] pcpAuxSums;
		}
	}
	//delete any existing merged views.....(grrrrr)
	CMDIFrameWnd *mainFrame = (CMDIFrameWnd *)AfxGetMainWnd();
	CWnd *child, *next, *safety; 
	safety = mainFrame->MDIGetActive();//, *topMost = NULL; 
	child = next = safety->GetNextWindow();
	while(next)
	{
		child = next;
		next = child->GetNextWindow();
		CRuntimeClass* prt = child->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CMergedFrame" ) == 0)
		{//close the frame
			if(((CMergedView *)((CMergedFrame *)child)->GetActiveView())->GetDocument() == pDoc)
				child->DestroyWindow();
		}
	}

	//ok...create new graphs
	//if(parent->IsKindOf( RUNTIME_CLASS( CSplitterFrame ) ) )
	//{//new one
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();
	CUGCell cell;
	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			theApp.GetNextDocTemplate(curTemplatePos);
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("MergedView"))
		{
			int nCount = yrsList.GetSelCount();
			CArray<int,int> aryListBoxSel;
			
			aryListBoxSel.SetSize(nCount);
			yrsList.GetSelItems(nCount, aryListBoxSel.GetData()); 
			for(int y = 0; y < nCount; y++)
			{
				int loc = 0;
				CMergedFrame* pFrame = (CMergedFrame*)curTemplate->CreateNewFrame(pDoc, NULL);
				int yrLoc = aryListBoxSel[y];//yrsList.GetCurSel();
				pFrame->year = yrsList.GetItemData(yrLoc);
				CString tTitle, yrTmp;
				if(pFrame->year <= 0)
					yrTmp = "Average";
				else
					yrTmp.Format("%d", pFrame->year);
				tTitle.Format("%s - %s - ", pDoc->m_pAnalysis->m_opts.stationName, yrTmp);//, abbrevs[parent->varID]);//, climSet->m_Variable_Name);
				//for(int i = 0; i < table.GetRowSet().GetCount(); i++)
				for(int i = 0; i < m_grid.GetNumberRows(); i++)
				{
					pDoc->m_pAnalysis->m_opts.mergedIDs[i] = pFrame->vars[i] = -1;	
					pFrame->yMax[i] = yMax[i];
					pFrame->yMin[i] = yMin[i];
					int wid;
					str = m_grid.QuickGetText(0, i);
					if(str.GetLength() > 0 && str != "(none)")//add a var to graph
					{
						if(loc != 0)
							tTitle += " + ";
						tTitle += str;
						pDoc->m_pAnalysis->m_opts.mergedIDs[i] = pFrame->vars[i] = pDoc->m_pAnalysis->AbbrevToVarID(str);
						loc++;
					}
					m_grid.GetCell(1, i, &cell);
					theApp.mergeColors[i] = cell.GetBackColor();//pColorCell->GetColor();//table.GetCellSet().GetItem(i, 1).GetBackColor();
					wid = atoi(m_grid.QuickGetText(2, i));
					theApp.mergeWidths[i] = (int)wid;
					str = m_grid.QuickGetText(3, i);
					theApp.mergeStyles[i] = StringToDashLineStyle(str);
				}
				curTemplate->InitialUpdateFrame(pFrame, pDoc);
				pFrame->SetWindowText(tTitle);
			}
			break;
		}
	}
	//get rid of safety window if a mergedframe grrrrrrr
	if(safety)
	{
		CRuntimeClass* prt = safety->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CMergedFrame" ) == 0)
			safety->DestroyWindow();
	}
	CDialog::OnOK();
//	}
/*	else if(parent->IsKindOf( RUNTIME_CLASS( CMergedFrame ) ) )
	{
		//reset the views contents to new settings
		int loc = 0;
		CString tTitle;
		tTitle.Format("%s - ", pDoc->m_pAnalysis->m_opts.stationName);//, abbrevs[parent->varID]);//, climSet->m_Variable_Name);
		CMergedFrame* pFrame = (CMergedFrame*)parent;
		for(int i = 0; i < table.GetRowSet().GetCount(); i++)
		{
			pFrame->vars[i] = -1;
			CDTData RowData = table.GetRowSet().GetItem(i).GetValue();
			RowData.GetItem(str, 0);
			short wid;
			if(str.GetLength() > 0 && str != "(none)")//add a var to graph
			{
				if(loc != 0)
					tTitle += " + ";
				tTitle += str;
				pFrame->vars[i] = AbbrevToVarID(str);
				loc++;
			}
			theApp.mergeColors[i] = table.GetCellSet().GetItem(i, 1).GetBackColor();
			RowData.GetItem(&wid, 2);
			theApp.mergeWidths[i] = (int)wid;
			RowData.GetItem(str, 3);
			theApp.mergeStyles[i] = StringToDashLineStyle(str);
		}
		int yrLoc = yrsList.GetCurSel();
		pFrame->year = yrsList.GetItemData(yrLoc);
		pFrame->ReconfigureView();
		pFrame->SetWindowText(tTitle);
		CDialog::OnOK();
		return;
		
	}*/
}

//BEGIN_EVENTSINK_MAP(CMergeDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CMergeDialog)
	//ON_EVENT(CMergeDialog, IDC_DATATABLEDB1, 50 /* LfClick */, OnLfClickDatatabledb1, VTS_I2 VTS_I4 VTS_I4 VTS_I4 VTS_PI4)
	//}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

/*void CMergeDialog::OnLfClickDatatabledb1(short Pane, long Row, long Column, long ClickPos, long FAR* Status) 
{
	if(Column == 1 && ClickPos == posOnCell)
	{
		CColorDialog cd(table.GetCellSet().GetItem(Row, Column).GetBackColor());
		if(cd.DoModal() == IDOK)
			table.GetCellSet().GetItem(Row, Column).SetBackColor(cd.GetColor());
	}
}*/


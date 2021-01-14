// PProbDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "SIGStationSet.h"
#include "ClimateSet.h"
#include "wxSet.h"
#include "fireplusSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "splitterframe.h"
#include "FireSumSet.h"
#include "climAnalysis.h"
#include "PProbDialog.h"
#include "fireplusView.h"
#include "ClimateSet.h"
#include "FFPOptsSet.h"
#include "RunSave.h"
#include "MainFrm.h"

extern int curr_runID;
extern CFireplusApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

PProbCUG::PProbCUG()
{
}

PProbCUG::~PProbCUG()
{
}

void PProbCUG::OnSetup()
{
	CUGCell cell;
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(3);
	SetNumberRows(6);
	QuickSetText(0, -1, "Class");
	QuickSetText(1, -1, "Min");
	QuickSetText(2, -1, "Max");
	int wid = rect.Width() / 10;
	SetColWidth(-1, 0);
	SetColWidth(0, 3 * wid);
	SetColWidth(1, 3 * wid);
	SetColWidth(2, 3 * wid);
	GetColDefault(0, &cell);
	cell.SetReadOnly(TRUE);
	SetColDefault(0,&cell);
	GetColDefault(1, &cell);
	cell.SetNumberDecimals(2);
	SetColDefault(1,&cell);
	GetColDefault(2, &cell);
	cell.SetNumberDecimals(2);
	SetColDefault(2,&cell);
	QuickSetText(0, 0, "1");
	QuickSetText(0, 1, "2");
	QuickSetText(0, 2, "3");
	QuickSetText(0, 3, "4");
	QuickSetText(0, 4, "5");
	QuickSetText(0, 5, "6");
	((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );
}

int PProbCUG::OnEditStart(int col, long row,CWnd **edit)
{
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(**edit);
	CUGCell cell;
	GetCellIndirect( col, row, &cell );
	DWORD dwStyle = 0;
	if ( cell.GetParam() == USE_COXNUMBER )
	{
		if ( ! m_cutNumeric.GetSafeHwnd())
		{
			m_cutNumeric.Create( WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, COXMASK_ID );
			m_cutNumeric.m_ctrl = this;
		}
		m_cutNumeric.SetDecimalSeparator( '.' );
		//m_cutNumeric.SetGroupSeparator( ',' );
		m_cutNumeric.SetGroupLength(0);
		m_cutNumeric.SetDecimalDigitCount(-1);
		m_cutNumeric.SetShowLeadingZero(FALSE);
		m_cutNumeric.SetFractionalDigitCount(2);
		*edit = &m_cutNumeric;
	}
	return TRUE;
}

void PProbCUG::OnKeyDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);

	if ( *vcKey == VK_TAB )
	{
		int col = GetCurrentCol();
		long row = GetCurrentRow();

		if ( GetKeyState( VK_SHIFT ) < 0 )
		{
			col --;
			if ( col < 0 )
			{
				col = GetNumberCols() - 1;
				row--;
			}
		}
		else
		{
			col ++;
			if ( col >= GetNumberCols())
			{
				col = 0;
				row++;
			}
		}
		if(row < 0)
			row = GetNumberRows() - 1;
		if(row >= GetNumberRows())
			row = 0;

		GotoCell( col, row );
	}
}

void PProbCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void PProbCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey );
}

COLORREF PProbCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);
}

/////////////////////////////////////////////////////////////////////////////
// CPProbDialog dialog

CPProbDialog::CPProbDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet /*=NULL*/)
	: CDialog(CPProbDialog::IDD, pParent), climSet(_fpSet->m_pDatabase), binSet(_fpSet->m_pDatabase)
{
	//{{AFX_DATA_INIT(CPProbDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	fpSet = _fpSet;
	hasData = false;
}

void CPProbDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPProbDialog)
	DDX_Control(pDX, IDC_COMBO1, indexList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPProbDialog, CDialog)
	//{{AFX_MSG_MAP(CPProbDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeIndex)
	ON_COMMAND(ID_VARSORT, OnVarSort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPProbDialog message handlers

BOOL CPProbDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);

	// TODO: Add extra initialization here
	if(fpSet)
	{
		try
		{    // requery recordset
			binSet.Open();
			climSet.m_strFilter = "[OptionType] >= 0";
			climSet.setVarSortOrder(theApp.varSortOrder);
			climSet.Open();
			if (climSet.IsOpen())
			{
				while (!climSet.IsEOF())
				{
					CString str = climSet.m_Variable_Name;
					
					
				    int eLoc = indexList.AddString(str);
					indexList.SetItemData(eLoc, climSet.m_VarID - 1);
					climSet.MoveNext();
				}
			}
			climSet.Close();
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPProbDialog::OnSelchangeIndex()
{
	if(hasData)
		SaveTable();
	int varID = (int) indexList.GetItemData(indexList.GetCurSel());
	if(varID != CB_ERR)
	{
		try
		{
			varID++;
			binSet.m_strFilter.Format("[VarID] = %d", varID);
			binSet.Requery();
			ConfigureTable();
			m_grid.RedrawAll();
			hasData = true;
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
			hasData = false;
		}
	}
	else
	{
		m_grid.RedrawAll();
		hasData = false;
	}
}

void CPProbDialog::ConfigureTable()
{
	CUGCell cell;
	m_grid.GetCell(1, 0, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin1Min);
	m_grid.SetCell(1, 0, &cell);
	m_grid.GetCell(2, 0, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin1Max);
	m_grid.SetCell(2, 0, &cell);
	m_grid.QuickSetText(0, 1, "2");
	m_grid.GetCell(1, 1, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin2Min);
	m_grid.SetCell(1, 1, &cell);
	m_grid.GetCell(2, 1, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin2Max);
	m_grid.SetCell(2, 1, &cell);
	m_grid.QuickSetText(0, 2, "3");
	m_grid.GetCell(1, 2, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin3Min);
	m_grid.SetCell(1, 2, &cell);
	m_grid.GetCell(2, 2, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin3Max);
	m_grid.SetCell(2, 2, &cell);
	m_grid.QuickSetText(0, 3, "4");
	m_grid.GetCell(1, 3, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin4Min);
	m_grid.SetCell(1, 3, &cell);
	m_grid.GetCell(2, 3, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin4Max);
	m_grid.SetCell(2, 3, &cell);
	m_grid.QuickSetText(0, 4, "5");
	m_grid.GetCell(1, 4, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin5Min);
	m_grid.SetCell(1, 4, &cell);
	m_grid.GetCell(2, 4, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin5Max);
	m_grid.SetCell(2, 4, &cell);
	m_grid.QuickSetText(0, 5, "6");
	m_grid.GetCell(1, 5, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin6Min);
	m_grid.SetCell(1, 5, &cell);
	m_grid.GetCell(2, 5, &cell);
	cell.SetNumberDecimals(2);
	cell.SetNumber(binSet.m_Bin6Max);
	m_grid.SetCell(2, 5, &cell);
}

void CPProbDialog::SaveTable()
{
	if(binSet.IsOpen() && m_grid.GetNumberRows() > 0 && hasData)
	{
		try
		{    // requery recordset
			CUGCell cell;
			binSet.Edit();
			//put data in appropriate slots
			m_grid.GetCell(1, 0, &cell);
			binSet.m_Bin1Min = cell.GetNumber();
			m_grid.GetCell(2, 0, &cell);
			binSet.m_Bin1Max = cell.GetNumber();
			m_grid.GetCell(1, 1, &cell);
			binSet.m_Bin2Min = cell.GetNumber();
			m_grid.GetCell(2, 1, &cell);
			binSet.m_Bin2Max = cell.GetNumber();
			m_grid.GetCell(1, 2, &cell);
			binSet.m_Bin3Min = cell.GetNumber();
			m_grid.GetCell(2, 2, &cell);
			binSet.m_Bin3Max = cell.GetNumber();
			m_grid.GetCell(1, 3, &cell);
			binSet.m_Bin4Min = cell.GetNumber();
			m_grid.GetCell(2, 3, &cell);
			binSet.m_Bin4Max = cell.GetNumber();
			m_grid.GetCell(1, 4, &cell);
			binSet.m_Bin5Min = cell.GetNumber();
			m_grid.GetCell(2, 4, &cell);
			binSet.m_Bin5Max = cell.GetNumber();
			m_grid.GetCell(1, 5, &cell);
			binSet.m_Bin6Min = cell.GetNumber();
			m_grid.GetCell(2, 5, &cell);
			binSet.m_Bin6Max = cell.GetNumber();
			//write the data
			binSet.Update();

			CString value1;
			value1.Format("%2d", indexList.GetItemData(indexList.GetCurSel()) + 1);

			CClimateSet cSet(fpSet->m_pDatabase);
			cSet.m_strFilter.Format("[VarID]=%s",value1);
			CString runDesc;
			runDesc.Format("Persistence Prob.: ");
			cSet.Open();
			if (cSet.IsOpen())
			{
				runDesc.Append(cSet.m_ShortName);
			}
			cSet.Close();
            // added for batch
			// single run mode
			CRunSave saveRun(fpSet);
			saveRun.SaveGeneral(0, runDesc, fpSet);
			saveRun.SaveClimateOptions(curr_runID, fpSet);
			// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
			saveRun.SaveFireOptions(curr_runID, fpSet);
	        // save variable

			saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_PERSISTENCE_PROB,0,"varID",value1,fpSet);

			// save bin values

			value1.Format("%6.2f", binSet.m_Bin1Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin1Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin1Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin1Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin2Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin2Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin2Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin2Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin3Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin3Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin3Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin3Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin4Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin4Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin4Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin4Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin5Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin5Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin5Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin5Max", value1, fpSet);

			value1.Format("%6.2f", binSet.m_Bin6Min);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin6Min", value1, fpSet);
			value1.Format("%6.2f", binSet.m_Bin6Max);
			saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERSISTENCE_PROB, 0, "Bin6Max", value1, fpSet);

			// end single run mode
			// end added for batch
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
}

void CPProbDialog::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	binSet.Close();
}

void CPProbDialog::OnOK()
{
	if(!hasData)
	{
		AfxMessageBox("Error: No variable selected.");
		return;
	}
	if(TableOK())
	{
		SaveTable();
		CClimAnalysis analysis(fpSet->m_pDatabase);
		int *varIDs = new int[1];
		varIDs[0] = indexList.GetItemData(indexList.GetCurSel());
		//EnableWindow(FALSE);
		int ret = analysis.RunAnalyze(varIDs, 1, fpSet, false);
			//Analyze(this, varIDs, 1, fpSet, false);
		//EnableWindow(TRUE);
		if(ret == IDOK)
		{
			analysis.PersistenceProbability(&binSet);
			CDialog::OnOK();
		}
	}
	else //an error in table entries
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: One or more incorrect table entries.\n"
			"The rules are:\n"
			"Max must be greater than Min.\n"
			"Max must be equal to next row's Min.\n"
			"All rows must be filled in.\n");
		AfxMessageBox(strMsg);
	}
}

//BEGIN_EVENTSINK_MAP(CPProbDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CPProbDialog)
	//ON_EVENT(CPProbDialog, IDC_DATATBLCTRL1, 7 /* UpdateCell */, OnUpdateCellDatatblctrl1, VTS_I4 VTS_I4 VTS_PI4)
	//}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

//#define step 0.01

/*void CPProbDialog::OnUpdateCellDatatblctrl1(long Row, long Column, long FAR* Status)
{
	// TODO: Add your control notification handler code here
	switch(Column)
	{
	case 0: //leaving a Min cell
	{
		double mx, mn, ev;
		CDTData RowData = indexTable.GetRowSet().GetItem(Row).GetValue(),
			cellData = indexTable.GetCurCell().GetEditValue();
		cellData.Get(&ev);
		RowData.GetItem(&mn, 0);
		RowData.GetItem(&mx, 1);
		if(mx <= mn)
		{//set max value if necessary
			ev += 10;
			CDTData nVal(ev);
			CPVDTCell nCell = indexTable.GetCellSet().GetItem(Row, 1);
			nCell.SetValue(nVal);
		}
		//set previous row's Min value automatically...
		if(Row > 0)
		{
			//ev += step;
			CDTData nVal(ev);
			CPVDTCell nCell = indexTable.GetCellSet().GetItem(Row - 1, 1);
			nCell.SetValue(nVal);
		}
	}
		break;
	case 1: //leaving a Max cell
	{
		double mx, mn, ev;
		CDTData RowData = indexTable.GetRowSet().GetItem(Row).GetValue(),
			cellData = indexTable.GetCurCell().GetEditValue();
		cellData.Get(&ev);
		RowData.GetItem(&mn, 0);
		RowData.GetItem(&mx, 1);
		//CDTCurCell curCell = indexTable.GetCurCell();
		if(ev <= mn)
		{
			CString strMsg;
			strMsg.Format(
				"ERROR: Max must be greater than Min.\n");
			AfxMessageBox(strMsg);
			*Status = true;
			return;
		}
		//set next row's Min value automatically...
		if(Row < 5)
		{
			//ev += step;
			CDTData nVal(ev);
			CPVDTCell nCell = indexTable.GetCellSet().GetItem(Row + 1, 0);
			nCell.SetValue(nVal);
		}
	}
		break;
	}
}*/

bool CPProbDialog::TableOK()
{
	CUGCell cell;
	double mx, mn, nmn;
	for(int i = 0; i < 6; i++)
	{
		m_grid.GetCell(1, i, &cell);
		mn = cell.GetNumber();
		m_grid.GetCell(2, i, &cell);
		mx = cell.GetNumber();
		if(mn >= mx)
			return false;
		if(i < 5)
		{
			m_grid.GetCell(1, i + 1, &cell);
			nmn = cell.GetNumber();
			if(nmn != mx)
				return false;
		}
	}

	return true;
}

void CPProbDialog::OnVarSort()
{
	// change the app's varSortOrder (and save to ffpOptions)
	// then re-sort and re-display

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;
	theApp.varSortOrder += 1;

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;

	CFFPOptsSet optsSet1(fpSet->m_pDatabase);
	optsSet1.Open();
	if(!optsSet1.IsEOF())
	{
			optsSet1.Edit();
			optsSet1.m_VarSortOrder = theApp.varSortOrder;
			optsSet1.Update();
	}
	optsSet1.Close();

	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";

	climSet.setVarSortOrder(theApp.varSortOrder);

	climSet.Open();

	indexList.ResetContent();

	while (!climSet.IsEOF())
				{
					CString str = climSet.m_Variable_Name;
					int eLoc = indexList.AddString(str);
					indexList.SetItemData(eLoc, climSet.m_VarID - 1);
					climSet.MoveNext();
				}

	climSet.Close();

	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}
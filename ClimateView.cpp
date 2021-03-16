// ClimateView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "fireplusDoc.h"
#include "climdoc.h"
#include "ClimateView.h"
#include "wxSet.h"
#include "SIGStationSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "FireSumSet.h"
#include "ClimAnalysis.h"
#include ".\climateview.h"
#include "RunSave.h"
#include "FFPOptsSet.h"
#include "ClimateSet.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int curr_runID;
extern CFireplusApp theApp;

ClimateCUG::ClimateCUG()
{
}

ClimateCUG::~ClimateCUG()
{
}

void ClimateCUG::OnSetup()
{
	m_iCheckBoxIndex = AddCellType( &m_checkBoxCT );
	m_iButtonIndex = AddCellType(&m_buttonCT);
	CRect rect;
	CUGCell cell;
	GetClientRect(&rect);
	int cWid = rect.Width() / 60;
	SetCurrentCellMode(2);
	SetNumberCols(8);
	SetUniformRowHeight(TRUE);
	SetDoubleBufferMode(TRUE);
	SetVScrollMode(UG_SCROLLTRACKING);
	SetColWidth(-1,cWid);
	SetColWidth(0, 6 * cWid);

	// 2012 added support for ClimSet variable sorting
	GetCell(0,-1,&cell);
	cell.SetCellType( m_iButtonIndex );
	cell.SetCellTypeEx( UGCT_BUTTONCLICK );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	cell.SetBackColor(RGB(215,215,215));
	cell.SetBorder(UG_BDR_RAISED);
	cell.SetText("Variable                                 ");
	SetCell(0,-1,&cell);

	SetColWidth(1, 4 * cWid);
	QuickSetText(1, -1, " Stats Table ");
	SetColWidth(2, 4 * cWid);
	QuickSetText(2, -1, " Stats Graph ");
	SetColWidth(3, 2 * cWid);
	QuickSetText(3, -1, " CP #1 ");
	SetColWidth(4, 2 * cWid);
	QuickSetText(4, -1, " CP #2 ");
	SetColWidth(5, 3.5 * cWid);
	QuickSetText(5, -1, " Daily Freqs ");
	SetColWidth(6, 3.5 * cWid);
	QuickSetText(6, -1, " Data Count ");
	SetColWidth(7, 3.5 * cWid);
	QuickSetText(7, -1, " Filter Value ");
	((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );

	GetColDefault( 0, &cell );

	cell.SetReadOnly(TRUE);
	cell.SetAlignment( UG_ALIGNLEFT|UG_ALIGNVCENTER);

	SetColDefault( 0, &cell );
	GetColDefault( 1, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 1, &cell );
	GetColDefault( 2, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 2, &cell );
	GetColDefault( 5, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 5, &cell );
	GetColDefault( 6, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 6, &cell );
	BestFit(0, 7, 1, UG_BESTFIT_TOPHEADINGS);
	LockColumns(1);
	//m_grid.QuickSetText(0, -1, "Station ID");
	//m_grid.QuickSetText(1, -1, "Station Name");
}

void ClimateCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void ClimateCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey );
}

int ClimateCUG::OnEditStart(int col, long row,CWnd **edit)
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
		m_cutNumeric.SetDecimalDigitCount(2);
		m_cutNumeric.SetShowLeadingZero(FALSE);
		//switch(col)
		//{
		//case 2:
		m_cutNumeric.SetFractionalDigitCount(0);
			//break;
		//case 3:
			//m_cutNumeric.SetFractionalDigitCount(2);
			//break;
		//}
		*edit = &m_cutNumeric;
	}
	return TRUE;
}

void ClimateCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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
		GotoCell( col, row );
	}
}

int ClimateCUG::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	//UNREFERENCED_PARAMETER(ID);
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(msg);
	//UNREFERENCED_PARAMETER(param);

	//if( ID == UGCT_CHECKBOX && msg == UGCT_CHECKBOXSET)
	if( msg == UGCT_CHECKBOXSET)
	{
		//check box value has changed....
		//CDTData cellData = m_table.GetCellSet().GetItem(Row,Column).GetValue();
		//LONG sel;
		//cellData.Get(&sel);
		CString tmp;
		QuickGetText(0, row, &tmp);//m_table.GetCellSet().GetItem(Row, 0).GetText();
		switch(col)
		{
		case 1:
			tmp += " - Summary Statistics Table";
			break;
		case 2:
			tmp += " - Summary Statistics Graph";
			break;
		case 5:
			tmp += " - Daily Frequencies Table";
			break;
		case 6:
			tmp += " - Analysis Period Data Count Table";
			break;
		default:
			return TRUE;
		}
		((CClimateView *)GetParent())->AddRemoveString(tmp, param);
	} else
    if (msg == UGCT_BUTTONCLICK)
	{
		if (col == 0)
			((CClimateView *)GetParent())->OnVarSort();
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CClimateView

IMPLEMENT_DYNCREATE(CClimateView, CRecordView)

CClimateView::CClimateView()
	: CRecordView(CClimateView::IDD)
{
	//{{AFX_DATA_INIT(CClimateView)
	m_pSet = NULL;
	//}}AFX_DATA_INIT
	m_climSet = NULL;
	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));}

CClimateView::~CClimateView()
{
	if(m_climSet)
	{
		if(m_climSet->IsOpen())
			m_climSet->Close();
		delete m_climSet;
	}
}

void CClimateView::DoDataExchange(CDataExchange* pDX)
{
	CRecordView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClimateView)
	DDX_Control(pDX, IDC_LIST1, m_SelectedList);
	//}}AFX_DATA_MAP
	//if(pDX->m_bSaveAndValidate == false)
		//ConfigureTable();
}

BEGIN_MESSAGE_MAP(CClimateView, CRecordView)
	//{{AFX_MSG_MAP(CClimateView)
	ON_BN_CLICKED(IDC_RUNCLIMATOLOGY, OnRunclimatology)
	ON_BN_CLICKED(IDC_CLEARALL, OnClearall)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_VARSORT, OnVarSort)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClimateView diagnostics

#ifdef _DEBUG
void CClimateView::AssertValid() const
{
	CRecordView::AssertValid();
}

void CClimateView::Dump(CDumpContext& dc) const
{
	CRecordView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CClimateView message handlers

CRecordset* CClimateView::OnGetRecordset()
{
	if (m_pSet != NULL)
		return m_pSet;

	//m_pSet = new CFireplusSet(NULL);
	//m_pSet->Open();
	m_pSet = ((CClimDoc *)GetDocument())->m_pSet;
	if(!m_climSet)
	{
		m_climSet = new CClimateSet(m_pSet->m_pDatabase);
		//m_climSet->m_strSort = "[VarID]";
		m_climSet->setVarSortOrder(theApp.varSortOrder);
		m_climSet->Open();
	}
	return m_pSet;
}

CFireplusSet* CClimateView::GetRecordset()
{
	CFireplusSet* pData = (CFireplusSet*) OnGetRecordset();
	ASSERT(pData == NULL || pData->IsKindOf(RUNTIME_CLASS(CFireplusSet)));
	return pData;
}

void CClimateView::OnInitialUpdate()
{
	BeginWaitCursor();
	GetRecordset();
	CRecordView::OnInitialUpdate();
	m_grid.AttachGrid(this, IDC_GRID);
	if (m_pSet->IsOpen())
	{
		char strTitle[256];
		strcpy_s(strTitle, m_pSet->m_pDatabase->GetDatabaseName());
		int loc;
		for(loc = strlen(strTitle); loc > 0; loc--)
		{
			if(strTitle[loc] == '\\')
			{
				loc++;
				break;
			}
		}
		strcpy_s(strTitle, &strTitle[loc]);
		strcat(strTitle, " Climatology Options");
		GetDocument()->SetTitle(strTitle);
	}
	//create/define the grid
	ConfigureTable();
	ResizeParentToFit(false);
	//m_climSet->Open();
	EndWaitCursor();
    CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));}

void CClimateView::OnRunclimatology()
{
	SaveTable();
	m_pSet->Edit();
	UpdateData(TRUE);
	m_pSet->Update();
	//ok, make sure query screen values are OK
	if(m_pSet->m_StartYear > m_pSet->m_EndYear)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: End year can not be less then Start Year.\n\n"));
		AfxMessageBox(strMsg);
		return;
	}
	//do adjustments to days of month if necessary....
	if(m_pSet->m_StartDay > 28)
	//if(m_pSet->m_StartDay > 29)
	{
		switch(MonthChtoInt(m_pSet->m_StartMonth))
		{
		case 4:
		case 6:
		case 9:
		case 11:
			if(m_pSet->m_StartDay == 31)
				m_pSet->m_StartDay = 30;
			break;
		case 2:
				m_pSet->m_StartDay = 28;
				//m_pSet->m_StartDay = 29;
			break;
		default://up to 31 is OK
			break;
		}
	}
	if(m_pSet->m_EndDay > 28)
	{
		switch(MonthChtoInt(m_pSet->m_EndMonth))
		{
		case 4:
		case 6:
		case 9:
		case 11:
			if(m_pSet->m_EndDay == 31)
				m_pSet->m_EndDay = 30;
			break;
		case 2:
				m_pSet->m_EndDay = 28;
				//m_pSet->m_EndDay = 29;
			break;
		default://up to 31 is OK
			break;
		}
	}
	//make sure something selected
	bool anything = false;
	m_climSet->MoveFirst();
	CString descString;
	int repCount = 0;
	while(!m_climSet->IsEOF())
	{
		if(m_climSet->m_Data_Count || m_climSet->m_Stats_Graph || m_climSet->m_Stats_Table 
			|| m_climSet->m_Daily_Freqs || m_climSet->m_Period_Mins || m_climSet->m_Period_Maxs)
		{
			if (repCount)
				descString.Append(", ");
			else
				descString.Format("Climatology: ");
			repCount ++;
			descString.Append(m_climSet->m_ShortName);

			anything = true;
		}
		m_climSet->MoveNext();
	}
	if(!anything)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: No reports selected.\n\n"));
		AfxMessageBox(strMsg);
		return;
	}
	// added for batch
	//
    // save run metadata
    // single run mode
	CRunSave saveRun(m_pSet);
	saveRun.SaveGeneral(0, descString, m_pSet);
	saveRun.SaveClimateOptions(curr_runID, m_pSet);
	saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_CLIMATOLOGY, 0, "", "", m_pSet);
	saveRun.SaveWxStation(curr_runID, m_pSet);
	saveRun.SaveFireOptions(curr_runID, m_pSet);

	// end single run mode
	// end added for batch
	//set up data and generate selected reports
		//create ClimateAnalysis
	int *tIDs = new int[repCount], ids = 0;
	m_climSet->MoveFirst();
	while (!m_climSet->IsEOF())
	{
		//check each var type
		if (m_climSet->m_Stats_Table || m_climSet->m_Stats_Graph || m_climSet->m_Daily_Freqs
			|| m_climSet->m_Period_Mins || m_climSet->m_Period_Maxs || m_climSet->m_Data_Count)
		{
			tIDs[ids] = m_climSet->m_VarID - 1;
			ids++;
		}
		m_climSet->MoveNext();
	}


	CClimAnalysis *climAnalysis = new CClimAnalysis(m_pSet->m_pDatabase);
	INT_PTR ret = climAnalysis->RunAnalyze(tIDs, repCount, m_pSet, false);
	if(ret == IDOK)
		ret = climAnalysis->GenerateClimatologyReports(m_climSet);
	else
		delete climAnalysis;
}

void CClimateView::ConfigureTable()
{
	CString temp;
	CUGCell cell;
	if(m_climSet->IsOpen())
	{
		try
		{    // requery recordset
			m_climSet->m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
			m_climSet->Requery();
			//set up rowdata structure
			m_SelectedList.ResetContent();
			m_grid.SetNumberRows(0);
			// Move through records
			int row = 0;
			while( !m_climSet->IsEOF( ) )
			{
				//add a record to the table
				m_grid.AppendRow();
				m_grid.GetCell(0, row, &cell);
				m_grid.QuickSetText(0, row, m_climSet->m_Variable_Name);
				cell.SetReadOnly(TRUE);
				cell.SetBackColor(RGB(0200,0200,0200));
				m_grid.GetCell(1, row, &cell);
				cell.SetNumber(m_climSet->m_Stats_Table);
				m_grid.SetCell(1, row, &cell);
				m_grid.GetCell(2, row, &cell);
				cell.SetNumber(m_climSet->m_Stats_Graph);
				m_grid.SetCell(2, row, &cell);
				m_grid.GetCell(3, row, &cell);
				cell.SetNumberDecimals(0);
				cell.SetNumber(m_climSet->m_CriticalPercentile);
				cell.SetReadOnly(FALSE);
				cell.SetParam(USE_COXNUMBER);
				m_grid.SetCell(3, row, &cell);
				m_grid.GetCell(4, row, &cell);
				cell.SetNumberDecimals(0);
				cell.SetNumber(m_climSet->m_cp2);
				cell.SetReadOnly(FALSE);
				cell.SetParam(USE_COXNUMBER);
				m_grid.SetCell(4, row, &cell);
				m_grid.GetCell(5, row, &cell);
				cell.SetNumber(m_climSet->m_Daily_Freqs);
				m_grid.SetCell(5, row, &cell);
				m_grid.GetCell(6, row, &cell);
				cell.SetNumber(m_climSet->m_Data_Count);
				m_grid.SetCell(6, row, &cell);
				m_grid.GetCell(7, row, &cell);
				if (!m_climSet->IsFieldNull(&m_climSet->m_FilterValue))
					cell.SetNumber(m_climSet->m_FilterValue);
				else
					cell.SetText("");
				m_grid.SetCell(7, row, &cell);
				//add any selected options to the selectedList
				if(m_climSet->m_Stats_Table)
				{
					temp.Format("%s - Summary Statistics Table",
						m_climSet->m_Variable_Name);
					m_SelectedList.AddString(temp);
				}
				if(m_climSet->m_Stats_Graph)
				{
					temp.Format("%s - Summary Statistics Graph",
						m_climSet->m_Variable_Name);
					m_SelectedList.AddString(temp);
				}
				if(m_climSet->m_Daily_Freqs)
				{
					temp.Format("%s - Daily Frequencies Table",
						m_climSet->m_Variable_Name);
					m_SelectedList.AddString(temp);
				}
				if(m_climSet->m_Period_Mins)
				{
					temp.Format("%s - Analysis Period Minimums Table",
						m_climSet->m_Variable_Name);
					m_SelectedList.AddString(temp);
				}
				if(m_climSet->m_Period_Maxs)
				{
					temp.Format("%s - Analysis Period Maximums Table",
						m_climSet->m_Variable_Name);
					m_SelectedList.AddString(temp);
				}
				if(m_climSet->m_Data_Count)
				{
					temp.Format("%s - Analysis Period Data Count Table",
						m_climSet->m_Variable_Name);
					m_SelectedList.AddString(temp);
				}
				//go to next record
				m_climSet->MoveNext( );
				row++;
			}
		}
		catch( CDBException* e )
		{
			// Do nothing--used for security violations     // when opening tables
			e->Delete( );
		}
	}
	//m_grid.BestFit(0, 7, 45, 2);
	m_grid.RedrawAll();
}

void CClimateView::SaveTable()
{
	//m_table.GetCurCell().EndEdit();
	if(m_climSet->IsOpen())
	{
		try
		{    // requery recordset
			m_climSet->MoveFirst();
			int rc = 0;
//			BOOL bVal;
//			short cp;
			CUGCell cell;
			CString fStr;
			while(!m_climSet->IsEOF())
			{
				//CDTData RowData = m_table.GetRowSet().GetItem(rc).GetValue();
				//rc++;
				m_climSet->Edit();
				m_grid.GetCell(1, rc, &cell);
				m_climSet->m_Stats_Table = cell.GetNumber();
				m_grid.GetCell(2, rc, &cell);
				m_climSet->m_Stats_Graph = cell.GetNumber();
				m_grid.GetCell(3, rc, &cell);
				m_climSet->m_CriticalPercentile = (short)cell.GetNumber();
				m_grid.GetCell(4, rc, &cell);
				m_climSet->m_cp2 = (short)cell.GetNumber();
				m_grid.GetCell(5, rc, &cell);
				m_climSet->m_Daily_Freqs = cell.GetNumber();
				m_grid.GetCell(6, rc, &cell);
				m_climSet->m_Data_Count = cell.GetNumber();
				m_grid.GetCell(7, rc, &cell);
				m_grid.QuickGetText(7, rc, &fStr);
				fStr.Trim();
				if (fStr.GetLength() > 0)
					m_climSet->m_FilterValue = cell.GetNumber();
				else
					m_climSet->SetFieldNull(&m_climSet->m_FilterValue);
				m_climSet->Update();
				m_climSet->MoveNext();
				rc++;
			}
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
}

void CClimateView::OnClearall()
{
	//clear all report and graph selections
	if(m_climSet->IsOpen())
	{
		try
		{    // requery recordset
			m_climSet->MoveFirst();
			while(!m_climSet->IsEOF())
			{
				m_climSet->Edit();
				m_climSet->m_Stats_Table = 0;
				m_climSet->m_Stats_Graph = 0;
				m_climSet->m_Daily_Freqs = 0;
				m_climSet->m_Data_Count = 0;
				m_climSet->Update();
				m_climSet->MoveNext();
			}
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
	ConfigureTable();
}

void CClimateView::OnFileSave()
{
}

void CClimateView::OnFileSaveAs()
{
}

void CClimateView::OnVarSort()
{
	// change the app's varSortOrder (and save to ffpOptions)
	// then re-sort and re-display

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;
	theApp.varSortOrder += 1;

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;

	CFFPOptsSet optsSet1(m_pSet->m_pDatabase);
	optsSet1.Open();
	if(!optsSet1.IsEOF())
	{
			optsSet1.Edit();
			optsSet1.m_VarSortOrder = theApp.varSortOrder;
			optsSet1.Update();
	}
	optsSet1.Close();

	m_climSet->setVarSortOrder(theApp.varSortOrder);
	ConfigureTable();
	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}

void CClimateView::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

int CClimateView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));

	if (CRecordView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	return 0;
}

void CClimateView::AddRemoveString(CString str, bool add)
{
	if (add)
		m_SelectedList.AddString(str);
	else
	{
		int delLoc = m_SelectedList.FindString(-1, str);
		if(delLoc >= 0)
			m_SelectedList.DeleteString(delLoc);
	}
}
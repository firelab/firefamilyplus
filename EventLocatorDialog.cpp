// EventLocatorDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "fireplusSet.h"
#include "ClimateSet.h"
#include "SIGStationSet.h"
#include "wxSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "splitterframe.h"
#include "CFiresSet.h"
#include "FFPOptsSet.h"
#include "ClimAnalysis.h"
#include "EventLocatorDialog.h"
#include ".\eventlocatordialog.h"
#include "RunSave.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;

EventLocatorCUG::EventLocatorCUG()
{
}

EventLocatorCUG::~EventLocatorCUG()
{
}

void EventLocatorCUG::OnSetup()
{
	CRect gRect;
	GetClientRect(&gRect);
	int wid = gRect.Width() / 25;
	SetCurrentCellMode(2);
	SetNumberCols(6);
	QuickSetText(0, -1, "Operator");

	CUGCell cell;

	// 2012 added support for ClimSet variable sorting
	m_iButtonIndex = AddCellType(&m_buttonCT);
	GetCell(1,-1,&cell);
	cell.SetCellType( m_iButtonIndex );
	cell.SetCellTypeEx( UGCT_BUTTONCLICK );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	cell.SetBackColor(RGB(215,215,215));
	cell.SetBorder(UG_BDR_RAISED);
	cell.SetText("Variable");
	SetCell(1,-1,&cell);

	QuickSetText(2, -1, "Category");
	QuickSetText(3, -1, "Operator");
	QuickSetText(4, -1, "Value");
	QuickSetText(5, -1, "Value Type");
	SetColWidth(-1, 0);
	SetColWidth(0, 3 * wid);
	SetColWidth(1, 8 * wid);
	SetColWidth(2, 3 * wid);
	SetColWidth(3, 3 * wid);
	SetColWidth(4, 3 * wid);
	SetColWidth(5, 4 * wid);
	((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );

	GetColDefault( 0, &cell );
	cell.SetCellType(UGCT_DROPLIST);
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText("AND\nOR\n");
	SetColDefault( 0, &cell );
	varsStr = "";
	CEventLocatorDialog *pDlg = (CEventLocatorDialog *)GetParent();
	//pDlg->climSet.m_strSort = "[VarID]";
	pDlg->climSet.setVarSortOrder(theApp.varSortOrder);
	pDlg->climSet.Open();
	while(!pDlg->climSet.IsEOF())
	{
		varsStr += pDlg->climSet.m_Variable_Name + "\n";
		pDlg->climSet.MoveNext();
	}
	GetColDefault( 1, &cell );
	cell.SetCellType(UGCT_DROPLIST);
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText(varsStr);
	SetColDefault( 1, &cell );
	GetColDefault( 2, &cell );
	cell.SetCellType(UGCT_DROPLIST);
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText("Sum\nAvg\nMin\nMax\nDaily\n");
	SetColDefault( 2, &cell );
	GetColDefault( 3, &cell );
	cell.SetCellType(UGCT_DROPLIST);
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText("=\n<=\n>=\n<\n>\n");
	SetColDefault( 3, &cell );
	GetColDefault( 4, &cell );
	cell.SetNumberDecimals(2);
	SetColDefault(4, &cell);
	GetColDefault( 5, &cell );
	cell.SetCellType(UGCT_DROPLIST);
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText("Value\nPercentile\n");
	SetColDefault( 5, &cell );
	//fill table
//	int row;
/*	CString str;
	for(int i = 0; i < theApp.nQrows; i++)
	{
		AppendRow();
		if(i > 0)
		{
			str = "AND";
			if(theApp.queryRows[i].rowOp != AND)
				str = "OR";
			QuickSetText(0, i, str);
		}
		else
		{
			GetCell(0, 0, &cell);
			cell.SetText("");
			cell.SetReadOnly(TRUE);
			cell.SetBackColor(RGB(0, 0, 0));
			cell.SetHBackColor(RGB(0, 0, 0));
			cell.SetTextColor(RGB(0, 0, 0));
			cell.SetHTextColor(RGB(0, 0, 0));
			SetCell(0, 0, &cell);
		}
		QuickSetText(1, i, theApp.queryRows[i].varString);
		switch(theApp.queryRows[i].cat)
		{
		case Sum:
			//rowData.PutItem("Sum", 2);
			str = "Sum";
			break;
		case Avg:
			//rowData.PutItem("Avg", 2);
			str = "Avg";
			break;
		case Min:
			//rowData.PutItem("Min", 2);
			str = "Min";
			break;
		case Max:
			//rowData.PutItem("Max", 2);
			str = "Max";
			break;
		case Daily:
			//rowData.PutItem("Daily", 2);
			str = "Daily";
			break;
		}
		QuickSetText(2, i, str);
		switch(theApp.queryRows[i].valOp)
		{
		case EQ:
			str = "=";
			break;
		case LE:
			str = "<=";
			//rowData.PutItem("<=", 3);
			break;
		case GE:
			str = ">=";
			//rowData.PutItem(">=", 3);
			break;
		case LT:
			str = "<";
			//rowData.PutItem("<", 3);
			break;
		case GT:
			str = ">";
			//rowData.PutItem(">", 3);
			break;
		}
		QuickSetText(3, i, str);
		GetCell(4, i, &cell);
		cell.SetNumber(theApp.queryRows[i].value);
		SetCell(4, i, &cell);
		str = "Value";
		if(theApp.queryRows[i].valType != Value)
			str = "Percentile";
		QuickSetText(5, i, str);
	}*/
}

int EventLocatorCUG::OnEditStart(int col, long row,CWnd **edit)
{
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

int EventLocatorCUG::OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param)
{
	if (msg == UGCT_BUTTONCLICK)
	{
		if (col == 1)
			((CEventLocatorDialog *)GetParent())->OnVarSort();
	}
	return TRUE;
}

void EventLocatorCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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

void EventLocatorCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void EventLocatorCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey );
}

COLORREF EventLocatorCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);
}

void EventLocatorCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	//DrawExcelFocusRect(dc,rect);

	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

/////////////////////////////////////////////////////////////////////////////
// CEventLocatorDialog dialog

CEventLocatorDialog::CEventLocatorDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet)
	: CDialog(CEventLocatorDialog::IDD, pParent), climSet(_fpSet->m_pDatabase), records(_fpSet->m_pDatabase)
{
	//{{AFX_DATA_INIT(CEventLocatorDialog)
	days = 1;
	//}}AFX_DATA_INIT
	//days = theApp.eventDays;
	fpSet = _fpSet;
	records.m_strFilter.Format("[SIG_Station] = '%s'", _fpSet->m_SIG_Station);
	records.m_strSort = "[Row]";
	records.Open();
}

CEventLocatorDialog::~CEventLocatorDialog()
{
	records.Close();
}

void CEventLocatorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventLocatorDialog)
	DDX_Text(pDX, IDC_EDIT1, days);
	DDV_MinMaxInt(pDX, days, 1, 366);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_REMOVEROW, m_removeButton);
}

BEGIN_MESSAGE_MAP(CEventLocatorDialog, CDialog)
	//{{AFX_MSG_MAP(CEventLocatorDialog)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ADDROW, OnBnClickedAddrow)
	ON_BN_CLICKED(IDC_REMOVEROW, OnBnClickedRemoverow)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_DEFAULTS, &CEventLocatorDialog::OnBnClickedButtonLoadDefaults)
	ON_BN_CLICKED(IDC_BUTTON_SET_DEFAULT, &CEventLocatorDialog::OnBnClickedButtonSetDefault)
	//ON_COMMAND(ID_VARSORT, OnVarSort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventLocatorDialog message handlers

BOOL CEventLocatorDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);
	m_removeButton.EnableWindow((m_grid.GetNumberRows() > 1));
	CString str;
	CUGCell cell;
	if(!records.IsBOF() && !records.IsEOF())
	{//have records
		int i = 0;
		while(!records.IsEOF())
		{
			m_grid.AppendRow();
			if(i > 0)
			{
				str = "AND";
				if(records.m_RowOp != AND)
					str = "OR";
				m_grid.QuickSetText(0, i, str);
			}
			else
			{
				m_grid.GetCell(0, 0, &cell);
				cell.SetText("");
				cell.SetReadOnly(TRUE);
				cell.SetBackColor(RGB(0, 0, 0));
				cell.SetHBackColor(RGB(0, 0, 0));
				cell.SetTextColor(RGB(0, 0, 0));
				cell.SetHTextColor(RGB(0, 0, 0));
				m_grid.SetCell(0, 0, &cell);
				days = records.m_RowOp;
				if(days <= 0)
					days = 1;
			}
			m_grid.QuickSetText(1, i, records.m_VarString);
			switch(records.m_Category)
			{
			case Sum:
				//rowData.PutItem("Sum", 2);
				str = "Sum";
				break;
			case Avg:
				//rowData.PutItem("Avg", 2);
				str = "Avg";
				break;
			case Min:
				//rowData.PutItem("Min", 2);
				str = "Min";
				break;
			case Max:
				//rowData.PutItem("Max", 2);
				str = "Max";
				break;
			case Daily:
				//rowData.PutItem("Daily", 2);
				str = "Daily";
				break;
			}
			m_grid.QuickSetText(2, i, str);
			switch(records.m_ValOp)
			{
			case EQ:
				str = "=";
				break;
			case LE:
				str = "<=";
				//rowData.PutItem("<=", 3);
				break;
			case GE:
				str = ">=";
				//rowData.PutItem(">=", 3);
				break;
			case LT:
				str = "<";
				//rowData.PutItem("<", 3);
				break;
			case GT:
				str = ">";
				//rowData.PutItem(">", 3);
				break;
			}
			m_grid.QuickSetText(3, i, str);
			m_grid.GetCell(4, i, &cell);
			cell.SetNumber(records.m_Value);
			m_grid.SetCell(4, i, &cell);
			str = "Value";
			if(records.m_ValType != Value)
				str = "Percentile";
			m_grid.QuickSetText(5, i, str);
			records.MoveNext();
			i++;
		}
	}
	m_removeButton.EnableWindow((m_grid.GetNumberRows() > 1));
	BlackenFirstCell();
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventLocatorDialog::OnOK()
{
	//build QueryRows...
	CString msg, str;
	CUGCell cell;
//	double val;
	UpdateData();
	int rows = m_grid.GetNumberRows();//table.GetRowSet().GetCount();
	if(rows <= 0)
	{
		AfxMessageBox("Error: No filter specified.");
		return;
	}
	QueryRow *qRows = new QueryRow[rows];

	CString value1;
	// added for batch
	// single run mode
	CRunSave saveRun(fpSet);
	// end single run mode
	// end added for batch
	int i;
	CString runDesc;
	runDesc.Format("Event Loc.: ");
	int repCount  = 0;

	for(i = 0; i < rows; i++)
	{
		if(i > 0)
		{//RowOp
			str = m_grid.QuickGetText(0, i);
			if(str == "AND")
				qRows[i].rowOp = AND;
			else if(str == "OR")
				qRows[i].rowOp = OR;
			else
			{
				msg.Format("ERROR: Row %d requires an AND or OR operator.", i + 1);
				AfxMessageBox(msg);
				delete[] qRows;
				return;
			}
			// operator
			value1.Format("%s", str);
		}
		else
			value1 = " ";

		saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR, i, "rowOp", value1, fpSet);

		//varID
		str = m_grid.QuickGetText(1, i);
		if(str.GetLength() <= 0)
		{
			msg.Format("Error: Line %d, no variable selected.", i + 1);
			AfxMessageBox(msg);
			delete[] qRows;
			return;
		}
		// var
		value1.Format("%s",str);
		saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR,i,"Variable_Name",value1,fpSet);
		qRows[i].varString = str;
		//CString seek;

		climSet.m_strFilter.Format("[Variable Name] = '%s'", str);
		climSet.setVarSortOrder(theApp.varSortOrder);
		climSet.Requery();

		//if(climSet.Find(AFX_DAO_FIRST, seek) == 0)
		if(climSet.IsEOF())
		{
			msg.Format("Error: Line %d, unrecognized variable name.", i + 1);
			AfxMessageBox(msg);
			delete[] qRows;
			return;
		}
		qRows[i].varID = climSet.m_VarID - 1;
		if (repCount)
			runDesc.Append(", ");
		runDesc.Append(climSet.m_ShortName);
		repCount ++;

		//category
		str = m_grid.QuickGetText(2, i);
		if(str.GetLength() <= 0 && days > 1)
		{
			msg.Format("Error: Line %d, no category selected.\n(Only necessary for multiple day periods.)", i + 1);
			AfxMessageBox(msg);
			delete[] qRows;
			return;
		}
		if(days < 1)
		{
			qRows[i].cat = Avg;
			value1 = "Avg";
		}
		else
		{
			if(str == "Sum")
				qRows[i].cat = Sum;
			else if(str == "Avg")
				qRows[i].cat = Avg;
			else if(str == "Min")
				qRows[i].cat = Min;
			else if(str == "Max")
				qRows[i].cat = Max;
			else if(str == "Daily")
				qRows[i].cat = Daily;
			else
			{
				msg.Format("Error: Line %d, unrecognized category.", i + 1);
				AfxMessageBox(msg);
				delete[] qRows;
				return;
			}
			value1.Format("%s",str);
		}
		saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR,i,"category",value1,fpSet);
		//operator
		str = m_grid.QuickGetText(3, i);
		if(str.GetLength() <= 0)
		{
			msg.Format("Error: Line %d, no variable operator selected.", i + 1);
			AfxMessageBox(msg);
			delete[] qRows;
			return;
		}
		if(str == "=")
			qRows[i].valOp = EQ;
		else if(str == "<=")
			qRows[i].valOp = LE;
		else if(str == ">=")
			qRows[i].valOp = GE;
		else if(str == "<")
			qRows[i].valOp = LT;
		else if(str == ">")
			qRows[i].valOp = GT;
		else
		{
			msg.Format("Error: Line %d, unrecognized variable operator.", i + 1);
			AfxMessageBox(msg);
			delete[] qRows;
			return;
		}
		value1.Format("%s",str);
		// value operator

		saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR,i,"valOp",value1,fpSet);
		//value
		m_grid.GetCell(4, i, &cell);
		qRows[i].value = cell.GetNumber();
		// value
		value1.Format("%8.2f",qRows[i].value);
		saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR,i,"value",value1,fpSet);
		//value type
		str = m_grid.QuickGetText(5, i);
		if(str.GetLength() <= 0)
		{
			msg.Format("Error: Line %d, no value type selected.", i + 1);
			AfxMessageBox(msg);
			delete[] qRows;
			return;
		}
		if(str == "Value")
			qRows[i].valType = Value;
		else if(str == "Percentile")
			qRows[i].valType = Percentile;
		else
		{
			msg.Format("Error: Line %d, unrecognized value type.", i + 1);
			AfxMessageBox(msg);
			delete[] qRows;
			return;
		}
		value1.Format("%s",str);
		saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR,i,"valType",value1,fpSet);
	}

	saveRun.SaveGeneral(0, runDesc, fpSet);
	saveRun.SaveClimateOptions(curr_runID, fpSet);
	// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
	saveRun.SaveFireOptions(curr_runID, fpSet);
	//ok, qRows complete
	int *varIDs = new int[rows];
	for(i = 0; i < rows; i++)
		varIDs[i] = qRows[i].varID;
	fpSet->Edit();
	fpSet->m_PeriodLength = "1";
	fpSet->Update();
	//EnableWindow(FALSE);
	CClimAnalysis analysis(fpSet->m_pDatabase);
	int ret = analysis.RunAnalyze(varIDs, rows, fpSet, false);
	//int ret = analysis.Analyze(this, varIDs, rows, fpSet, false);
	//EnableWindow(TRUE);
	if(ret == IDOK)
	{
		analysis.LocateEvents(qRows, rows, days);

		if(!records.IsBOF())// && !records.IsEOF())
		{//clear existing records
			records.MoveFirst();
			while(!records.IsEOF())
			{
				records.Delete();
				records.MoveNext();
			}
		}
		for(int r = 0; r < rows; r++)
		{
			records.AddNew();
			records.m_SIG_Station = fpSet->m_SIG_Station;
			records.m_Row = r;
			if(r == 0)
				records.m_RowOp = days;
			else
				records.m_RowOp = qRows[r].rowOp;
			records.m_Category = qRows[r].cat;
			records.m_ValOp = qRows[r].valOp;
			records.m_ValType = qRows[r].valType;
			records.m_Value = qRows[r].value;
			records.m_VarID = qRows[r].varID;
			records.m_VarString = qRows[r].varString;
			records.Update();
		}
		delete[] qRows;
		//if(theApp.queryRows)
		//	delete[] theApp.queryRows;
		//theApp.queryRows = qRows;
		//theApp.nQrows = rows;
		//theApp.eventDays = days;
		CDialog::OnOK();
	}
	else
		delete[] qRows;
}

void CEventLocatorDialog::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	climSet.Close();

	CDialog::PostNcDestroy();
}

void CEventLocatorDialog::BlackenFirstCell()
{
	CUGCell cell;
	m_grid.GetCell(0, 0, &cell);
	cell.SetText("");
	cell.SetReadOnly(TRUE);
	cell.SetBackColor(RGB(0, 0, 0));
	cell.SetHBackColor(RGB(0, 0, 0));
	cell.SetTextColor(RGB(0, 0, 0));
	cell.SetHTextColor(RGB(0, 0, 0));
	m_grid.SetCell(0, 0, &cell);
}

void CEventLocatorDialog::OnBnClickedAddrow()
{
	m_grid.AppendRow();
	m_removeButton.EnableWindow((m_grid.GetNumberRows() > 1));
	BlackenFirstCell();
}

void CEventLocatorDialog::OnBnClickedRemoverow()
{
	if(m_grid.GetNumberRows() > 1)
	{
		m_grid.DeleteRow(m_grid.GetCurrentRow());
	}
	BlackenFirstCell();
	m_removeButton.EnableWindow((m_grid.GetNumberRows() > 1));
}

void CEventLocatorDialog::OnBnClickedButtonLoadDefaults()
{
	if(IDYES == AfxMessageBox("Load default event criteria?", MB_YESNO))
	{
		m_grid.SetNumberRows(0);
		CFFPDailyEventsSet tSet(records.m_pDatabase);
		tSet.m_strFilter.Format("[SIG_Station] = '######'");
		tSet.Open();
		int i = 0;
		CString str;
		CUGCell cell;
		while(!tSet.IsEOF())
		{
			m_grid.AppendRow();
			if(i > 0)
			{
				str = "AND";
				if(tSet.m_RowOp != AND)
					str = "OR";
				m_grid.QuickSetText(0, i, str);
			}
			else
			{
				m_grid.GetCell(0, 0, &cell);
				cell.SetText("");
				cell.SetReadOnly(TRUE);
				cell.SetBackColor(RGB(0, 0, 0));
				cell.SetHBackColor(RGB(0, 0, 0));
				cell.SetTextColor(RGB(0, 0, 0));
				cell.SetHTextColor(RGB(0, 0, 0));
				m_grid.SetCell(0, 0, &cell);
				days = tSet.m_RowOp;
				if(days <= 0)
					days = 1;
			}
			m_grid.QuickSetText(1, i, tSet.m_VarString);
			switch(records.m_Category)
			{
			case Sum:
				//rowData.PutItem("Sum", 2);
				str = "Sum";
				break;
			case Avg:
				//rowData.PutItem("Avg", 2);
				str = "Avg";
				break;
			case Min:
				//rowData.PutItem("Min", 2);
				str = "Min";
				break;
			case Max:
				//rowData.PutItem("Max", 2);
				str = "Max";
				break;
			case Daily:
				//rowData.PutItem("Daily", 2);
				str = "Daily";
				break;
			}
			m_grid.QuickSetText(2, i, str);
			switch(tSet.m_ValOp)
			{
			case EQ:
				str = "=";
				break;
			case LE:
				str = "<=";
				//rowData.PutItem("<=", 3);
				break;
			case GE:
				str = ">=";
				//rowData.PutItem(">=", 3);
				break;
			case LT:
				str = "<";
				//rowData.PutItem("<", 3);
				break;
			case GT:
				str = ">";
				//rowData.PutItem(">", 3);
				break;
			}
			m_grid.QuickSetText(3, i, str);
			m_grid.GetCell(4, i, &cell);
			cell.SetNumber(tSet.m_Value);
			m_grid.SetCell(4, i, &cell);
			str = "Value";
			if(tSet.m_ValType != Value)
				str = "Percentile";
			m_grid.QuickSetText(5, i, str);
			i++;
			tSet.MoveNext();
		}
		UpdateData(FALSE);
		BlackenFirstCell();
		m_removeButton.EnableWindow((m_grid.GetNumberRows() > 1));
		m_grid.RedrawAll();
	}
}

void CEventLocatorDialog::OnVarSort()
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
	climSet.m_strFilter.Format("");
	climSet.setVarSortOrder(theApp.varSortOrder);

	climSet.Requery();
	m_grid.varsStr="";

	while(! climSet.IsEOF())
	{
		m_grid.varsStr += climSet.m_Variable_Name + "\n";
		climSet.MoveNext();
	}
	CUGCell cell;

	long row = m_grid.GetCurrentRow();
	if (row >= 0)
	{
	   m_grid.GetCell(1,row,&cell);
	   m_grid.GetColDefault( 1, &cell );
	   cell.SetLabelText(m_grid.varsStr);
	   m_grid.SetCell(1,row,&cell);
	}
	m_grid.RedrawAll();

	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}

void CEventLocatorDialog::OnBnClickedButtonSetDefault()
{
	UpdateData();
	if(m_grid.GetNumberRows() <= 0)
	{
		AfxMessageBox("No criteria specified...");
		return;
	}
	if(IDYES == AfxMessageBox("Set event criteria as default?", MB_YESNO))
	{
		CUGCell cell;
		CFFPDailyEventsSet tSet(records.m_pDatabase);
		tSet.m_strFilter.Format("[SIG_Station] = '######'");
		tSet.Open();
		while(!tSet.IsEOF())
		{
			tSet.Delete();
			tSet.MoveNext();
		}
		CString str;
		for(int r = 0; r < m_grid.GetNumberRows(); r++)
		{
			tSet.AddNew();
			tSet.m_SIG_Station = "######";
			tSet.m_Row = r;
			if(r == 0)
				tSet.m_RowOp = days;
			else
			{
				str = m_grid.QuickGetText(0, r);
				tSet.m_RowOp = OR;
				if(str == "AND")
					tSet.m_RowOp = AND;
			}
			str = m_grid.QuickGetText(1, r);
			tSet.m_VarString = str;
			climSet.m_strFilter.Format("[Variable Name] = '%s'", str);
			climSet.Requery();
			if(!climSet.IsEOF())
				tSet.m_VarID = climSet.m_VarID - 1;
			str = m_grid.QuickGetText(2, r);
			tSet.m_Category = Daily;
			if(str == "Sum")
				tSet.m_Category = Sum;
			else if(str == "Avg")
				tSet.m_Category = Avg;
			else if(str == "Min")
				tSet.m_Category = Min;
			else if(str == "Max")
				tSet.m_Category = Max;
			str = m_grid.QuickGetText(3, r);
			tSet.m_ValOp = EQ;
			if(str == "<=")
				tSet.m_ValOp = LE;
			else if(str == ">=")
				tSet.m_ValOp = GE;
			else if(str == "<")
				tSet.m_ValOp = LT;
			else if(str == ">")
				tSet.m_ValOp = GT;
			m_grid.GetCell(4, r, &cell);
			tSet.m_Value = cell.GetNumber();
			str = m_grid.QuickGetText(5, r);
			tSet.m_ValType = Value;
			if(str == "Percentile")
				tSet.m_ValType = Percentile;
			tSet.Update();
		}
		tSet.Close();
	}
}
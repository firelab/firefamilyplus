// HourlyEventLocatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "fireplusSet.h"
#include "ClimateSet.h"
#include "SIGStationSet.h"
#include "wxSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"
#include "FFPOptsSet.h"
#include "HourlyEventLocatorDlg.h"
#include ".\hourlyeventlocatordlg.h"
//#include "EventLocatorDialog.h"
#include "DiurnalAnalysis2.h"
#include "FFPHourlyEventsSet.h"
#include "MainFrm.h"

extern CFireplusApp theApp;
#define USE_COXNUMBER 1001
#define COXMASK_ID			320001

/*CHourlyEventSource::CHourlyEventSource(void)
{
	records = NULL;
	totalRecs = 0;
	virRec = 0;
}

CHourlyEventSource::~CHourlyEventSource(void)
{
}

long CHourlyEventSource::GetNumRows()
{
	return totalRecs;
}

int CHourlyEventSource::GetNumCols()
{
	return 6;
}

int	CHourlyEventSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 0:
			cell->SetText("Operator");
			return UG_SUCCESS;
		case 1:
			cell->SetText("Variable");
			return UG_SUCCESS;
		case 2:
			cell->SetText("Category");
			return UG_SUCCESS;
		case 3:
			cell->SetText("Operator");
			return UG_SUCCESS;
		case 4:
			cell->SetText("Value");
			return UG_SUCCESS;
		case 5:
			cell->SetText("Value Type");
			return UG_SUCCESS;
		default:
			cell->SetText("");
			return UG_SUCCESS;
		}
	}
	if(row == 0)
		records->MoveFirst();
	else if(row == totalRecs - 1)
		records->MoveLast();
	else if(row != virRec)
		records->Move(row - virRec);
	//CString str;
	switch(col)
	{
	case 0:
		if(row > 0)
		{
			str = "AND";
			if(records->m_RowOp != AND)
				str = "OR";
			cell->SetText(str);
		}
		else
		{
			cell->SetText("");
			cell->SetReadOnly(TRUE);
			cell->SetBackColor(RGB(0, 0, 0));
			cell->SetHBackColor(RGB(0, 0, 0));
			cell->SetTextColor(RGB(0, 0, 0));
			cell->SetHTextColor(RGB(0, 0, 0));
		}
		//cell->SetNumber(row + 1);
		break;
	case 1:
		cell->SetText(records->m_VarString);
		break;
	case 2:
		switch(records->m_Category)
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
			str = "Hourly";
			break;
		}
		cell->SetText(str);
		break;
	case 3:
		switch(records->m_ValOp)
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
			break;
		}
		cell->SetText(str);
		break;
	case 4:
		cell->SetNumber(records->m_Value);
		break;
	case 5:
		str = "Value";
		if(records->m_ValType != Value)
			str = "Percentile";
		cell->SetText(str);
		break;
	}
	virRec = row;
	return UG_SUCCESS;
}

void CHourlyEventSource::SetRecords(CFFPHourlyEventsSet *_records)
{
	records = _records;
	totalRecs = 0;
	virRec = 0;
	if(!records->IsBOF() || !records->IsEOF())
	{
		records->MoveFirst();
		while(!records->IsEOF())
		{
			totalRecs++;
			records->MoveNext();
		}
		if(totalRecs > 0)
			records->MoveFirst();
	}
}
*/

HourlyEventLocatorCUG::HourlyEventLocatorCUG()
{
}

HourlyEventLocatorCUG::~HourlyEventLocatorCUG()
{
}

void HourlyEventLocatorCUG::OnSetup()
{
	CRect gRect;
	GetClientRect(&gRect);
	int wid = gRect.Width() / 25;
	SetCurrentCellMode(2);
	SetNumberCols(6);

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

	QuickSetText(1, -1, "Variable");
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
	CString varsStr = "";
	CHourlyEventLocatorDlg *pDlg = (CHourlyEventLocatorDlg *)GetParent();
	//pDlg->climSet.m_strSort = "[VarID]";
	pDlg->climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
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
	cell.SetLabelText("Sum\nAvg\nMin\nMax\nHourly\n");
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
	//CFFPHourlyEventsSet eventsSet(fpSet->
	/*CString str;
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
			str = "Hourly";
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

int HourlyEventLocatorCUG::OnEditStart(int col, long row,CWnd **edit)
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

void HourlyEventLocatorCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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

void HourlyEventLocatorCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void HourlyEventLocatorCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey );
}

COLORREF HourlyEventLocatorCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);
}

void HourlyEventLocatorCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	//DrawExcelFocusRect(dc,rect);

	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

int HourlyEventLocatorCUG::OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param)
{
	if (msg == UGCT_BUTTONCLICK)
	{
		if (col == 1)
			((CHourlyEventLocatorDlg *)GetParent())->OnVarSort();
	}
	return TRUE;
}

// CHourlyEventLocatorDlg dialog

IMPLEMENT_DYNCREATE(CHourlyEventLocatorDlg, CDialog)

CHourlyEventLocatorDlg::CHourlyEventLocatorDlg(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet /*= NULL*/)
: CDialog(CHourlyEventLocatorDlg::IDD, pParent), climSet(_fpSet->m_pDatabase), records(_fpSet->m_pDatabase)
{
	hours = 1;//theApp.eventHours;
	fpSet = _fpSet;
	records.m_strFilter.Format("[SIG_Station] = '%s'", _fpSet->m_SIG_Station);
	records.m_strSort = "[Row]";
	records.Open();
	//m_grid.m_data.SetRecords(&records);
}

CHourlyEventLocatorDlg::~CHourlyEventLocatorDlg()
{
	records.Close();
}

void CHourlyEventLocatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventLocatorDialog)
	DDX_Text(pDX, IDC_EDIT1, hours);
	DDV_MinMaxInt(pDX, hours, 1, 1000);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_REMOVEROW, m_removeButton);
}

BOOL CHourlyEventLocatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//if(!records.IsBOF() || !records.IsEOF())
	//	records.MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);
	//add whatever records already exist for this SIG/Station
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
				hours = records.m_RowOp;
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
				str = "Hourly";
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
}

BEGIN_MESSAGE_MAP(CHourlyEventLocatorDlg, CDialog)
	ON_BN_CLICKED(IDC_ADDROW, OnBnClickedAddrow)
	ON_BN_CLICKED(IDC_REMOVEROW, OnBnClickedRemoverow)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_DEFAULTS, &CHourlyEventLocatorDlg::OnBnClickedButtonLoadDefaults)
	ON_BN_CLICKED(IDC_BUTTON_SET_DEFAULT, &CHourlyEventLocatorDlg::OnBnClickedButtonSetDefault)
END_MESSAGE_MAP()

// CHourlyEventLocatorDlg message handlers

void CHourlyEventLocatorDlg::OnBnClickedOk()
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
	//CRunSave saveRun(fpSet);
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

//		saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR, i, "rowOp", value1, fpSet);

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
//		saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR,i,"Variable_Name",value1,fpSet);
		qRows[i].varString = str;
		//CString seek;
		climSet.m_strFilter.Format("[Variable Name] = '%s'", str);
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
		if(str.GetLength() <= 0 && hours > 1)
		{
			msg.Format("Error: Line %d, no category selected.\n(Only necessary for multiple hour periods.)", i + 1);
			AfxMessageBox(msg);
			delete[] qRows;
			return;
		}
		if(hours < 1)
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
			else if(str == "Hourly")
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
		//saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR,i,"category",value1,fpSet);
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

		//saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR,i,"valOp",value1,fpSet);
		//value
		m_grid.GetCell(4, i, &cell);
		qRows[i].value = cell.GetNumber();
		// value
		value1.Format("%8.2f",qRows[i].value);
		//saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR,i,"value",value1,fpSet);
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
		//saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_EVENT_LOCATOR,i,"valType",value1,fpSet);
	}

	//saveRun.SaveGeneral(0, runDesc, fpSet);
	//saveRun.SaveClimateOptions(curr_runID, fpSet);
	// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
	//saveRun.SaveFireOptions(curr_runID, fpSet);
	//ok, qRows complete
	int *varIDs = new int[rows];
	for(i = 0; i < rows; i++)
		varIDs[i] = qRows[i].varID;
	fpSet->Edit();
	fpSet->m_PeriodLength = "1";
	fpSet->Update();
	EnableWindow(FALSE);
	CDiurnalAnalysis2 analysis;//fpSet->m_pDatabase);
	int ret = analysis.Analyze(this, fpSet, rows, varIDs);
	EnableWindow(TRUE);
	if(ret == 0)
	{
		analysis.LocateEvents(qRows, rows, hours, fpSet);

		//save the query info for this SIG/Station
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
				records.m_RowOp = hours;
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
		/*if(theApp.queryRows)
			delete[] theApp.queryRows;
		theApp.queryRows = qRows;
		theApp.nQrows = rows;
		theApp.eventHours = hours;*/
		CDialog::OnOK();
	}
	else
		delete[] qRows;
}

void CHourlyEventLocatorDlg::PostNcDestroy()
{
	climSet.Close();

	CDialog::PostNcDestroy();
}

void CHourlyEventLocatorDlg::BlackenFirstCell()
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

void CHourlyEventLocatorDlg::OnBnClickedAddrow()
{
	m_grid.AppendRow();
	m_removeButton.EnableWindow((m_grid.GetNumberRows() > 1));
	BlackenFirstCell();
}

void CHourlyEventLocatorDlg::OnBnClickedRemoverow()
{
	if(m_grid.GetNumberRows() > 1)
	{
		m_grid.DeleteRow(m_grid.GetCurrentRow());
	}
	BlackenFirstCell();
	m_removeButton.EnableWindow((m_grid.GetNumberRows() > 1));
}

void CHourlyEventLocatorDlg::OnBnClickedButtonLoadDefaults()
{
	if(IDYES == AfxMessageBox("Load default event criteria?", MB_YESNO))
	{
		m_grid.SetNumberRows(0);
		CFFPHourlyEventsSet tSet(records.m_pDatabase);
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
				hours = tSet.m_RowOp;
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
				str = "Hourly";
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

void CHourlyEventLocatorDlg::OnBnClickedButtonSetDefault()
{
	if(m_grid.GetNumberRows() <= 0)
	{
		AfxMessageBox("No criteria specified...");
		return;
	}
	if(IDYES == AfxMessageBox("Set event criteria as default?", MB_YESNO))
	{
		CUGCell cell;
		CFFPHourlyEventsSet tSet(records.m_pDatabase);
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
				tSet.m_RowOp = hours;
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
			climSet.setVarSortOrder(theApp.varSortOrder);
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

void CHourlyEventLocatorDlg::OnVarSort()
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
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
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
// UserBrowseDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "UserBrowseDialog.h"
#include "UserVarSet.h"
#include "DelRecsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CUserVarsDataSource::CUserVarsDataSource(void)
{
	records = NULL;
	totalRecs = 0;
	virRec = 0;
	//varSet = NULL;
	nCols = 0;
	colNames = NULL;
	//m_vars = NULL;
}

CUserVarsDataSource::~CUserVarsDataSource(void)
{
	/*if(varSet)
	{
		varSet->Close();
		delete varSet;
	}*/
	if(colNames)
		delete[] colNames;

}

long CUserVarsDataSource::GetNumRows()
{
	return totalRecs;
}

int CUserVarsDataSource::GetNumCols()
{
	if(records)
		return nCols;//records->GetFieldCount();
	return 0;
}

void CUserVarsDataSource::SetRecords(CODBCRecordset *_records)
{
	records = _records;
	totalRecs = 0;
	virRec = 0;
	CUserVarSet varSet(records->m_pDatabase);
	//varSet = new CUserVarSet(records->m_pDatabase);
	varSet.Open();
	//nCols = records->GetFieldCount();
	nCols = records->m_nFields;
	if(colNames)
		delete[] colNames;
	colNames = new CString[nCols];
	colNames[0] = "StationID";
	colNames[1] = "Date";
	for(int c = 2; c < records->m_nFields; c++)
	{
		CODBCFieldInfo fInfo;
		//records->GetFieldInfo(c, fInfo);
		records->GetODBCFieldInfo(c, fInfo);
		varSet.m_strFilter.Format("[UserVarID] = %d", atoi(fInfo.m_strName));
		varSet.Requery();
		colNames[c] = varSet.m_VarName;
		colNames[c].Trim();
		//table.GetColumnSet().Add(dtAfterLast);
		//CPVDTColumn col = table.GetColumnSet().GetItem (c);
		//col.SetHeading(varSet.m_VarName);
		//col.SetWidth(80);
		//col.SetDataType(dttDoubleFloat);
	}
	varSet.Close();
	//m_vars = new CDBVariant[nCols];

	records->MoveFirst();
	while(!records->IsEOF())
	{
		totalRecs++;
		records->MoveNext();
	}
	if(totalRecs > 0)
		records->MoveFirst();
}

int CUserVarsDataSource::SetCell(int col,long row,CUGCell *cell)
{
	if(col < 0 || row < 0)
		return UG_NA;
	//write the cells new contents to the database
	if(row >= totalRecs)
		return UG_NA;
	if(virRec != row)
	{
		records->Move(row - virRec);
		virRec = row;
	}
	records->Edit();
	CString str = cell->GetText();
	switch(col)
	{
	case 0:
	case 1:
		break;
	default:
		double var = cell->GetNumber();
		//records->SetFieldValue(col, var);
		records->Field(col) = var;

		break;
	}
	records->Update();

	return UG_SUCCESS;
}

int	CUserVarsDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	bool moved = false;
	if(row < 0)
	{
		if(col < nCols && col >= 0)
		{
			cell->SetText(colNames[col]);
			return UG_SUCCESS;
		}
		return UG_SUCCESS;
	}
	if(row == 0)
	{
		try
		{
			records->MoveFirst();
			moved = true;
		}
		catch(CDBException* e)
		{
			DisplayDBException(e);
			e->Delete();
			return UG_SUCCESS;
		}
	}
	else if(row == totalRecs - 1)
	{
		try
		{
			records->MoveLast();
			moved = true;
		}
		catch(CDBException* e)
		{
			DisplayDBException(e);
			e->Delete();
			return UG_SUCCESS;
		}
	}
	else if(row != virRec)
	{
		try
		{
			records->Move(row - virRec);
			moved = true;
		}
		catch(CDBException* e)
		{
			DisplayDBException(e);
			e->Delete();
			return UG_SUCCESS;
		}
	}
	//else//already fetched!
	//	return UG_NA;
	CDBVariant var;
	//for(int i = 0; i < records->GetFieldCount(); i++)
	//{
	if(col < 0 && row >= 0)
		cell->SetNumber(row + 1);
	else if(row >= 0)
	{
		if(moved)//get variants
		{
			for(int c = 0; c < nCols; c++)
			{
				records->GetFieldValue(c, m_vars[c]);
			}
		}
		//records->GetFieldValue(col, m_vars[col]);
		//switch(var.m_dwType)
		switch(m_vars[col].m_dwType)
		//switch(records->GetF)
		{
		case DBVT_STRING:
		case DBVT_ASTRING:
		case DBVT_WSTRING:
			//cell->SetText(*var.m_pstring);
			cell->SetText(records->Field(col).AsString());
			break;
		case DBVT_NULL:
			cell->SetText("");
			break;
		case DBVT_BOOL:
			cell->SetBool(records->Field(col).AsBool());
			break;
		case DBVT_UCHAR:
			{
				char txt[2];
				txt[0] = records->Field(col).AsChar();
				txt[1] = 0;
				cell->SetText(txt);
			}
			break;
		case DBVT_SHORT:
			cell->SetNumber(records->Field(col).AsInt());
			break;
		case DBVT_LONG:
			cell->SetNumber(records->Field(col).AsLong());
			break;
		case DBVT_SINGLE:
			cell->SetNumber(records->Field(col).AsFloat());
			break;
		case DBVT_DOUBLE:
			cell->SetNumber(records->Field(col).AsDouble());
			break;
		case DBVT_DATE:
			{
				COleDateTime dateTime = records->Field(col).AsDate();
					//(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
					//var.m_pdate->minute, var.m_pdate->second);
				cell->SetText(dateTime.Format("%m/%d/%y %H:%M:%S"));
				cell->SetDataType( UGCELLDATA_TIME );
				//cell->SetText("");
			}
			break;
		case DBVT_BINARY:
			cell->SetText("");
			break;
		default:
			cell->SetText(_T("*unknown*"));
		}
		/*{		
		case VT_BSTR:
			cell->SetText(var.pcVal);
			break;
		case VT_BYREF|VT_UI1:
			cell->SetText(_T(""));
            break;
        case VT_ARRAY|VT_UI1:
            cell->SetText(_T("LongBinary"));
            break;
        case VT_DISPATCH:
			cell->SetText(_T(""));
            break;
        case VT_UNKNOWN:
			cell->SetText(_T(""));
            break;
        case VT_VARIANT:
			cell->SetText(_T(""));
            break;
		case VT_EMPTY:
			cell->SetText(_T(""));
            break;
		case VT_UI1:
			cell->SetBool( var.bVal );
            break;
		case VT_I2:
			cell->SetNumber( var.iVal );
            break;
		case VT_I4:
			cell->SetNumber( var.lVal);
            break;
		case VT_R4:
			cell->SetNumber( var.fltVal);
            break;
		case VT_R8:
			cell->SetNumber( var.dblVal);
            break;
		case VT_BOOL:
			if(var.iVal)
				cell->SetBool(TRUE);
			else
				cell->SetBool(FALSE);
            break;
		case VT_CY:{
            //m_coCurrency = var;
			cell->SetText("$");
			//cell->SetDataType( UGCELLDATA_CURRENCY );
            break;
				   }
		case VT_DATE:{
            COleDateTime dateTime = var;
    		cell->SetText(dateTime.Format());
			cell->SetDataType( UGCELLDATA_TIME );
            break;
					 }
 		case VT_NULL:
			cell->SetText(_T(""));
            break;
        case VT_ERROR:
            cell->SetText(_T(""));
            break;
		default:
			cell->SetText(_T("*unknown*"));
		}*/
		/*{		
		case DBVT_STRING:
			cell->SetText(*var.m_pstring);
			break;
		case DBVT_NULL:
			cell->SetText("");
			break;
		case DBVT_BOOL:
			cell->SetBool(var.m_boolVal);
			break;
		case DBVT_UCHAR:
			{
				char txt[2];
				txt[0] = var.m_chVal;
				txt[1] = 0;
				cell->SetText(txt);
			}
			break;
		case DBVT_SHORT:
			cell->SetNumber(var.m_iVal);
			break;
		case DBVT_LONG:
			cell->SetNumber(var.m_lVal);
			break;
		case DBVT_SINGLE:
			cell->SetNumber(var.m_fltVal);
			break;
		case DBVT_DOUBLE:
			cell->SetNumber(var.m_dblVal);
			break;
		case DBVT_DATE:
			{
				CTime dateTime(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
					var.m_pdate->minute, var.m_pdate->second);
				cell->SetText(dateTime.Format("%m//%d//%y %H:%M:%S"));
				cell->SetDataType( UGCELLDATA_TIME );
				//cell->SetText("");
			}
			break;
		case DBVT_BINARY:
			cell->SetText("");
			break;
		default:
			cell->SetText(_T("*unknown*"));
		}*/
	}
	virRec = row;
	return UG_SUCCESS;
}

void CUserVarsDataSource::SortAscending(int col)
{
	switch(col)
	{
	case 0:
		records->m_strSort = "[StationID],[ObsDate]";
		break;
	case 1:
		records->m_strSort = "[ObsDate],[StationID]";
		break;
	default:
	{
		CODBCFieldInfo fInfo;
		records->GetODBCFieldInfo(col, fInfo);
		records->m_strSort.Format("[%s],[ObsDate],[StationID]", fInfo.m_strName);
		break;
	}
	}
	records->Requery();
	virRec = 0;
}

void CUserVarsDataSource::SortDescending(int col)
{
	switch(col)
	{
	case 0:
		records->m_strSort = "[StationID] DESC,[ObsDate]";
		break;
	case 1:
		records->m_strSort = "[ObsDate] DESC,[StationID]";
		break;
	default:
	{
		CODBCFieldInfo fInfo;
		records->GetODBCFieldInfo(col, fInfo);
		records->m_strSort.Format("[%s] DESC,[ObsDate],[StationID]", fInfo.m_strName);
		break;
	}
	}
	records->Requery();
	virRec = 0;
}

UserBrowseCUG::UserBrowseCUG()
{
}

UserBrowseCUG::~UserBrowseCUG()
{
}

void UserBrowseCUG::OnSetup()
{
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(m_data.nCols);
	int wid = rect.Width() / 30;
	SetColWidth(-1, wid * 4);
	SetColWidth(0, wid * 4);
	SetColWidth(1, wid * 7);
	CUGCell cell;
	GetColDefault( 0, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 0, &cell );
	GetColDefault( 1, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 1, &cell );
	for(int c = 2; c < m_data.nCols; c++)
	{
		SetColWidth(c, wid * 4);
		GetColDefault( c, &cell );
		cell.SetNumberDecimals(2);
		cell.SetParam(USE_COXNUMBER);
		SetColDefault( c, &cell );
	}

	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 

	EnableMenu(TRUE);

}

void UserBrowseCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	//DrawExcelFocusRect(dc,rect);
	
	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

COLORREF UserBrowseCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

void UserBrowseCUG::OnMenuCommand(int col,long row,int section,int item)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 0)
	{ 
		if (item == ID_SORT_SORTASCENDING)
		{
			m_data.SortAscending(col);//(records);
			GotoRow(0);
			RedrawAll();
		}
		else if(item == ID_SORT_SORTDESCENDING)
		{
			m_data.SortDescending(col);
			GotoRow(0);
			RedrawAll();
		}
	}
}

int  UserBrowseCUG::OnMenuStart(int col,long row,int section)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 0)
	{ 

		//****** Empty the Menu!!
		EmptyMenu(); 

		//******* Add the Menu Items
		AddMenuItem(ID_SORT_SORTASCENDING,"Sort &Ascending");
		AddMenuItem(ID_SORT_SORTDESCENDING, "Sort &Descending");
	} 
	return TRUE;
}

void UserBrowseCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void UserBrowseCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void UserBrowseCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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

int UserBrowseCUG::OnEditStart(int col, long row,CWnd **edit)
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
		m_cutNumeric.SetGroupSeparator( ',' );
		m_cutNumeric.SetShowLeadingZero(FALSE);
		m_cutNumeric.SetFractionalDigitCount(2);
		m_cutNumeric.SetDecimalDigitCount(9);
		/*}
		else 
		{
			m_cutNumeric.SetFractionalDigitCount(0);
			m_cutNumeric.SetDecimalDigitCount(1);
		}*/
		*edit = &m_cutNumeric;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUserBrowseDialog dialog


CUserBrowseDialog::CUserBrowseDialog(CWnd* pParent /*=NULL*/, CODBCRecordset *_records /*= NULL*/)
	: CDialog(CUserBrowseDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserBrowseDialog)
	//}}AFX_DATA_INIT
	records = _records;
	virRec = 0;
	totalRecs = 0;
	m_grid.m_data.SetRecords(records);
}


void CUserBrowseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserBrowseDialog)
	//DDX_Control(pDX, IDC_DATATBLCTRL1, table);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserBrowseDialog, CDialog)
	//{{AFX_MSG_MAP(CUserBrowseDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_TRASH, OnTrash)
	//ON_COMMAND(ID_SORT_SORTASCENDING, OnSortSortascending)
	//ON_COMMAND(ID_SORT_SORTDESCENDING, OnSortSortdescending)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_PRINT, OnPrint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserBrowseDialog message handlers

BOOL CUserBrowseDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	EnableToolTips(TRUE);
	saveButton.AutoLoad(IDC_SAVE, this);
	printButton.AutoLoad(IDC_PRINT, this);
	saveButton.EnableWindow(FALSE);
	printButton.EnableWindow(FALSE);
	trashButton.AutoLoad(IDC_TRASH, this);
	rbCol = 1;
	CWaitCursor wait;
	//table.GetCellSet().SetNullString(" ");
	//table.GetRowSet().Reset();
	/*CUserVarSet varSet(records->m_pDatabase);
	varSet.Open();
	for(int c = 2; c < records->GetFieldCount(); c++)
	{
		CDaoFieldInfo fInfo;
		records->GetFieldInfo(c, fInfo);
		varSet.m_strFilter.Format("[UserVarID] = %d", atoi(fInfo.m_strName));
		varSet.Requery();
		
		//table.GetColumnSet().Add(dtAfterLast);
		//CPVDTColumn col = table.GetColumnSet().GetItem (c);
		//col.SetHeading(varSet.m_VarName);
		//col.SetWidth(80);
		//col.SetDataType(dttDoubleFloat);
	}
	varSet.Close();
	records->MoveFirst();
	while(!records->IsEOF())
	{
		totalRecs++;
		records->MoveNext();
	}*/
	records->MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);

	//table.VMReady(totalRecs); 
	//table.GetRowSet().SetTrapDeleteKey(TRUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserBrowseDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	CWaitCursor wait;
	if(m_grid.m_hWnd && printButton.m_hWnd)
	{
		CRect trect;
		printButton.GetWindowRect(&trect);
		int a = trect.Height() + 1, b = cy - 2 - trect.Height();
		if(b <= 0)
			b = 1;
		m_grid.MoveWindow(1, a, cx - 2, b);
	}
	/*if(table.m_hWnd && printButton.m_hWnd)
	{
		CRect trect;
		printButton.GetWindowRect(&trect);
		int a = trect.Height() + 1, b = cy - 2 - trect.Height();
		if(b <= 0)
			b = 1;
		table.MoveWindow(1, a, cx - 2, b);
	}*/
}

//BEGIN_EVENTSINK_MAP(CUserBrowseDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CUserBrowseDialog)
///	ON_EVENT(CUserBrowseDialog, IDC_DATATBLCTRL1, -605 /* MouseDown */, OnMouseDownDatatblctrl1, VTS_I2 VTS_I2 VTS_I4 VTS_I4)
//	ON_EVENT(CUserBrowseDialog, IDC_DATATBLCTRL1, 41 /* VMGetFirst */, OnVMGetFirstDatatblctrl1, VTS_PI4)
//	ON_EVENT(CUserBrowseDialog, IDC_DATATBLCTRL1, 42 /* VMGetPrev */, OnVMGetPrevDatatblctrl1, VTS_PI4)
//	ON_EVENT(CUserBrowseDialog, IDC_DATATBLCTRL1, 43 /* VMGetNext */, OnVMGetNextDatatblctrl1, VTS_PI4)
//	ON_EVENT(CUserBrowseDialog, IDC_DATATBLCTRL1, 44 /* VMGetPercent */, OnVMGetPercentDatatblctrl1, VTS_PI4)
//	ON_EVENT(CUserBrowseDialog, IDC_DATATBLCTRL1, 45 /* VMGetRowData */, OnVMGetRowDataDatatblctrl1, VTS_PI4)
//	ON_EVENT(CUserBrowseDialog, IDC_DATATBLCTRL1, 46 /* VMGetLast */, OnVMGetLastDatatblctrl1, VTS_PI4)
//	ON_EVENT(CUserBrowseDialog, IDC_DATATBLCTRL1, 53 /* RtClick */, OnRtClickDatatblctrl1, VTS_I2 VTS_I4 VTS_I4 VTS_I4 VTS_PI4)
//	ON_EVENT(CUserBrowseDialog, IDC_DATATBLCTRL1, 6 /* Change */, OnChangeDatatblctrl1, VTS_I4 VTS_I4)
//	ON_EVENT(CUserBrowseDialog, IDC_DATATBLCTRL1, 20 /* BeforeDeleteRow */, OnBeforeDeleteRowDatatblctrl1, VTS_I4 VTS_PI4)
	//}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

/*void CUserBrowseDialog::OnMouseDownDatatblctrl1(short Button, short Shift, long X, long Y) 
{
	rbMouse.x = X;
	rbMouse.y = Y;
}

void CUserBrowseDialog::OnVMGetFirstDatatblctrl1(long FAR* Status) 
{
	long Row = 0;
	table.SetVMDataKey(Row); // always set before setting data
	records->MoveFirst();
	SetRowData(Row);
	virRec = Row;
}

void CUserBrowseDialog::OnVMGetPrevDatatblctrl1(long FAR* Status) 
{
	long Row = table.GetVMDataKey();
	if (Row <= 0)
		*Status=1;
	else
	{
		Row--;
		table.SetVMDataKey(Row); // always set before setting data
		records->Move(Row - virRec);
		SetRowData(Row);
		virRec = Row;
	}
}

void CUserBrowseDialog::OnVMGetNextDatatblctrl1(long FAR* Status) 
{
	long Row = table.GetVMDataKey();
	if (Row >= totalRecs - 1)
		*Status=1;
	else
	{
		Row++;
		table.SetVMDataKey(Row); // always set before setting data
		records->Move(Row - virRec);
		SetRowData(Row);
		virRec = Row;
	}
}

void CUserBrowseDialog::OnVMGetPercentDatatblctrl1(long FAR* Status) 
{
	long Row = table.GetVMDataKey();
	// always set before setting data
	table.SetVMDataKey ((Row*totalRecs)/100); 
	records->Move(Row - virRec);
	SetRowData(Row);
	virRec = Row;
}

void CUserBrowseDialog::OnVMGetRowDataDatatblctrl1(long FAR* Status) 
{
	long Row = table.GetVMDataKey();
	if (Row < 0 || Row > totalRecs - 1)
		*Status=1;
	else
	{
		// VMDataKey does not need to be set
		if(virRec != Row)
		{
			records->Move(Row - virRec);
			virRec = Row;
		}
		SetRowData(Row);
	}
}

void CUserBrowseDialog::OnVMGetLastDatatblctrl1(long FAR* Status) 
{
	long Row = totalRecs - 1;
	table.SetVMDataKey(Row); // always set before setting data
	records->MoveLast();
	SetRowData(Row);
	virRec = Row;
}

void CUserBrowseDialog::OnRtClickDatatblctrl1(short Pane, long Row, long Column, long ClickPos, long FAR* Status) 
{
	if(ClickPos == posOnHeading)
	{
		rbCol = Column;
		CMenu menu;
		if (menu.LoadMenu(IDR_SORTMENU))
		{
			CMenu* pPopup = menu.GetSubMenu(0);
			ASSERT(pPopup != NULL);
			CRect rect;
			table.GetWindowRect(&rect);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				rbMouse.x + rect.left, rbMouse.y + rect.top, this); 
		}
	}	
}

void CUserBrowseDialog::OnChangeDatatblctrl1(long Row, long Column) 
{
	if(Row >= totalRecs)
		return;
	if(virRec != Row)
	{
		records->Move(Row - virRec);
		virRec = Row;
	}
	records->Edit();
	//for(int i = 2; i < records->GetFieldCount(); i++)
	//{
		//if(var.vt == VT_BSTR)
		//	rowData.PutItem(var.pcVal, i);
		//else
	CPVDTCell cell = table.GetCellSet().GetItem(Row, Column);
	COleVariant var = cell.GetValue();
	records->SetFieldValue(Column, var);
	//}

	records->Update();
}

void CUserBrowseDialog::OnBeforeDeleteRowDatatblctrl1(long Row, long FAR* Status) 
{
	CString strMsg;
	strMsg.Format("Permanently delete record %ld?", Row + 1);
	if(AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION) != IDYES)
	{
		*Status = 1;
		return;
	}
	CWaitCursor wait;
	if(Row != virRec)
		records->Move(Row - virRec);
	records->Delete();
	records->Requery();
	virRec = 0;
	totalRecs--;
}

void CUserBrowseDialog::SetRowData(long Row)
{
	CDTData rowData;
	rowData.CreateArray(table.GetColumnSet().GetCount());
	COleVariant var;
	for(int i = 0; i < records->GetFieldCount(); i++)
	{
		records->GetFieldValue(i, var);
		if(var.vt == VT_BSTR)
			rowData.PutItem(var.pcVal, i);
		else
			rowData.PutItem(var, i);
	}
	table.GetRowSet().GetItem(Row).SetValue(rowData);
	for(i = 0; i < records->GetFieldCount(); i++)
	{
		records->GetFieldValue(i, var);
		if(var.vt == VT_NULL)
			table.GetCellSet().GetItem(Row, i).SetNullFlag((var.vt == VT_NULL) ? TRUE : FALSE);
	}
}*/

void CUserBrowseDialog::OnTrash() 
{
	CDelRecsDialog drd(this, records, m_grid.m_data.GetNumRows(), m_grid.GetCurrentRow() + 1);
	if(drd.DoModal() == IDOK)
	{
		CWaitCursor wait;
		records->Requery();
		while(!records->IsEOF())
		{
			totalRecs++;
			records->MoveNext();
		}
		if(totalRecs == 0)
		{
			AfxMessageBox("All of the records have been deleted.\nData editor will close");
			CDialog::OnCancel();
			return;
		}
		virRec = 0;
		m_grid.m_data.SetRecords(records);
		int index = m_grid.GetDefDataSource();
		m_grid.SetGridUsingDataSource(index); 
		/*//reset table contents
		CWaitCursor wait;
		totalRecs = virRec = 0;
		table.GetRowSet().Reset();
		records->Requery();
		while(!records->IsEOF())
		{
			totalRecs++;
			records->MoveNext();
		}
		if(totalRecs == 0)
		{
			AfxMessageBox("All of the records have been deleted.\nData editor will close");
			CDialog::OnCancel();
			return;
		}
		table.VMReady(totalRecs); */
	}
}

/*void CUserBrowseDialog::OnSortSortascending() 
{
	switch(rbCol)
	{
	case 0:
		records->m_strSort = "[StationID],[ObsDate]";
		break;
	case 1:
		records->m_strSort = "[ObsDate],[StationID]";
		break;
	default:
	{
		CDaoFieldInfo fInfo;
		records->GetFieldInfo(rbCol, fInfo);
		records->m_strSort.Format("[%s],[ObsDate],[StationID]", fInfo.m_strName);
		break;
	}
	}
	records->Requery();
	virRec = 0;
	//table.GetRowSet().Reset();
	//table.VMReady(totalRecs);
	//table.GetCellSet().GetItem(0, rbCol).MakeVisible();
}

void CUserBrowseDialog::OnSortSortdescending() 
{
	switch(rbCol)
	{
	case 0:
		records->m_strSort = "[StationID],[ObsDate]";
		break;
	case 1:
		records->m_strSort = "[ObsDate],[StationID]";
		break;
	default:
	{
		CDaoFieldInfo fInfo;
		records->GetFieldInfo(rbCol, fInfo);
		records->m_strSort.Format("[%s] DESC,[ObsDate],[StationID]", fInfo.m_strName);
		break;
	}
	}
	CWaitCursor wait;
	//int c1 = table.GetColumnSet().GetFirstVisible(paneLeft),
	//	c2 = table.GetColumnSet().GetFirstVisible(paneRight);
	//table.SetRedraw(FALSE);
	records->Requery();
	virRec = 0;
	//table.GetRowSet().Reset();
	//table.VMReady(totalRecs);
	//table.SetRedraw(TRUE);
	//table.GetColumnSet().SetFirstVisible(paneLeft, c1);
	//table.GetColumnSet().SetFirstVisible(paneRight, c2);
	//table.Refresh();
}*/

void CUserBrowseDialog::OnSave() 
{
	// TODO: Add your control notification handler code here
	
}

void CUserBrowseDialog::OnPrint() 
{
	// TODO: Add your control notification handler code here
	
}

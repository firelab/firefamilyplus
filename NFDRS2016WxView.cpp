// NFDRS2016WxView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "NFDRS2016WxView.h"

CNFDRS2016WxDataSource::CNFDRS2016WxDataSource(void)
{
	records = NULL;
	totalRecs = 0;
	virRec = 0;
}

CNFDRS2016WxDataSource::~CNFDRS2016WxDataSource(void)
{

}

long CNFDRS2016WxDataSource::GetNumRows()
{
	return totalRecs;
}

int CNFDRS2016WxDataSource::GetNumCols()
{
	return 17;
}

int CNFDRS2016WxDataSource::SetCell(int col, long row, CUGCell *cell)
{
	if (col < 0 || row < 0)
		return UG_NA;
	//write the cells new contents to the database
	if (row >= totalRecs)
		return UG_NA;
	if (!records)
		return UG_NA;
	if (virRec != row)
	{
		records->Move(row - virRec);
		virRec = row;
	}
/*	records->Edit();
	CString str = cell->GetText();
	str = str.Trim();
	switch (col)
	{
	case 0:
	case 1:
		break;
	case 2:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_Temp);
		else
			records->m_Temp = cell->GetNumber();;
		break;
	case 3:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_RH);
		else
			records->m_RH = cell->GetNumber();;
		break;
	case 4:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_TmpMin);
		else
			records->m_TmpMin = cell->GetNumber();;
		break;
	case 5:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_TmpMax);
		else
			records->m_TmpMax = cell->GetNumber();;
		break;
	case 6:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_RHMin);
		else
			records->m_RHMin = cell->GetNumber();;
		break;
	case 7:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_HourlyPrecip);
		else
			records->m_HourlyPrecip = cell->GetNumber();;
		break;
	case 8:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_PPTAMT);
		else
			records->m_PPTAMT = cell->GetNumber();;
		break;
	case 9:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_SolarRadiation);
		else
			records->m_SolarRadiation = cell->GetNumber();;
		break;
	case 10:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_WS);
		else
			records->m_WS = cell->GetNumber();;
		break;
	case 11:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_SnowFlag);
		else
			records->m_SnowFlag = cell->GetNumber();;
		break;
	case 12:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FM1);
		else
			records->m_FM1 = cell->GetNumber();;
		break;
	case 13:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FM10);
		else
			records->m_FM10 = cell->GetNumber();;
		break;
	case 14:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FM100);
		else
			records->m_FM100 = cell->GetNumber();;
		break;
	case 15:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FM1000);
		else
			records->m_FM1000 = cell->GetNumber();;
		break;
	case 16:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FuelTemperature);
		else
			records->m_FuelTemperature = cell->GetNumber();;
		break;
	}


	records->Update();*/

	return UG_SUCCESS;
}

int	CNFDRS2016WxDataSource::GetCell(int col, long row, CUGCell *cell)
{
	if (!records)
		return UG_SUCCESS;
	CString str;
	if (row < 0)
	{
		switch (col)
		{
		case 0:
			cell->SetText("StationID");
			return UG_SUCCESS;
		case 1:
			cell->SetText("ObsDate");
			return UG_SUCCESS;
		case 2:
			cell->SetText("Temp(F)");
			return UG_SUCCESS;
		case 3:
			cell->SetText("RH");
			return UG_SUCCESS;
		case 4:
			cell->SetText("Min Temp");
			return UG_SUCCESS;
		case 5:
			cell->SetText("Max Temp");
			return UG_SUCCESS;
		case 6:
			cell->SetText("Min RH");
			return UG_SUCCESS;
		case 7:
			cell->SetText("Hourly Precip");
			return UG_SUCCESS;
		case 8:
			cell->SetText("24Hr Precip");
			return UG_SUCCESS;
		case 9:
			cell->SetText("SolarRad");
			return UG_SUCCESS;
		case 10:
			cell->SetText("Wind Speed");
			return UG_SUCCESS;
		case 11:
			cell->SetText("Snow Flag");
			return UG_SUCCESS;
		case 12:
			cell->SetText("1 Hour DFM");
			return UG_SUCCESS;
		case 13:
			cell->SetText("10 Hour DFM");
			return UG_SUCCESS;
		case 14:
			cell->SetText("100 Hour DFM");
			return UG_SUCCESS;
		case 15:
			cell->SetText("1000 Hour DFM");
			return UG_SUCCESS;
		case 16:
			cell->SetText("Fuel Temp (C)");
			return UG_SUCCESS;
		default:
			cell->SetText("");
			return UG_SUCCESS;
		}
	}
	if (records->IsBOF() && records->IsEOF())
		return UG_SUCCESS;
	if (row == 0)
		records->MoveFirst();
	else if (row == totalRecs - 1)
		records->MoveLast();
	else if (row != virRec)
		records->Move(row - virRec);
	switch (col)
	{
	case -1:
		cell->SetNumber(row + 1);
		break;
	case 0:
		if (!records->IsFieldNull(&records->m_StationID))
			cell->SetText(records->m_StationID);
		else
			cell->SetText("");
		break;
	case 1:
		if (!records->IsFieldNull(&records->m_ObsDate))
		{  
			str = records->m_ObsDate.Format("%m/%d/%y %H:%M");
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 2:
		if (!records->IsFieldNull(&records->m_Temp))
			cell->SetNumber(records->m_Temp);
		else
			cell->SetText("");
		break;
	case 3:
		if (!records->IsFieldNull(&records->m_RH))
			cell->SetNumber(records->m_RH);
		else
			cell->SetText("");
		break;
	case 4:
		if (!records->IsFieldNull(&records->m_TmpMin))
			cell->SetNumber(records->m_TmpMin);
		else
			cell->SetText("");
		break;
	case 5:
		if (!records->IsFieldNull(&records->m_TmpMax))
			cell->SetNumber(records->m_TmpMax);
		else
			cell->SetText("");
		break;
	case 6:
		if (!records->IsFieldNull(&records->m_RHMin))
			cell->SetNumber(records->m_RHMin);
		else
			cell->SetText("");
		break;
	case 7:
		if (!records->IsFieldNull(&records->m_HourlyPrecip))
			cell->SetNumber(records->m_HourlyPrecip);
		else
			cell->SetText("");
		break;
	case 8:
		if (!records->IsFieldNull(&records->m_PPTAMT))
			cell->SetNumber(records->m_PPTAMT);
		//if (!records->IsFieldNull(&records->m_WS))
		//	cell->SetNumber(records->m_WS);
		else
			cell->SetText("");
		break;
	case 9:
		if (!records->IsFieldNull(&records->m_SolarRadiation))
			cell->SetNumber(records->m_SolarRadiation);
		else
			cell->SetText("");
		break;
	case 10:
		if (!records->IsFieldNull(&records->m_WS))
			cell->SetNumber(records->m_WS);
		else
			cell->SetText("");
		break;
	case 11:
		if (!records->IsFieldNull(&records->m_SnowFlag))
			cell->SetNumber(records->m_SnowFlag);
		else
			cell->SetText("");
		break;
	case 12:
		if (!records->IsFieldNull(&records->m_FM1))
			cell->SetNumber(records->m_FM1);
		else
			cell->SetText("");
		break;
	case 13:
		if (!records->IsFieldNull(&records->m_FM10))
			cell->SetNumber(records->m_FM10);
		else
			cell->SetText("");
		break;
	case 14:
		if (!records->IsFieldNull(&records->m_FM100))
			cell->SetNumber(records->m_FM100);
		else
			cell->SetText("");
		break;
	case 15:
		if (!records->IsFieldNull(&records->m_FM1000))
			cell->SetNumber(records->m_FM1000);
		else
			cell->SetText("");
		break;
	case 16:
		if (!records->IsFieldNull(&records->m_FuelTemperature))
			cell->SetNumber(records->m_FuelTemperature);
		else
			cell->SetText("");
		break;
	}
	virRec = row;
	return UG_SUCCESS;
}

void CNFDRS2016WxDataSource::SetRecords(CNFDRS2016WxSet *_records)
{
	records = _records;
	totalRecs = 0;
	virRec = 0;
	if (!records->IsBOF() && !records->IsEOF())
	{
		records->MoveFirst();
		while (!records->IsEOF())
		{
			totalRecs++;
			records->MoveNext();
		}
		if (totalRecs > 0)
			records->MoveFirst();
	}
}

void CNFDRS2016WxDataSource::SortAscending(int col)
{
	switch (col)
	{
	case 0:
		records->m_strSort = "WXObs.[StationID],WXObs.[ObsDate]";
		break;
	case 1:
		records->m_strSort = "WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 2:
		records->m_strSort = "[Temp],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 3:
		records->m_strSort = "[RH],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 4:
		records->m_strSort = "[TmpMin],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 5:
		records->m_strSort = "[TmpMax],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 6:
		records->m_strSort = "[RHMin],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 7:
		records->m_strSort = "[HourlyPrecip],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 8:
		records->m_strSort = "[PPTAMT],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 9:
		records->m_strSort = "[SolarRadiation],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 10:
		records->m_strSort = "[WS],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 11:
		records->m_strSort = "[SnowFlag],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 12:
		records->m_strSort = "[FM_1],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 13:
		records->m_strSort = "[FM_10],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 14:
		records->m_strSort = "[FM_100],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 15:
		records->m_strSort = "[FM_1000],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 16:
		records->m_strSort = "[FuelTemperature],WXObs.[ObsDate],WXObs.[StationID]";
		break;
	}
	records->Requery();
	virRec = 0;
}

void CNFDRS2016WxDataSource::SortDescending(int col)
{
	switch (col)
	{
	case 0:
		records->m_strSort = "WXObs.[StationID] DESC,WXObs.[ObsDate]";
		break;
	case 1:
		records->m_strSort = "WXObs.[ObsDate] DESC,WXObs.[StationID]";
		break;
	case 2:
		records->m_strSort = "[Temp] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 3:
		records->m_strSort = "[RH] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 4:
		records->m_strSort = "[TmpMin] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 5:
		records->m_strSort = "[TmpMax] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 6:
		records->m_strSort = "[RHMin] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 7:
		records->m_strSort = "[HourlyPrecip] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 8:
		records->m_strSort = "[PPTAMT] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 9:
		records->m_strSort = "[SolarRadiation] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 10:
		records->m_strSort = "[WS] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 11:
		records->m_strSort = "[SnowFlag] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 12:
		records->m_strSort = "[FM_1] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 13:
		records->m_strSort = "[FM_10] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 14:
		records->m_strSort = "[FM_100] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 15:
		records->m_strSort = "[FM_1000] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;
	case 16:
		records->m_strSort = "[FuelTemperature] DESC,WXObs.[ObsDate],WXObs.[StationID]";
		break;

	}
	records->Requery();
	virRec = 0;
}

NFDRS2016WxCUG::NFDRS2016WxCUG()
{
}

NFDRS2016WxCUG::~NFDRS2016WxCUG()
{
}

void NFDRS2016WxCUG::OnSetup()
{
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(17);
	SetUniformRowHeight(TRUE);
	SetDoubleBufferMode(TRUE);
	SetVScrollMode(UG_SCROLLTRACKING);
	int wid = rect.Width() / 18;
	SetColWidth(-1, wid);
	SetColWidth(0, wid);
	SetColWidth(1, wid);
	SetColWidth(2, wid);
	SetColWidth(3, wid);
	SetColWidth(4, wid);
	SetColWidth(5, wid);
	SetColWidth(6, wid);
	SetColWidth(7, wid);
	SetColWidth(8, wid);
	SetColWidth(9, wid);
	SetColWidth(10, wid);
	SetColWidth(11, wid);
	SetColWidth(12, wid);
	SetColWidth(13, wid);
	SetColWidth(14, wid);
	SetColWidth(15, wid);
	SetColWidth(16, wid);

	CUGCell cell;
	GetColDefault(0, &cell);
	cell.SetReadOnly(TRUE);
	SetColDefault(0, &cell);
	GetColDefault(1, &cell);
	cell.SetReadOnly(TRUE);
	SetColDefault(1, &cell);
	GetColDefault(2, &cell);
	SetColDefault(2, &cell);
	GetColDefault(3, &cell);
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(3, &cell);
	GetColDefault(4, &cell);
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(4, &cell);
	GetColDefault(5, &cell);
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(5, &cell);
	GetColDefault(6, &cell);
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(6, &cell);
	GetColDefault(7, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(7, &cell);
	GetColDefault(8, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(8, &cell);
	GetColDefault(9, &cell);
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(9, &cell);
	GetColDefault(10, &cell);
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(10, &cell);
	GetColDefault(11, &cell);
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(11, &cell);
	GetColDefault(12, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(12, &cell);
	GetColDefault(13, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(13, &cell);
	GetColDefault(14, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(14, &cell);
	GetColDefault(15, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(15, &cell);
	GetColDefault(16, &cell);
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(16, &cell);

	LockColumns(2);

	int index = AddDataSource(&m_data);
	SetDefDataSource(index);
	SetGridUsingDataSource(index);

	EnableMenu(TRUE);
}

void NFDRS2016WxCUG::OnDrawFocusRect(CDC *dc, RECT *rect)
{
	//DrawExcelFocusRect(dc,rect);

	rect->bottom--;
	rect->right--;
	dc->DrawFocusRect(rect);
}

COLORREF NFDRS2016WxCUG::OnGetDefBackColor(int section)
{
	if (section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);
}

void NFDRS2016WxCUG::OnMenuCommand(int col, long row, int section, int item)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 0)
	{
		if (item == ID_SORT_SORTASCENDING)
		{
			m_data.SortAscending(col);//(records);
			GotoRow(0);
			RedrawAll();
		}
		else if (item == ID_SORT_SORTDESCENDING)
		{
			m_data.SortDescending(col);
			GotoRow(0);
			RedrawAll();
		}
	}
}

int  NFDRS2016WxCUG::OnMenuStart(int col, long row, int section)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 0)
	{
		//****** Empty the Menu!!
		EmptyMenu();

		//******* Add the Menu Items
		AddMenuItem(ID_SORT_SORTASCENDING, "Sort &Ascending");
		AddMenuItem(ID_SORT_SORTDESCENDING, "Sort &Descending");
	}
	return TRUE;
}

void NFDRS2016WxCUG::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void NFDRS2016WxCUG::OnCharDown(UINT *vcKey, BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit(*vcKey);
}

void NFDRS2016WxCUG::OnKeyDown(UINT *vcKey, BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);

	if (*vcKey == VK_TAB)
	{
		int col = GetCurrentCol();
		long row = GetCurrentRow();

		if (GetKeyState(VK_SHIFT) < 0)
		{
			col--;
			if (col < 0)
			{
				col = GetNumberCols() - 1;
				row--;
			}
		}
		else
		{
			col++;
			if (col >= GetNumberCols())
			{
				col = 0;
				row++;
			}
		}
		GotoCell(col, row);
	}
}

int NFDRS2016WxCUG::OnEditVerify(int col, long row, CWnd *edit, UINT *vcKey)
{
	// change space to backspace
	if (vcKey)
		if (*vcKey == 32)
			*vcKey = 8;

	return CUGCtrl::OnEditVerify(col, row, edit, vcKey);
}

int NFDRS2016WxCUG::OnEditStart(int col, long row, CWnd **edit)
{
	CUGCell cell;
	GetCellIndirect(col, row, &cell);
	DWORD dwStyle = 0;
	if (cell.GetParam() == USE_COXNUMBER)
	{
		if (!m_cutNumeric.GetSafeHwnd())
		{
			m_cutNumeric.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, COXMASK_ID);
			m_cutNumeric.m_ctrl = this;
		}
		m_cutNumeric.SetDecimalSeparator('.');
		m_cutNumeric.SetGroupSeparator(',');
		m_cutNumeric.SetShowLeadingZero(FALSE);
		if (col == 6)//precip
			m_cutNumeric.SetFractionalDigitCount(2);
		else if (col == 19)
			m_cutNumeric.SetFractionalDigitCount(2);
		else
			m_cutNumeric.SetFractionalDigitCount(0);
		switch (col)
		{
		case 3:
		case 9:
		case 15:
		case 24:
		case 25:
		case 26:
			m_cutNumeric.SetDecimalDigitCount(1);
			break;
		case 4:
		case 5:
		case 6:
		case 8:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 16:
		case 17:
		case 18:
			m_cutNumeric.SetDecimalDigitCount(3);
			break;
		case 7:
			m_cutNumeric.SetDecimalDigitCount(2);
			break;
		}
		*edit = &m_cutNumeric;
	}
	return TRUE;
}



// CNFDRS2016WxView

IMPLEMENT_DYNCREATE(CNFDRS2016WxView, CFormView)

CNFDRS2016WxView::CNFDRS2016WxView()
	: CFormView(IDD_NFDRS2016WXVIEW)
{
	records = NULL;
}

CNFDRS2016WxView::~CNFDRS2016WxView()
{
}

void CNFDRS2016WxView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNFDRS2016WxView, CFormView)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CNFDRS2016WxView diagnostics

#ifdef _DEBUG
void CNFDRS2016WxView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CNFDRS2016WxView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CNFDRS2016WxView message handlers
void CNFDRS2016WxView::SetRecords(CNFDRS2016WxSet *_records)
{
	records = _records;
	virRec = 0;
	totalRecs = 0;
	if (records)
		m_grid.m_data.SetRecords(records);
	//CFormView::OnInitialUpdate();

	//rbCol = 1;
	if (records)
		records->MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);
	m_grid.BestFit(0, m_grid.GetNumberCols() - 1, 2, UG_BESTFIT_TOPHEADINGS);

	CRect rect;
	GetClientRect(&rect);
	OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
}



void CNFDRS2016WxView::OnInitialUpdate()
{
	virRec = 0;
	totalRecs = 0;
	if (records)
		m_grid.m_data.SetRecords(records);
	CFormView::OnInitialUpdate();
	//m_grid.BestFit(1, m_grid.GetNumberCols() - 1, 1, UG_BESTFIT_TOPHEADINGS);
	//rbCol = 1;
	if (records)
		records->MoveFirst();
	//m_grid.AttachGrid(this, IDC_GRID);
	CRect rect;
	GetClientRect(&rect);
	OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
}


void CNFDRS2016WxView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	CWaitCursor wait;
	if (m_grid.m_hWnd)
	{
		//CRect trect;
		//int a = trect.Height() + 1, b = cy - 2 - trect.Height();
		//if(b <= 0)
		//b = 1;
		m_grid.MoveWindow(1, 1, cx - 2, cy - 2);
	}
}

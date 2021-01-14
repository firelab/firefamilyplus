// FiresView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "FiresView.h"
#include "fireplusset.h"
#include "WthrPrintDialog.h"
#include "DelRecsDialog.h"

#include "FireRegionSet.h"
#include "FireAgencySet.h"
#include "FireUnitSet.h"
#include "FireSubUnitSet.h"
#include "FireShapeFileExportDialog.h"
#include "utils.h"

CString causeText[9] =
{
	"Lightning",
	"Equipment Use",
	"Smoking",
	"Campfire",
	"Debris Burning",
	"Railroad",
	"Arson",
	"Children",
	"Miscellaneous",

};

CFireDataSource::CFireDataSource(void)
{
	records = NULL;
	totalRecs = 0;
	virRec = 0;
}

CFireDataSource::~CFireDataSource(void)
{
}

long CFireDataSource::GetNumRows()
{
	return totalRecs;
}

int CFireDataSource::GetNumCols()
{
	return 15;
}

int CFireDataSource::SetCell(int col,long row,CUGCell *cell)
{
	if(col < 0 || row < 0)
		return UG_NA;
	//write the cells new contents to the database
	if(row >= totalRecs)
		return UG_NA;
	if(!records)
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
		break;
	case 1:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_TotalAcres);
		else
			records->m_TotalAcres = cell->GetNumber();
		break;
	case 2:
		if(str.GetLength() <= 0)
		{
			records->SetFieldNull(&records->m_StatisticalCause);
		    records->SetFieldNull(&records->m_GeneralCause);
		} 
		else
		{
			//records->m_GeneralCause = cell->GetNumber();
			//records->m_StatisticalCause = cell->GetNumber();
			int tCause = cell->GetNumber();
			if (tCause > 0 && tCause <= 9)
			{
				CString tmp;
				if (!records->IsFieldNull(&records->m_AgencyID) && records->m_AgencyID >= 2 && records->m_AgencyID <= 5)
				{
					utils util;
					tCause = util.TranslateCauseToDOI(tCause, 1);
				}
			}
			records->m_GeneralCause = records->m_StatisticalCause = tCause;
		}
			break;
	case 3:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FireName);
		else
			records->m_FireName = cell->GetText();
		break;
	case 4:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FireNumber);
		else
			records->m_FireNumber = cell->GetText();;
		break;
	case 5:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_LatDD);
		else
			records->m_LatDD = cell->GetNumber();
		break;
	case 6:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_LatMM);
		else
			records->m_LatMM = cell->GetNumber();
		break;
	case 7:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_LatSS);
		else
			records->m_LatSS = cell->GetNumber();
		break;
	case 8:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_LonDD);
		else
			records->m_LonDD = cell->GetNumber();
		break;
	case 9:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_LonMM);
		else
			records->m_LonMM = cell->GetNumber();
		break;
	case 10:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_LonSS);
		else
			records->m_LonSS = cell->GetNumber();
		break;
	case 11:  // agency

	case 12: // region
	case 13: // unit
	case 14: // subunit
		 break;
	}
	records->Update();

	return UG_SUCCESS;
}

int	CFireDataSource::GetCell(int col,long row,CUGCell *cell)
{
	if(!records)
		return UG_SUCCESS;
	if(row >= totalRecs)
		return UG_NA;
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 0:
			cell->SetText("Discovery");
			return UG_SUCCESS;
		case 1:
			cell->SetText("Acres");
			return UG_SUCCESS;
		case 2:
			cell->SetText("Cause");
			return UG_SUCCESS;	
		case 3:
			cell->SetText("Fire Name");
			return UG_SUCCESS;	
		case 4:
			cell->SetText("Fire Num");
			return UG_SUCCESS;
		case 5:
			cell->SetText("Lat. (Deg)");
			return UG_SUCCESS;
		case 6:
			cell->SetText("(min)");
			return UG_SUCCESS;
		case 7:
			cell->SetText("(sec)");
			return UG_SUCCESS;
		case 8:
			cell->SetText("Lon. (Deg)");
			return UG_SUCCESS;
		case 9:
			cell->SetText("(min)");
			return UG_SUCCESS;
		case 10:
			cell->SetText("(sec)");
			return UG_SUCCESS;
		case 11:
			cell->SetText("Agency");
			return UG_SUCCESS;
		case 12:
			cell->SetText("Region");
			return UG_SUCCESS;
		case 13:
			cell->SetText("Unit");
			return UG_SUCCESS;
		case 14:
			cell->SetText("SubUnit");
			return UG_SUCCESS;
		default:
			cell->SetText("");
			return UG_SUCCESS;
		}
	}
	if(records->IsBOF() && records->IsEOF())
		return UG_SUCCESS;
	if(row == 0)
		records->MoveFirst();
	else if(row == totalRecs - 1)
		records->MoveLast();
	else if(row != virRec)
		records->Move(row - virRec);
	switch(col)
	{
	case -1:
		cell->SetNumber(row + 1);
		break;
	case 0:
		if(!records->IsFieldNull(&records->m_Discovery))
		{
			str = records->m_Discovery.Format("%m/%d/%y");//"%m/%d/%y");
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 1:
		if(!records->IsFieldNull(&records->m_TotalAcres))
			cell->SetNumber(records->m_TotalAcres);
		else
			cell->SetText("");
		break;
	case 2:
		{
			int tCause = 0;
			if (! records->IsFieldNull(&records->m_GeneralCause) && (records->m_GeneralCause > 0))
				//cell->SetNumber(records->m_GeneralCause);
				tCause = records->m_GeneralCause;
			else if(!records->IsFieldNull(&records->m_StatisticalCause))
				//cell->SetNumber(records->m_StatisticalCause);
				tCause = records->m_StatisticalCause;
			if(tCause > 0 && tCause <= 9)
			{
				CString tmp;
				if (!records->IsFieldNull(&records->m_AgencyID) && records->m_AgencyID >= 2 && records->m_AgencyID <= 5)
				{
					utils util;
					tCause = util.TranslateCauseToUSFS(tCause, records->m_AgencyID);
				}
				tmp.Format("%d %s", tCause, causeText[tCause - 1]);
				cell->SetText(tmp);
			}
			else
				cell->SetText("");
		}
		break;
	case 3:
		if(!records->IsFieldNull(&records->m_FireName))
			cell->SetText(records->m_FireName.Trim());
		else
			cell->SetText("");
		break;
	case 4:
		if(!records->IsFieldNull(&records->m_FireNumber))
			cell->SetText(records->m_FireNumber);
		else
			cell->SetText("");
		break;
	
	case 5:
		if(!records->IsFieldNull(&records->m_LatDD))
			cell->SetNumber(records->m_LatDD);
		else
			cell->SetText("");
		break;
	case 6:
		if(!records->IsFieldNull(&records->m_LatMM))
			cell->SetNumber(records->m_LatMM);
		else
			cell->SetText("");
		break;
	case 7:
		if(!records->IsFieldNull(&records->m_LatSS))
			cell->SetNumber(records->m_LatSS);
		else
			cell->SetText("");
		break;
	case 8:
		if(!records->IsFieldNull(&records->m_LonDD))
			cell->SetNumber(records->m_LonDD);
		else
			cell->SetText("");
		break;
	case 9:
		if(!records->IsFieldNull(&records->m_LonMM))
			cell->SetNumber(records->m_LonMM);
		else
			cell->SetText("");
		break;
	case 10:
		if(!records->IsFieldNull(&records->m_LonSS))
			cell->SetNumber(records->m_LonSS);
		else
			cell->SetText("");
		break;
	case 11:
		if(!records->IsFieldNull(&records->m_AgencyID)){
			CFireAgencySet agSet(records->m_pDatabase);
			CString strFilter;
            strFilter.Format("[AgencyID] = %d",records->m_AgencyID);
			agSet.m_strFilter = strFilter;
            agSet.Open();
			CString agencyStr;
			if (! agSet.IsEOF())
				agencyStr.Format("%s",agSet.m_Name);
			else
				agencyStr = "";
			agSet.Close();
			cell->SetText(agencyStr);
		} else
			cell->SetText("");
		break;
	case 12:
		if(!records->IsFieldNull(&records->m_RegionID)){
			CFireRegionSet regSet(records->m_pDatabase);
			CString strFilter;
            strFilter.Format("[RegionID] = %d AND [AgencyID] = %d",records->m_RegionID,records->m_AgencyID);
			regSet.m_strFilter = strFilter;
            regSet.Open();
			CString regionStr;
			if (! regSet.IsEOF())
				regionStr.Format("%s",regSet.m_Name);
			else
				regionStr = "";
			regSet.Close();
			cell->SetText(regionStr);
		} else
			cell->SetText("");
		break;
	case 13:
		if(!records->IsFieldNull(&records->m_UnitID)){
			CFireUnitSet unitSet(records->m_pDatabase);
			CString strFilter;
            strFilter.Format("[UnitID] = %d AND [RegionID] = %d ",records->m_UnitID,records->m_RegionID);
			unitSet.m_strFilter = strFilter;
            unitSet.Open();
			CString unitStr;
			if (! unitSet.IsEOF())
				unitStr.Format("%s",unitSet.m_Name);
			else
				unitStr = "";
			unitSet.Close();
			cell->SetText(unitStr);
		} else
			cell->SetText("");
		break;
	case 14:
		if(!records->IsFieldNull(&records->m_SubunitID)){
			CFireSubunitSet suSet(records->m_pDatabase);
			CString strFilter;
            strFilter.Format("[SubunitID] = %d AND [UnitID] = %d",records->m_SubunitID, records->m_UnitID);
			suSet.m_strFilter = strFilter;
            suSet.Open();
			CString suStr;
			if (! suSet.IsEOF())
				suStr.Format("%s",suSet.m_Name);
			else
				suStr = "";
			suSet.Close();
			cell->SetText(suStr);
		} else
			cell->SetText("");
		break;

	}
	
	virRec = row;
	return UG_SUCCESS;
}

void CFireDataSource::SetRecords(CFireEditSet *_records)
{
	records = _records;
	records->Requery();
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

void CFireDataSource::SortAscending(int col)
{
	switch(col)
	{
	case 0:
		records->m_strSort = "[Discovery]";
		break;
	case 1:
		records->m_strSort = "[TotalAcres],[Discovery]";
		break;
	case 2:
		records->m_strSort = "[StatisticalCause],[Discovery]";
		break;
	case 3:
		records->m_strSort = "[FireName],[Discovery]";
		break;
	case 4:
		records->m_strSort = "[FireNumber],[Discovery]";
		break;
	case 5:
	case 6:
	case 7:
		records->m_strSort = "[LatDD],[LatMM],[LatSS]";
		break;
	case 8:
	case 9:
	case 10:
		records->m_strSort = "[LonDD],[LonMM],[LonSS]";
		break;
	case 11:  // agency
	case 12: // region
	case 13: // unit
	case 14: // subunit
		records->m_strSort = "[AgencyID],[RegionID],[UnitID],[SubUnitID]";
		break;

	}
	records->Requery();
	virRec = 0;
}

void CFireDataSource::SortDescending(int col)
{
	switch(col)
	{
	case 0:
		records->m_strSort = "[Discovery] DESC";
		break;
	case 1:
		records->m_strSort = "[TotalAcres] DESC,[Discovery]";
		break;
	case 2:
		records->m_strSort = "[StatisticalCause] DESC,[Discovery]";
		break;
	case 3:
		records->m_strSort = "[FireName] DESC,[Discovery]";
		break;
	case 4:
		records->m_strSort = "[FireNumber] DESC,[Discovery]";
		break;
	case 5:
	case 6:
	case 7:
		records->m_strSort = "[LatDD] DESC,[LatMM] DESC,[LatSS] DESC";
		break;
	case 8:
	case 9:
	case 10:
		records->m_strSort = "[LonDD] DESC,[LonMM] DESC,[LonSS] DESC";
		break;
	case 11:
	case 12:
	case 13:
	case 14: // subunit
		records->m_strSort = "[AgencyID]DESC ,[RegionID] DESC,[UnitID] DESC,[SubUnitID] DESC";
		break;
	}
	records->Requery();
	virRec = 0;
}

FiresCUG::FiresCUG()
{
}

FiresCUG::~FiresCUG()
{
}

void FiresCUG::OnSetup()
{
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(15);
	SetUniformRowHeight(TRUE);
	SetDoubleBufferMode(TRUE);
	SetVScrollMode(UG_SCROLLTRACKING);
	//	int wid = rect.Width() / 40;
	int wid = 20;
	SetColWidth(-1, wid * 3);
	SetColWidth(0, wid * 5);
	SetColWidth(1, wid * 3);
	SetColWidth(2, wid * 3);
	SetColWidth(3, wid * 7);
	SetColWidth(4, wid * 4);
	SetColWidth(5, wid * 4);
	SetColWidth(6,wid * 2);
	SetColWidth(7, wid * 2);
	SetColWidth(8,wid * 4);
	SetColWidth(9,wid * 2);
	SetColWidth(10,wid * 2);
	SetColWidth(11, wid* 3); // agency
	SetColWidth(12, wid * 5); // region
	SetColWidth(13, wid * 5); // unit
	SetColWidth(14, wid * 5); // subunit

	CUGCell cell;
	GetColDefault( 0, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 0, &cell );
	GetColDefault( 1, &cell );
	cell.SetNumberDecimals(2);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault( 1, &cell );
	GetColDefault( 2, &cell );
	cell.SetNumberDecimals(0);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault( 2, &cell );
	LockColumns(1);

	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 

	EnableMenu(TRUE);

}

void FiresCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	//DrawExcelFocusRect(dc,rect);
	
	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

COLORREF FiresCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

void FiresCUG::OnMenuCommand(int col,long row,int section,int item)
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

int  FiresCUG::OnMenuStart(int col,long row,int section)
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

void FiresCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void FiresCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void FiresCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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

int FiresCUG::OnEditStart(int col, long row,CWnd **edit)
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
		if(col == 1)//acres
		{
			m_cutNumeric.SetFractionalDigitCount(1);
			m_cutNumeric.SetDecimalDigitCount(9);
		}
		else 
		{
			m_cutNumeric.SetFractionalDigitCount(0);
			m_cutNumeric.SetDecimalDigitCount(1);
		}
		*edit = &m_cutNumeric;
	}
	return TRUE;
}



// CFiresView

IMPLEMENT_DYNCREATE(CFiresView, CFormView)

CFiresView::CFiresView()
	: CFormView(CFiresView::IDD)
{
	records = NULL;
}

CFiresView::~CFiresView()
{
}

void CFiresView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFiresView, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(ID_DATA_EXPORT, &CFiresView::OnDataExport)
	ON_COMMAND(ID_DATA_PRINT, &CFiresView::OnDataPrint)
	ON_COMMAND(ID_DATA_DELETERECORDS, &CFiresView::OnDataDeleterecords)
	ON_UPDATE_COMMAND_UI(ID_DATA_DELETERECORDS, &CFiresView::OnUpdateDataDeleterecords)
	ON_UPDATE_COMMAND_UI(ID_DATA_EXPORT, &CFiresView::OnUpdateDataExport)
	ON_UPDATE_COMMAND_UI(ID_DATA_PRINT, &CFiresView::OnUpdateDataPrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CFiresView::OnUpdateFileSaveAs)
END_MESSAGE_MAP()


// CFiresView diagnostics

#ifdef _DEBUG
void CFiresView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFiresView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFiresView message handlers

void CFiresView::OnInitialUpdate()
{
	virRec = 0;
	totalRecs = 0;
	if(records)
		m_grid.m_data.SetRecords(records);
	CFormView::OnInitialUpdate();

	rbCol = 1;
	//records->MoveFirst();
	//m_grid.AttachGrid(this, IDC_GRID);
	CRect rect;
	GetClientRect(&rect);
	OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
}

void CFiresView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	CWaitCursor wait;
	if(m_grid.m_hWnd)
	{
		//CRect trect;
		//int a = trect.Height() + 1, b = cy - 2 - trect.Height();
		//if(b <= 0)
			//b = 1;
		m_grid.MoveWindow(1, 1, cx - 2, cy - 2);
	}
}

void CFiresView::OnDataExport()
{
	CFireShapeFileExportDialog fed(this, records,fpSet);
	fed.DoModal();	
}

void CFiresView::OnDataPrint()
{
	CWthrPrintDialog wpd(this, NULL, records);
	wpd.DoModal();
}

void CFiresView::OnDataDeleterecords()
{
	CDelRecsDialog drd(this, records, m_grid.m_data.GetNumRows(), m_grid.GetCurrentRow() + 1, FALSE);
	if(drd.DoModal() == IDOK)
	{

		long start, end;
		start = drd.m_start;//startCtl.GetValueInteger();
		end = drd.m_end;//endCtl.GetValueInteger();
		if(end < start)
		{
			//AfxMessageBox("ERROR: End record can not be less than start record.");
			return;
		}
		if(end > m_grid.m_data.GetNumRows())
			end = m_grid.m_data.GetNumRows();
		records->SetAbsolutePosition(start);
		records->Delete();
		for(int r = 0; r < end - start && !records->IsEOF(); r++)
		{
			records->MoveNext();
			if(!records->IsEOF())
				records->Delete();
		}
		
		//reset table contents
		m_grid.SetNumberRows(0);
		CWaitCursor wait;
		totalRecs = 0;
		virRec = 0;
		m_grid.m_data.SetRecords(records);
		int index = m_grid.GetDefDataSource();
		m_grid.SetGridUsingDataSource(index); 
	}
	m_grid.RedrawAll();
}

void CFiresView::OnUpdateDataDeleterecords(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_grid.GetNumberRows() > 0) ? 1 : 0);
}

void CFiresView::OnUpdateDataExport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_grid.GetNumberRows() > 0) ? 1 : 0);
}

void CFiresView::OnUpdateDataPrint(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_grid.GetNumberRows() > 0) ? 1 : 0);
}

void CFiresView::SetRecords(CFireEditSet *_records, CFireplusSet *_fpSet)
{
	records = _records;
	fpSet = _fpSet;
	virRec = 0;
	totalRecs = 0;
	if(records)
		m_grid.m_data.SetRecords(records);
	//CFormView::OnInitialUpdate();

	rbCol = 1;
	if(records)
		records->MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);
	CRect rect;
	GetClientRect(&rect);
	OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
}

void CFiresView::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

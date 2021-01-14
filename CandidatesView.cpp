// CandidatesView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "CandidatesView.h"
#include "CandidatesDoc.h"
#include "CandidatesFrame.h"
#include "DelRecsDialog.h"
#include <direct.h>

extern CFireplusApp theApp;

CCandidatesDataSource::CCandidatesDataSource(void)
{
	records = NULL;
	totalRecs = 0;
	virRec = 0;
	//pClimSet = NULL;
}

CCandidatesDataSource::~CCandidatesDataSource(void)
{
	/*if(pClimSet)
	{
		pClimSet->Close();
		delete pClimSet;
	}*/
}

long CCandidatesDataSource::GetNumRows()
{
	return totalRecs;
}

int CCandidatesDataSource::GetNumCols()
{
	return 23;
}

int CCandidatesDataSource::SetCell(int col,long row,CUGCell *cell)
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
	switch(col)
	{
	case 22:
		{
			CString str = cell->GetText();
			records->Edit();
			if(str.GetLength() <= 0)
				records->SetFieldNull(&records->m_Comment);
			else
				records->m_Comment = cell->GetText();
			records->Update();
		}
		break;
	default:
		 break;
	}

	return UG_SUCCESS;
}

int	CCandidatesDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 0:
			cell->SetText("SIG/Station");
			return UG_SUCCESS;
		case 1:
			cell->SetText("Years");
			return UG_SUCCESS;
		case 2:
			cell->SetText("Annual Filter");
			return UG_SUCCESS;	
		case 3:
			cell->SetText("Variable");
			return UG_SUCCESS;	
		case 4:
			cell->SetText("Model");
			return UG_SUCCESS;	
		case 5:
			cell->SetText("Greenup");
			return UG_SUCCESS;
		case 6:
			cell->SetText("Freeze");
			return UG_SUCCESS;
		case 7:
			cell->SetText("FD Type");
			return UG_SUCCESS;
		case 8:
			cell->SetText("FD R^2");
			return UG_SUCCESS;
		case 9:
			cell->SetText("FD Chi^2");
			return UG_SUCCESS;
		case 10:
			cell->SetText("FD P-Val");
			return UG_SUCCESS;
		case 11:
			cell->SetText("FD P-Range");
			return UG_SUCCESS;
		case 12:
			cell->SetText("LFD Acres");
			return UG_SUCCESS;
		case 13:
			cell->SetText("LFD R^2");
			return UG_SUCCESS;
		case 14:
			cell->SetText("LFD Chi^2");
			return UG_SUCCESS;
		case 15:
			cell->SetText("LFD P-Val");
			return UG_SUCCESS;
		case 16:
			cell->SetText("LFD P-Range");
			return UG_SUCCESS;
		case 17:
			cell->SetText("MFD Fires");
			return UG_SUCCESS;
		case 18:
			cell->SetText("MFD R^2");
			return UG_SUCCESS;
		case 19:
			cell->SetText("MFD Chi^2");
			return UG_SUCCESS;
		case 20:
			cell->SetText("MFD P-Val");
			return UG_SUCCESS;
		case 21:
			cell->SetText("MFD P-Range");
			return UG_SUCCESS;
		case 22:
			cell->SetText("Comment");
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
	switch(col)
	{
	case -1:
		cell->SetNumber(row + 1);
		break;
	case 0:
		if(!records->IsFieldNull(&records->m_SIG_Station))
		{
			str = records->m_SIG_Station;//m_Discovery.Format("%m/%d/%y");//"%m/%d/%y");
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 1:
		if(!records->IsFieldNull(&records->m_StartYear) && !records->IsFieldNull(&records->m_EndYear))
		{
			str.Format("%d - %d", records->m_StartYear, records->m_EndYear);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 2:
		if (!records->IsFieldNull(&records->m_StartMonth) && !records->IsFieldNull(&records->m_EndMonth)
			&& !records->IsFieldNull(&records->m_StartDay) && !records->IsFieldNull(&records->m_EndDay))
		{
			str.Format("%d/%d - %d/%d", MonthChtoInt(records->m_StartMonth.Trim()), records->m_StartDay,
				MonthChtoInt(records->m_EndMonth.Trim()), records->m_EndDay);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 3:
		if(!records->IsFieldNull(&records->m_VarName))
			cell->SetText(records->m_VarName);
		else
			cell->SetText("");
		break;
	case 4:
		if(!records->IsFieldNull(&records->m_MODEL))
			cell->SetText(records->m_MODEL);
		else
			cell->SetText("");
		break;
	case 5:
		if(!records->IsFieldNull(&records->m_GreenJulian))
		{
			str.Format("%d/%d", records->m_GreenJulian.GetMonth(), records->m_GreenJulian.GetDay());
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	
	case 6:
		if(!records->IsFieldNull(&records->m_FreezeJulian))
		{
			str.Format("%d/%d", records->m_FreezeJulian.GetMonth(), records->m_FreezeJulian.GetDay());
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 7:
		if(!records->IsFieldNull(&records->m_FD_Type))
		{
			str.Format("%s", records->m_FD_Type.Trim());
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 8:
		if(!records->IsFieldNull(&records->m_FD_R2))
		{
			str.Format("%.2f", records->m_FD_R2);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 9:
		if(!records->IsFieldNull(&records->m_FD_Chi2))
		{
			str.Format("%.2f", records->m_FD_Chi2);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 10:
		if(!records->IsFieldNull(&records->m_FD_PVal))
		{
			str.Format("%.4f", records->m_FD_PVal);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 11:
		if(!records->IsFieldNull(&records->m_FD_PMin) && !records->IsFieldNull(&records->m_FD_PMax))
		{
			str.Format("%.2f - %.2f", records->m_FD_PMin, records->m_FD_PMax);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 12:
		if(!records->IsFieldNull(&records->m_LFD_Acres))
		{
			str.Format("%s", records->m_LFD_Acres.Trim());
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 13:
		if(!records->IsFieldNull(&records->m_LFD_R2))
		{
			str.Format("%.2f", records->m_LFD_R2);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 14:
		if(!records->IsFieldNull(&records->m_LFD_Chi2))
		{
			str.Format("%.2f", records->m_LFD_Chi2);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 15:
		if(!records->IsFieldNull(&records->m_LFD_PVal))
		{
			str.Format("%.4f", records->m_LFD_PVal);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 16:
		if(!records->IsFieldNull(&records->m_LFD_PMin) && !records->IsFieldNull(&records->m_LFD_PMax))
		{
			str.Format("%.2f - %.2f", records->m_LFD_PMin, records->m_LFD_PMax);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 17:
		if(!records->IsFieldNull(&records->m_MFD_NumFires))
		{
			str.Format("%s", records->m_MFD_NumFires.Trim());
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 18:
		if(!records->IsFieldNull(&records->m_MFD_R2))
		{
			str.Format("%.2f", records->m_MFD_R2);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 19:
		if(!records->IsFieldNull(&records->m_MFD_Chi2))
		{
			str.Format("%.2f", records->m_MFD_Chi2);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 20:
		if(!records->IsFieldNull(&records->m_MFD_PVal))
		{
			str.Format("%.4f", records->m_MFD_PVal);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 21:
		if(!records->IsFieldNull(&records->m_MFD_PMin) && !records->IsFieldNull(&records->m_MFD_PMax))
		{
			str.Format("%.2f - %.2f", records->m_MFD_PMin, records->m_MFD_PMax);
			cell->SetText(str);
		}
		else
			cell->SetText("");
		break;
	case 22:
		if(!records->IsFieldNull(&records->m_Comment))
			cell->SetText(records->m_Comment);
		else
			cell->SetText("");
		break;

	}
	
	virRec = row;
	return UG_SUCCESS;
}

void CCandidatesDataSource::SetRecords(CCandidateSet *_records)
{
	records = _records;
	totalRecs = 0;
	virRec = 0;
 	if(!records->IsEOF() || !records->IsBOF())
	{
		records->MoveFirst();
		while(!records->IsEOF())
		{
			totalRecs++;
			records->MoveNext();
		}
	}
	//set up climateSet
	/*if(!pClimSet)
	{
		pClimSet = new CClimateSet(records->m_pDatabase);
		pClimSet->m_strSort = "[VarID]";
		pClimSet->Open();
	}*/
	if(totalRecs > 0)
		records->MoveFirst();
}

//CandidatesCUG
CandidatesCUG::CandidatesCUG()
{
}

CandidatesCUG::~CandidatesCUG()
{
}

void CandidatesCUG::OnSetup()
{
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(23);
	SetUniformRowHeight(TRUE);
	SetDoubleBufferMode(TRUE);
	SetVScrollMode(UG_SCROLLTRACKING);
	//	int wid = rect.Width() / 40;
	int wid = 22;
	SetColWidth(-1, wid * 2);
	SetColWidth(0, wid * 4);
	SetColWidth(1, wid * 3);
	SetColWidth(2, wid * 4);
	SetColWidth(3, wid * 3);
	SetColWidth(4, wid * 3);
	SetColWidth(5, wid * 3);
	SetColWidth(6,wid * 3);
	SetColWidth(7, wid * 3);
	SetColWidth(8,wid * 3);
	SetColWidth(9,wid * 3);
	SetColWidth(10,wid * 3);
	SetColWidth(11,wid * 4);
	SetColWidth(12,wid * 3);
	SetColWidth(13, wid * 3);
	SetColWidth(14, wid * 3);
	SetColWidth(15, wid * 3);
	SetColWidth(16, wid * 4);
	SetColWidth(17, wid * 3);
	SetColWidth(18, wid * 3);
	SetColWidth(19, wid * 3);
	SetColWidth(20, wid * 3);
	SetColWidth(21, wid * 4);
	SetColWidth(22, wid * 10);

	CUGCell cell;
	for(int c = 0; c < 19; c++)
	{
		GetColDefault( c, &cell );
		cell.SetReadOnly(TRUE);
		SetColDefault( c, &cell );
	}
	LockColumns(5);

	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 

	EnableMenu(TRUE);

}

COLORREF CandidatesCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

void CandidatesCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void CandidatesCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void CandidatesCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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

void CandidatesCUG::OnMenuCommand(int col,long row,int section,int item)
{
	if (/*section == UG_GRID || */section == UG_TOPHEADING && col >= 0)
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


int  CandidatesCUG::OnMenuStart(int col,long row,int section)
{
	if (/*section == UG_GRID || */section == UG_TOPHEADING && col >= 0)
	{ 

		//****** Empty the Menu!!
		EmptyMenu(); 

		//******* Add the Menu Items
		AddMenuItem(ID_SORT_SORTASCENDING,"Sort &Ascending");
		AddMenuItem(ID_SORT_SORTDESCENDING, "Sort &Descending");
	} 
	return TRUE;
}

void CCandidatesDataSource::SortAscending(int col)
{
	switch(col)
	{
	case 0:
		records->m_strSort = "[SIG_Station],[StartYear],[EndYear]";
		//records->m_strSort = "[SIG_Station]";
		break;
	case 1:
		records->m_strSort = "[StartYear],[EndYear],[SIG_Station]";
		break;
	case 2:
		records->m_strSort = "[StartMonth],[StartDay],[EndMonth],[EndDay],[SIG_Station]";
		break;
	case 3:
		records->m_strSort = "[VarName],[SIG_Station]";
		break;
	case 4:
		records->m_strSort = "[MODEL],[SIG_Station]";
		break;
	case 5:
		records->m_strSort = "[GreenJulian],[SIG_Station]";
		break;
	case 6:
		records->m_strSort = "[FreezeJulian],[SIG_Station]";
		break;
	case 7:
		records->m_strSort = "[FD_Type],[SIG_Station]";
		break;
	case 8:
		records->m_strSort = "[FD_R2],[SIG_Station]";
		break;
	case 9:
		records->m_strSort = "[FD_Chi2],[SIG_Station]";
		break;
	case 10:
		records->m_strSort = "[FD_PVal],[SIG_Station]";
		break;
	case 11:
		records->m_strSort = "[FD_PMin],[FD_PMax],[SIG_Station]";
		break;
	case 12:
		records->m_strSort = "[LFD_Acres],[SIG_Station]";
		break;
	case 13:
		records->m_strSort = "[LFD_R2],[SIG_Station]";
		break;
	case 14:
		records->m_strSort = "[LFD_Chi2],[SIG_Station]";
		break;
	case 15:
		records->m_strSort = "[LFD_PVal],[SIG_Station]";
		break;
	case 16:
		records->m_strSort = "[LFD_PMin],[LFD_PMax],[SIG_Station]";
		break;
	case 17:
		records->m_strSort = "[MFD_NumFires],[SIG_Station]";
		break;
	case 18:
		records->m_strSort = "[MFD_R2],[SIG_Station]";
		break;
	case 19:
		records->m_strSort = "[MFD_Chi2],[SIG_Station]";
		break;
	case 20:
		records->m_strSort = "[MFD_PVal],[SIG_Station]";
		break;
	case 21:
		records->m_strSort = "[MFD_PMin],[MFD_PMax],[SIG_Station]";
		break;
	case 22:
		records->m_strSort = "[Comment],[SIG_Station]";
		break;
	}
	records->Requery();
	virRec = 0;
}

void CCandidatesDataSource::SortDescending(int col)
{
	switch(col)
	{
	case 0:
		records->m_strSort = "[SIG_Station] DESC,[StartYear] DESC,[EndYear]";
		break;
	case 1:
		records->m_strSort = "[StartYear] DESC,[EndYear],[SIG_Station]";
		break;
	case 2:
		records->m_strSort = "[StartMonth] DESC,[StartDay] DESC,[EndMonth] DESC,[EndDay] DESC,[SIG_Station]";
		break;
	case 3:
		records->m_strSort = "[VarName] DESC,[SIG_Station]";
		break;
	case 4:
		records->m_strSort = "[MODEL] DESC,[SIG_Station]";
		break;
	case 5:
		records->m_strSort = "[GreenJulian] DESC,[SIG_Station]";
		break;
	case 6:
		records->m_strSort = "[FreezeJulian] DESC,[SIG_Station]";
		break;
	case 7:
		records->m_strSort = "[FD_Type] DESC,[SIG_Station]";
		break;
	case 8:
		records->m_strSort = "[FD_R2] DESC,[SIG_Station]";
		break;
	case 9:
		records->m_strSort = "[FD_Chi2] DESC,[SIG_Station]";
		break;
	case 10:
		records->m_strSort = "[FD_PVal] DESC,[SIG_Station]";
		break;
	case 11:
		records->m_strSort = "[FD_PMax] DESC,[FD_PMin] DESC,[SIG_Station]";
		break;
	case 12:
		records->m_strSort = "[LFD_Acres] DESC,[SIG_Station]";
		break;
	case 13:
		records->m_strSort = "[LFD_R2] DESC,[SIG_Station]";
		break;
	case 14:
		records->m_strSort = "[LFD_Chi2] DESC,[SIG_Station]";
		break;
	case 15:
		records->m_strSort = "[LFD_PVal] DESC,[SIG_Station]";
		break;
	case 16:
		records->m_strSort = "[LFD_PMax] DESC,[LFD_PMin] DESC,[SIG_Station]";
		break;
	case 17:
		records->m_strSort = "[MFD_NumFires] DESC,[SIG_Station]";
		break;
	case 18:
		records->m_strSort = "[MFD_R2] DESC,[SIG_Station]";
		break;
	case 19:
		records->m_strSort = "[MFD_Chi2] DESC,[SIG_Station]";
		break;
	case 20:
		records->m_strSort = "[MFD_PVal] DESC,[SIG_Station]";
		break;
	case 21:
		records->m_strSort = "[MFD_PMax] DESC,[MFD_PMin] DESC,[SIG_Station]";
		break;
	case 22:
		records->m_strSort = "[Comment] DESC,[SIG_Station]";
		break;
	}
	records->Requery();
	virRec = 0;
}

/*int CandidatesCUG::OnEditStart(int col, long row,CWnd **edit)
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
}*/


// CCandidatesDlg dialog


// CCandidatesView

IMPLEMENT_DYNCREATE(CCandidatesView, CFormView)

CCandidatesView::CCandidatesView()
	: CFormView(CCandidatesView::IDD)
{
	records = NULL;

}

CCandidatesView::~CCandidatesView()
{
}

void CCandidatesView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCandidatesView, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(ID_VIEW_FIREDAY, &CCandidatesView::OnViewFireday)
	ON_COMMAND(ID_VIEW_LARGEFIREDAY, &CCandidatesView::OnViewLargefireday)
	ON_COMMAND(ID_VIEW_MULTIPLEFIREDAY, &CCandidatesView::OnViewMultiplefireday)
	ON_COMMAND(ID_DATA_EXPORT, &CCandidatesView::OnDataExport)
	ON_COMMAND(ID_DATA_PRINT, &CCandidatesView::OnDataPrint)
	ON_COMMAND(ID_DATA_DELETERECORDS, &CCandidatesView::OnDataDeleterecords)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FIREDAY, &CCandidatesView::OnUpdateViewFireday)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LARGEFIREDAY, &CCandidatesView::OnUpdateViewLargefireday)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MULTIPLEFIREDAY, &CCandidatesView::OnUpdateViewMultiplefireday)
END_MESSAGE_MAP()


// CCandidatesView drawing

/*void CCandidatesView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}*/


// CCandidatesView diagnostics

#ifdef _DEBUG
void CCandidatesView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCandidatesView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCandidatesView message handlers

void CCandidatesView::OnInitialUpdate()
{
	virRec = 0;
	totalRecs = 0;
	if(records)
		m_grid.m_data.SetRecords(records);
	CFormView::OnInitialUpdate();

	rbCol = 1;
	if(records && !records->IsEOF() && !records->IsBOF())
		records->MoveFirst();
	//m_grid.AttachGrid(this, IDC_GRID);
	CRect rect;
	GetClientRect(&rect);
	OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
}

void CCandidatesView::OnSize(UINT nType, int cx, int cy)
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

void CCandidatesView::ResetRecords(CCandidateSet *_records)
{
	records = _records;
	virRec = 0;
	totalRecs = 0;
	int index = m_grid.GetDefDataSource();//AddDataSource(&m_data); 
	m_grid.RemoveDataSource(index);

	m_grid.m_data.SetRecords(records);
	index = m_grid.AddDataSource(&m_grid.m_data);

	m_grid.SetDefDataSource(index);
	m_grid.SetGridUsingDataSource(index); 

	CRect rect;
	GetClientRect(&rect);
	OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
	//m_grid.
}

void CCandidatesView::SetRecords(CCandidateSet *_records)
{
	records = _records;
	virRec = 0;
	totalRecs = 0;
	if(records)
		m_grid.m_data.SetRecords(records);
	//CFormView::OnInitialUpdate();

	rbCol = 1;
	if(records && !records->IsBOF() && !records->IsEOF())
		records->MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);
	CRect rect;
	GetClientRect(&rect);
	OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
	//m_grid.RedrawAll();
}


void CCandidatesView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	int wid = 22;
	CCandidatesDoc *pDoc = (CCandidatesDoc *)GetDocument();
	if(pDoc->m_FireDay)
	{
		m_grid.SetColWidth(8, wid * 3);
		m_grid.SetColWidth(9, wid * 3);
		m_grid.SetColWidth(10, wid * 3);
		m_grid.SetColWidth(11, wid * 4);
	}
	else
	{
		m_grid.SetColWidth(8, 0);
		m_grid.SetColWidth(9, 0);
		m_grid.SetColWidth(10, 0);
		m_grid.SetColWidth(11, 0);
	}
	if(pDoc->m_LargeFireDay)
	{
		m_grid.SetColWidth(12, wid * 3);
		m_grid.SetColWidth(13, wid * 3);
		m_grid.SetColWidth(14, wid * 3);
		m_grid.SetColWidth(15, wid * 3);
		m_grid.SetColWidth(16, wid * 4);
	}
	else
	{
		m_grid.SetColWidth(12, 0);
		m_grid.SetColWidth(13, 0);
		m_grid.SetColWidth(14, 0);
		m_grid.SetColWidth(15, 0);
		m_grid.SetColWidth(16, 0);
	}
	if(pDoc->m_MultiFireDay)
	{
		m_grid.SetColWidth(17, wid * 3);
		m_grid.SetColWidth(18, wid * 3);
		m_grid.SetColWidth(19, wid * 3);
		m_grid.SetColWidth(20, wid * 3);
		m_grid.SetColWidth(21, wid * 4);
	}
	else
	{
		m_grid.SetColWidth(17, 0);
		m_grid.SetColWidth(18, 0);
		m_grid.SetColWidth(19, 0);
		m_grid.SetColWidth(20, 0);
		m_grid.SetColWidth(21, 0);
	}
	if(m_grid.m_hWnd)
		m_grid.RedrawAll();
}

void CCandidatesView::OnViewFireday()
{
	CCandidatesDoc *pDoc = (CCandidatesDoc *)GetDocument();
	pDoc->m_FireDay = !pDoc->m_FireDay;
	pDoc->UpdateAllViews(NULL);
	CCandidatesFrame *pFrame = (CCandidatesFrame *)this->GetParent();
	pFrame->m_candidatesBar.UpdateData(FALSE);
}

void CCandidatesView::OnViewLargefireday()
{
	CCandidatesDoc *pDoc = (CCandidatesDoc *)GetDocument();
	pDoc->m_LargeFireDay = !pDoc->m_LargeFireDay;
	pDoc->UpdateAllViews(NULL);
	CCandidatesFrame *pFrame = (CCandidatesFrame *)this->GetParent();
	pFrame->m_candidatesBar.UpdateData(FALSE);
}

void CCandidatesView::OnViewMultiplefireday()
{
	CCandidatesDoc *pDoc = (CCandidatesDoc *)GetDocument();
	pDoc->m_MultiFireDay = !pDoc->m_MultiFireDay;
	pDoc->UpdateAllViews(NULL);
	CCandidatesFrame *pFrame = (CCandidatesFrame *)this->GetParent();
	pFrame->m_candidatesBar.UpdateData(FALSE);
}

void CCandidatesView::OnDataExport()
{
	_chdir(theApp.userDir);
	CFileDialog fd(false, "csv", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Comma Seperated Values Files (*.csv)|*.csv||", this);
	if(fd.DoModal() == IDOK)
	{
		CCandidatesDoc *pDoc = (CCandidatesDoc *)GetDocument();
		FILE * out = fopen(fd.GetPathName(), "wt");
		if(!out)
		{
			CString str;
			str.Format("Error opening %s as output.", fd.GetPathName());
			AfxMessageBox(str);
			return;
		}
		fprintf(out, "SIG/Station,Years,Annual_Filter,Variable,Model,Greenup,Freeze,FD_Type");
		if(pDoc->m_FireDay)
			fprintf(out, ",FD_R^2,FD_Chi^2,FD_P-Val,FD_P-Range");
		if(pDoc->m_LargeFireDay)
			fprintf(out, ",LFD_Acres,LFD_R^2,LFD_Chi^2,LFD_P-Val,LFD_P-Range");
		if(pDoc->m_MultiFireDay)
			fprintf(out, ",MFD_Fires,MFD_R^2,MFD_Chi^2,MFD_P-Val,MFD_P-Range");
		fprintf(out, ",Comment\n");
		CString strCell;
		for(int r = 0; r < m_grid.GetNumberRows(); r++)
		{
			m_grid.QuickGetText(0, r, &strCell);
			fprintf(out, "%s", strCell.Trim());
			m_grid.QuickGetText(1, r, &strCell);
			fprintf(out, ",%s", strCell.Trim());
			m_grid.QuickGetText(2, r, &strCell);
			fprintf(out, ",%s", strCell.Trim());
			m_grid.QuickGetText(3, r, &strCell);
			fprintf(out, ",%s", strCell.Trim());
			m_grid.QuickGetText(4, r, &strCell);
			fprintf(out, ",%s", strCell.Trim());
			m_grid.QuickGetText(5, r, &strCell);
			fprintf(out, ",%s", strCell.Trim());
			m_grid.QuickGetText(6, r, &strCell);
			fprintf(out, ",%s", strCell.Trim());
			m_grid.QuickGetText(7, r, &strCell);
			fprintf(out, ",%s", strCell.Trim());
			if(pDoc->m_FireDay)
			{
				m_grid.QuickGetText(8, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(9, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(10, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(11, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
			}
			if(pDoc->m_LargeFireDay)
			{
				m_grid.QuickGetText(12, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(13, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(14, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(15, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(16, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
			}
			if(pDoc->m_MultiFireDay)
			{
				m_grid.QuickGetText(17, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(18, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(19, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(20, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
				m_grid.QuickGetText(21, r, &strCell);
				fprintf(out, ",%s", strCell.Trim());
			}
			m_grid.QuickGetText(22, r, &strCell);
			fprintf(out, ",%s\n", strCell.Trim());
		}
		fclose(out);
	}
	chdir(theApp.dbDir);//theApp.workDir);
}

void CCandidatesView::OnDataPrint()
{
}

void CCandidatesView::OnDataDeleterecords()
{
	if(m_grid.GetNumberRows() <= 0)
		return;
	//if(totalRecs <= 0)
	//	return;
	CDelRecsDialog drd(this, records, m_grid.m_data.GetNumRows(), m_grid.GetCurrentRow() + 1, FALSE);
	//EnableWindow(FALSE);
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
		/*//reset table contents
		CWaitCursor wait;
		records->Requery();
		totalRecs = 0;
		while(!records->IsEOF())
		{
			totalRecs++;
			records->MoveNext();
		}

		virRec = 0;
		m_grid.m_data.SetRecords(records);
		int index = m_grid.GetDefDataSource();
		m_grid.SetGridUsingDataSource(index); 
		m_grid.RedrawAll();
		//EnableWindow(TRUE);*/
	}
	m_grid.RedrawAll();
}

void CCandidatesView::OnUpdateViewFireday(CCmdUI *pCmdUI)
{
	CCandidatesDoc *pDoc = (CCandidatesDoc *)GetDocument();
	pCmdUI->SetCheck(pDoc->m_FireDay);
}

void CCandidatesView::OnUpdateViewLargefireday(CCmdUI *pCmdUI)
{
	CCandidatesDoc *pDoc = (CCandidatesDoc *)GetDocument();
	pCmdUI->SetCheck(pDoc->m_LargeFireDay);
}

void CCandidatesView::OnUpdateViewMultiplefireday(CCmdUI *pCmdUI)
{
	CCandidatesDoc *pDoc = (CCandidatesDoc *)GetDocument();
	pCmdUI->SetCheck(pDoc->m_MultiFireDay);
}

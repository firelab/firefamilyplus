// FireSelDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "FireSelDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Standard FireSelCUG construction/destruction
FireSelCUG::FireSelCUG()
{
}

FireSelCUG::~FireSelCUG()
{
}

void FireSelCUG::OnSetup()
{
	CRect rect;
	GetClientRect(&rect);
	m_nButtonIndex = AddCellType(&m_button);
	SetCurrentCellMode(2);
	SetNumberCols(4);
	SetUniformRowHeight(TRUE);
	SetDoubleBufferMode(TRUE);
	SetVScrollMode(UG_SCROLLTRACKING);
	//QuickSetText(0, -1, ""); 
	//QuickSetText(1, -1, "Discovery");
	//QuickSetText(2, -1, "Acres");
	//QuickSetText(3, -1, "Name");
	int wid = rect.Width() / 13;
	SetColWidth(-1, 0);
	SetColWidth(0, wid * 2);
	SetColWidth(1, wid * 3);
	SetColWidth(2, wid * 3);
	SetColWidth(3, wid * 4);
	CUGCell cell;
	GetColDefault( 0, &cell );
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	SetColDefault( 0, &cell );
	GetColDefault( 1, &cell );
	cell.SetCellType(m_nButtonIndex);
	//cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	SetColDefault( 1, &cell );
	GetColDefault( 2, &cell );
	cell.SetCellType(m_nButtonIndex);
	//cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	SetColDefault( 2, &cell );
	GetColDefault( 3, &cell );
	cell.SetCellType(m_nButtonIndex);
	//cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	SetColDefault( 3, &cell );

	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 


}

void FireSelCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	//DrawExcelFocusRect(dc,rect);
	
	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

COLORREF FireSelCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

int FireSelCUG::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	//UNREFERENCED_PARAMETER(ID);
	UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(msg);
	UNREFERENCED_PARAMETER(param);
	if(ID == m_nButtonIndex && msg == UGCT_BUTTONCLICK)
	{
		CUGCell cell;
		GetCell(col,row,&cell);
		CFireSelDialog *fsd = (CFireSelDialog *) GetParent();
		fsd->selRec = row;
		fsd->EndDialog( IDOK );
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CFireSelDialog dialog


CFireSelDialog::CFireSelDialog(CWnd* pParent /*=NULL*/, CFireEditSet *_records /*= NULL*/)
	: CDialog(CFireSelDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFireSelDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	totalRecs = 0;
	virRec = 0;
	records = _records;
	m_grid.m_data.SetRecords(records);
}


void CFireSelDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFireSelDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFireSelDialog, CDialog)
	//{{AFX_MSG_MAP(CFireSelDialog)
	ON_BN_CLICKED(IDC_SORTACRES, OnSortacres)
	ON_BN_CLICKED(IDC_SORTDATE, OnSortdate)
	ON_BN_CLICKED(IDC_SORTNAME, OnSortname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireSelDialog message handlers

//BEGIN_EVENTSINK_MAP(CFireSelDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CFireSelDialog)
	//}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

/*void CFireSelDialog::OnLfDblClickDatatabledb1(short Pane, long Row, long Column, long ClickPos, long FAR* Status) 
{
	// TODO: Add your control notification handler code here
	if(Row >= 0 && Row < totalRecs)
	{
		selRec = Row;
		CDialog::OnOK();
	}
}

void CFireSelDialog::OnVMGetFirstDatatabledb1(long FAR* Status) 
{
	long Row = 0;
	table.SetVMDataKey(Row); // always set before setting data
	records->MoveFirst();
	SetRowData(Row);
	virRec = Row;
}

void CFireSelDialog::OnVMGetPrevDatatabledb1(long FAR* Status) 
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

void CFireSelDialog::OnVMGetNextDatatabledb1(long FAR* Status) 
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

void CFireSelDialog::OnVMGetPercentDatatabledb1(long FAR* Status) 
{
	long Row = table.GetVMDataKey();
	// always set before setting data
	table.SetVMDataKey ((Row*totalRecs)/100); 
	records->Move(Row - virRec);
	SetRowData(Row);
	virRec = Row;
}

void CFireSelDialog::OnVMGetRowDataDatatabledb1(long FAR* Status) 
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

void CFireSelDialog::OnVMGetLastDatatabledb1(long FAR* Status) 
{
	long Row = totalRecs - 1;
	table.SetVMDataKey(Row); // always set before setting data
	records->MoveLast();
	SetRowData(Row);
	virRec = Row;	
}*/

//void CFireSelDialog::SetRowData(long Row)
//{
//	int loc = 0;
	//CDTData rowData;
	//rowData.CreateArray(3);
	//rowData.PutItem(records->m_Discovery, 0);
	//rowData.PutItem(records->m_TotalAcres, 1);
	//if(!records->IsFieldNull(&records->m_FireName))
	//	rowData.PutItem(records->m_FireName, 2);
	//table.GetRowSet().GetItem(Row).SetValue(rowData);
	
//}


BOOL CFireSelDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CWaitCursor wait;
	((CButton *)GetDlgItem(IDC_SORTDATE))->SetCheck(TRUE);
	//table.GetRowSet().Reset();
	records->MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);
	/*while(!records->IsEOF())
	{
		totalRecs++;
		records->MoveNext();
	}*/
	//table.VMReady(totalRecs); 
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFireSelDialog::OnSortacres() 
{
	CWaitCursor wait;
	records->m_strSort = "[TotalAcres] DESC";
	records->Requery();
	//table.GetRowSet().Reset();
	virRec = 0;
	//table.VMReady(totalRecs); 
	m_grid.m_data.SetRecords(records);
	m_grid.GotoRow(0);
	m_grid.RedrawAll();
}

void CFireSelDialog::OnSortdate() 
{
	CWaitCursor wait;
	records->m_strSort = "[Discovery]";
	records->Requery();
	//table.GetRowSet().Reset();
	virRec = 0;
	//table.VMReady(totalRecs); 
	m_grid.m_data.SetRecords(records);
	m_grid.GotoRow(0);
	m_grid.RedrawAll();
}

void CFireSelDialog::OnSortname() 
{
	CWaitCursor wait;
	records->m_strSort = "[FireName]";
	records->Requery();
	//table.GetRowSet().Reset();
	virRec = 0;
	//table.VMReady(totalRecs); 
	m_grid.m_data.SetRecords(records);
	m_grid.GotoRow(0);
	m_grid.RedrawAll();
}

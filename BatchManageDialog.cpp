// BatchManageDialog.cpp : implementation file
//
//

#include "stdafx.h"
#include "fireplus.h"
#include "BatchManageDialog.h"
#include "BatchEditDialog.h"
#include "BatchCreateDialog.h"

#include "RunsSet.h"
#include "RunSave.h"
#include "BatchSet.h"
#include "BatchRunsSet.h"

#include "ugctrl.h"
#include "UTEdit.h"
#include ".\batchmanagedialog.h"
#include "BatchOutputDoc.h"
#include "updialog2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool AnalysisRunProc(const CUPDUPDATA* pCUPDUPData)
{
	CBatchOutput* pBatch = (CBatchOutput*)pCUPDUPData->GetAppData();
	pBatch->pCUPDUPData = (CUPDUPDATA*)pCUPDUPData;
	pCUPDUPData->SetProgress(_T("Initializing.."),0);
	pBatch->RunBatch();

	return true;
}

CBatchManageDataSource::CBatchManageDataSource(void)
{
	records = NULL;
	//totalRecs = 0;
	virRec = 0;
	
}

CBatchManageDataSource::~CBatchManageDataSource(void)
{
}

long CBatchManageDataSource::GetNumRows()
{
	return totalRecs;
}

int CBatchManageDataSource::GetNumCols()
{
	return 4;
}

int CBatchManageDataSource::SetCell(int col,long row,CUGCell *cell)
{
	
	return UG_SUCCESS;
}



int	CBatchManageDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 0:
			cell->SetText("BatchID");
			return UG_SUCCESS;
		case 1:
			cell->SetText("Description");
			return UG_SUCCESS;
		case 2:
			cell->SetText("CreatedOn");
			return UG_SUCCESS;
		case 3:
			cell->SetText("LastRun");
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
		// cell->SetNumber(row + 1);
		break;
	case 0:
		if(!records->IsFieldNull(&records->m_batchID))
			cell->SetNumber(records->m_batchID);
		else
			cell->SetNumber(0);
		break;
	case 1:
		if(!records->IsFieldNull(&records->m_Description))
			cell->SetText(records->m_Description);
		else
			cell->SetText("");
		break;
	case 2:
		if(!records->IsFieldNull(&records->m_CreatedOn))
		{
			//str = records->m_ObsDate.Format();//"%m/%d/%y");
			cell->SetText(records->m_CreatedOn);
		}
		else
			cell->SetText("");
		break;
	case 3:
		if(!records->IsFieldNull(&records->m_LastRun))
			cell->SetText(records->m_LastRun);
		else
			cell->SetText("");
		break;
	
	}
	virRec = row;
	return UG_SUCCESS;
}

void CBatchManageDataSource::SetRecords(CBatchSet *_records)
{
	records = _records;
	totalRecs = 0;
	virRec = 0;
	
	// xxxx

	CString seek;
	seek.Format("[batchID] > -1");
	
	records->m_strFilter = seek;
	if (! records->IsOpen())
    	records->Open();

	records->Requery();

	//records->MoveFirst();
	while(!records->IsEOF())
	{
		totalRecs++;
		records->MoveNext();
	}
	if(totalRecs > 0)
		records->MoveFirst();
	
}

void CBatchManageDataSource::SortAscending(int col)
{
	switch(col)
	{
	case 0:
		records->m_strSort = "[batchID],[LastRun]";
		break;
	case 1:
		records->m_strSort = "[Description],[batchID]";
		break;
	case 2:
		records->m_strSort = "[CreatedOn],[Description]";
		break;
	case 3:
		records->m_strSort = "[LastRun],[Description]";
		break;
	
	}
	records->Requery();
	virRec = 0;
}

void CBatchManageDataSource::SortDescending(int col)
{
	switch(col)
	{
	case 0:
		records->m_strSort = "[batchID] DESC";
		break;
	case 1:
		records->m_strSort = "[Description] DESC";
		break;
	case 2:
		records->m_strSort = "[CreatedOn] DESC";
		break;
	case 3:
		records->m_strSort = "[LastRun] DESC";
		break;
	
	}
	records->Requery();
	virRec = 0;
}


BatchManageCUG::BatchManageCUG()
{
}

BatchManageCUG::~BatchManageCUG()
{
}

void BatchManageCUG::OnSetup()
{

	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(5);

    SetMultiSelectMode(false);

	int wid = rect.Width() / 28;

	SetColWidth(-1, 0);
	SetColWidth(0, wid * 3);
	SetColWidth(1, wid * 15);
	SetColWidth(2, wid * 5);
	SetColWidth(3, wid * 5);

	
    QuickSetText(0, -1, "BatchID");

	CUGCell cell;
	GetColDefault( 0, &cell );
	cell.SetReadOnly(TRUE);
	cell.SetNumberDecimals(0);
	SetColDefault( 0, &cell );


	GetColDefault( 1, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 1, &cell );

	GetColDefault( 2, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 2, &cell );

	GetColDefault( 3, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 3, &cell );




	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 
	
	EnableUpdate(TRUE);

}


COLORREF BatchManageCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}





void BatchManageCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void BatchManageCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void BatchManageCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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



/////////////////////////////////////////////////////////////////////////////
// CBatchManageDialog dialog


CBatchManageDialog::CBatchManageDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet, CBatchSet *_records /* = NULL*/)
	: CDialog(CBatchManageDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchManageDialog)
	
	//}}AFX_DATA_INIT
	records = _records;
	fpSet = _fpSet;
	virRec = 0;
	totalRecs = 0;
	m_grid.m_data.SetRecords(records);
}





void CBatchManageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CBatchManageDialog)
	
	
	//}}AFX_DATA_MAP
	
	
	
}


BEGIN_MESSAGE_MAP(CBatchManageDialog, CDialog)
	//{{AFX_MSG_MAP(CBatchManageDialog)
	ON_BN_CLICKED(IDC_BATCHEDIT, OnBatchEdit)
	ON_BN_CLICKED(IDC_BATCHDELETE,OnBatchDelete)
	ON_BN_CLICKED(IDC_BATCHNEW, OnBatchNew)
	
	//}}AFX_MSG_MAP
	


	ON_BN_CLICKED(IDC_BATCHRUN, OnBnClickedBatchrun)
	ON_BN_CLICKED(IDC_BATCHRUNAUTOSAVE, OnBnClickedBatchRunAutoSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchManageDialog message handlers





BOOL CBatchManageDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	EnableToolTips(TRUE);


	rbCol = 1;
	CWaitCursor wait;
	records->MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void BatchManageCUG::OnMenuCommand(int col,long row,int section,int item)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 1)
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

int  BatchManageCUG::OnMenuStart(int col,long row,int section)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 1)
	{ 

		//****** Empty the Menu!!
		EmptyMenu(); 

		//******* Add the Menu Items
		AddMenuItem(ID_SORT_SORTASCENDING,"Sort &Ascending");
		AddMenuItem(ID_SORT_SORTDESCENDING, "Sort &Descending");
	} 
	return TRUE;
}





void CBatchManageDialog::OnBatchNew(){
	
	EnableWindow(FALSE);

	

	CRunSaveViewSet records(fpSet->m_pDatabase);
	

	CString query, temp;
	
	CRunsSet records2(fpSet->m_pDatabase);
	records2.Open();
	
	
	//records.m_strFilter = _T(query + temp);
	CString strSQL = "SELECT *  FROM ffpRuns, ffpRunDescription WHERE ffpRuns.runID = ffpRunDescription.runID ORDER BY ffpRuns.runID";


	records.Open(CRecordset::dynaset, strSQL);
	
	// turn off automatic update of dialog data

	if(records.IsBOF() || records.IsEOF())
		AfxMessageBox("No runs found. You must run and save at least one report before creating a new batch.");
	else
	{
		CBatchCreateDialog bcd(this, fpSet, &records,&records2);
	
		
		if (bcd.DoModal()){
			
	

		}
	}
	records.Close();

	EnableWindow(TRUE);
	CDialog::OnCancel();
}

void CBatchManageDialog::OnBatchEdit(){

	int editID = -1;

	CUGCell cell;
	m_grid.GetCell(0, m_grid.GetCurrentRow(), &cell);
	cell.GetNumber(&editID);

	if (m_grid.GetCurrentRow() < 0 || editID < 0){
		AfxMessageBox("Please select a batch to edit or view.");
		return;

	}


	CString seek;
	seek.Format("[batchID] = %d", editID);
	
	//CBatchSet bSet(fpSet->m_pDatabase);
	records->m_strFilter = seek;
	if (! records->IsOpen())
    	records->Open();
	records->Requery();
	

	
	
	CRunSaveViewSet views(fpSet->m_pDatabase);
	

	CString query, temp;
	
	CRunsSet runs(fpSet->m_pDatabase);
	
    CString strSQL = "SELECT *  FROM ffpRuns, ffpRunDescription WHERE ffpRuns.runID = ffpRunDescription.runID ORDER BY ffpRuns.runID";

	views.Open(CRecordset::dynaset, strSQL);
	runs.Open();
	
	if(runs.IsBOF() || runs.IsEOF()){
		AfxMessageBox("ERROR: No runs found.");
		return;
	}

	
	
	if(records->IsEOF()){
		AfxMessageBox("Please select a batch to edit or view.");
		runs.Close();
		return;
	} else {


		CBatchRunsSet brSet(fpSet->m_pDatabase);
		seek.Format("[batchID] = %d", editID);

		brSet.m_strFilter = seek;
		brSet.Open();
		brSet.Requery();

		/* clear all include flags */
		CRunsSet runSet(fpSet->m_pDatabase);
		runSet.Open();
		while (! runSet.IsEOF()){
			runSet.Edit();
			runSet.m_BatchInc = false;
			runSet.Update();
			runSet.MoveNext();
		}

		/* set appropriate include flags */

		while (! brSet.IsEOF()){
			seek.Format("[runID] = %d", brSet.m_runID);
			
			runSet.m_strFilter = seek;
			runSet.Requery();
			if (! runSet.IsEOF()){
				runSet.Edit();
				runSet.m_BatchInc = true;
				runSet.Update();
			}
			brSet.MoveNext();

		}
		runSet.Close();
		brSet.Close();

		CBatchEditDialog bed(this, fpSet, &views,&runs, editID);
	
		
		if (bed.DoModal()){
			
			 m_grid.m_data.SetRecords(records);
			 int index = m_grid.GetDefDataSource();
			 m_grid.SetGridUsingDataSource(index); 	
			
			 m_grid.RedrawAll();	
	

		}
	}
	runs.Close();

}

void CBatchManageDialog::OnOK() {


}


void CBatchManageDialog::OnBatchDelete() 
{
	int editID = -1;

	CUGCell cell;
	m_grid.GetCell(0, m_grid.GetCurrentRow(), &cell);
	cell.GetNumber(&editID);
	
	long delRow = m_grid.GetCurrentRow();

	if (m_grid.GetCurrentRow() < 0 || editID < 0){
		AfxMessageBox("Please select a batch to delete.");
		return;

	}


	CString seek;
	seek.Format("[batchID] = %d", editID);

	//CBatchSet bSet(fpSet->m_pDatabase);
	records->m_strFilter = seek;
	if (! records->IsOpen())
    	records->Open();
	records->Requery();
	

	
	
	
	if(records->IsEOF()){
		AfxMessageBox("Please select a batch to delete.");
	
		return;
	} else {

        Description = records->m_Description;
        

		CBatchDeleteConfirmDialog bdd(this, fpSet, Description,editID,records);
	
		
		if (bdd.DoModal()){
		   // totalRecs --;
            // m_grid.m_data.totalRecs --;
			records->Requery();
	       m_grid.m_data.SetRecords(records);
		   int index = m_grid.GetDefDataSource();
		   m_grid.SetGridUsingDataSource(index); 	
			//m_grid.DeleteRow(delRow);
			
			m_grid.RedrawAll();
	

		}
	}
	
}





/////////////////////////////////////////////////////////////////////////////
// CBatchDeleteConfirmDialog dialog


CBatchDeleteConfirmDialog::CBatchDeleteConfirmDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet ,  CString _Description, int _editID, CBatchSet *_records)
	: CDialog(CBatchDeleteConfirmDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchEditDialog)
	//}}AFX_DATA_INIT

	records = _records;

	Description = _Description;
	fpSet = _fpSet;
	editID = _editID;


}


void CBatchDeleteConfirmDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BATCH_DESCRIPTION, Description);
}


BEGIN_MESSAGE_MAP(CBatchDeleteConfirmDialog, CDialog)
	//{{AFX_MSG_MAP(CBatchDeleteConfirmDialog)
	
	
	//}}AFX_MSG_MAP
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchDeleteDialog message handlers





BOOL CBatchDeleteConfirmDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CBatchDeleteConfirmDialog::OnOK() 
{
	///EnableWindow(FALSE);

	CString strMsg;

	// basic validation

	// find record
	CString seek;
	CString str;
	

	// basic validation

	
	//CBatchSet batchSet(fpSet->m_pDatabase);
	if (! records->IsOpen())
	   records->Open();

	records->m_strFilter.Format("[batchID] = %d", editID);
	records->Requery();
	if (records->IsEOF())
	{
		CString msg;
		msg.Format("ERROR: Batch ID not found.");
		AfxMessageBox(msg);
			
		return;
	}

	

	
	records->Delete();
	//m_grid
	

	CBatchRunsSet brSet(fpSet->m_pDatabase);
	
	brSet.Open();
    seek.Format("[batchID] = %d", editID);

	brSet.m_strFilter = seek;
	brSet.Requery();

	/* delete existing BatchRun entries */

	while(!brSet.IsEOF())
	{
		brSet.Delete();
		brSet.MoveNext();
	}
	brSet.Close();

	

	strMsg.Format("Batch deleted.");
	AfxMessageBox(strMsg);
	CDialog::OnOK();
	
}





void CBatchManageDialog::OnBnClickedBatchrun()
{
	int editID = -1;

	CUGCell cell;
	m_grid.GetCell(0, m_grid.GetCurrentRow(), &cell);
	cell.GetNumber(&editID);
	
	if (m_grid.GetCurrentRow() < 0 || editID < 0)
	{
		AfxMessageBox("Please select a batch to run.");
		return;
	}
	CBatchOutput *batch = new CBatchOutput();
	batch->batchID = editID;
	batch->docDir = fpSet->m_pDatabase->GetDatabaseName();
	for(int c = batch->docDir.GetLength() - 1; c >= 0; c--)
	{
		if(batch->docDir.GetAt(c) != '\\')
			batch->docDir.SetAt(c, 0);
		else
		{
			batch->docDir.SetAt(c, 0);
			break;
		}
	}
	batch->m_pDB = fpSet->m_pDatabase;
	batch->m_autoSave = FALSE;

	// update "LastRun" field
	
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%Y/%m/%d-%H:%M:%S");
	// update grid cell first

	m_grid.GetCell(3, m_grid.GetCurrentRow(), &cell);
	cell.SetText(dStr);
	m_grid.SetCell(3, m_grid.GetCurrentRow(), &cell);

	// update batch table
	CString seek, oldFilter;
	seek.Format("[batchID] = %d", editID);
	//CBatchSet bSet(fpSet->m_pDatabase);
	oldFilter = records->m_strFilter;
	records->m_strFilter = seek;
	if (! records->IsOpen())
    	records->Open();
	records->Requery();
	records->Edit();
	records->m_LastRun = dStr;
	
    records->Update();
	records->m_strFilter = oldFilter;
	records->Requery();
	records->MoveFirst();
	records->MoveFirst();

	//batch.RunBatch(editID, fpSet->m_pDatabase);
	EnableWindow(FALSE);
	CUPDialog cupDlg(m_hWnd, AnalysisRunProc, batch);
	cupDlg.DoModal();
	//runOutputs need to be displayed here, can not create windows from secondary thread...
	batch->GenerateReports();

	delete batch;
	EnableWindow(TRUE);
	CDialog::OnOK();
}

void CBatchManageDialog::OnBnClickedBatchRunAutoSave()
{
	int editID = -1;

	CUGCell cell;
	m_grid.GetCell(0, m_grid.GetCurrentRow(), &cell);
	cell.GetNumber(&editID);
	
	if (m_grid.GetCurrentRow() < 0 || editID < 0)
	{
		AfxMessageBox("Please select a batch to run.");
		return;
	}

	// update "LastRun" field
	
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%Y/%m/%d-%H:%M:%S");
	// update grid cell first

	m_grid.GetCell(3, m_grid.GetCurrentRow(), &cell);
	cell.SetText(dStr);
	m_grid.SetCell(3, m_grid.GetCurrentRow(), &cell);

	// update batch table
	CString oldFilter;
	CString seek;
	seek.Format("[batchID] = %d", editID);
	//CBatchSet bSet(fpSet->m_pDatabase);
	oldFilter = records->m_strFilter;
	records->m_strFilter = seek;
	if (! records->IsOpen())
    	records->Open();
	records->Requery();
	records->Edit();
	records->m_LastRun = dStr;
	
    records->Update();

	records->m_strFilter = oldFilter;
	records->Requery();
	records->MoveFirst();

	CBatchOutput *batch = new CBatchOutput();
	batch->batchID = editID;
	//batch->m_pDBName = fpSet->m_pDatabase->GetName();
	batch->m_pDB = fpSet->m_pDatabase;
	batch->m_autoSave = TRUE;
	//batch.RunBatch(editID, fpSet->m_pDatabase);
	EnableWindow(FALSE);
	CUPDialog cupDlg(m_hWnd, AnalysisRunProc, batch);
	cupDlg.DoModal();
	//need to save reports and graphs
	batch->SaveReports();
	CWaitCursor wait;
	//runOutputs need to be destroyed here
	delete batch;
	EnableWindow(TRUE);
	m_grid.RedrawAll();
}

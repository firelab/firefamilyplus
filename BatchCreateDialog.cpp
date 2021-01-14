// BatchCreateDialog.cpp : implementation file
//
//
#include "stdafx.h"
#include "fireplus.h"
#include "BatchCreateDialog.h"
#include "RunsSet.h"
#include "RunSave.h"
#include "BatchSet.h"
#include "BatchRunsSet.h"
#include "RunSaveViewSet.h"

#include "ugctrl.h"
#include "UTEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBatchCreateDataSource::CBatchCreateDataSource(void)
{
	records = NULL;
	records2 = NULL;
	totalRecs = 0;
	virRec = 0;
	
}

CBatchCreateDataSource::~CBatchCreateDataSource(void)
{
}

long CBatchCreateDataSource::GetNumRows()
{
	return totalRecs;
}

int CBatchCreateDataSource::GetNumCols()
{
	return 8;
}

int CBatchCreateDataSource::SetCell(int col,long row,CUGCell *cell)
{
	
	return UG_SUCCESS;
}



int	CBatchCreateDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 0:
			cell->SetText("Include?");
			return UG_SUCCESS;
		case 1:
			cell->SetText("RunID");
			return UG_SUCCESS;
		case 2:
			cell->SetText("SIG/Sta");
			return UG_SUCCESS;	
		case 3:
			cell->SetText("Years");
			return UG_SUCCESS;	
		case 4:
			cell->SetText("Ann. Filter");
			return UG_SUCCESS;	
		case 5:
			cell->SetText("Description");
			return UG_SUCCESS;	
		case 6:
			cell->SetText("CreatedOn");
			return UG_SUCCESS;
		case 7:
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
		if(!records->IsFieldNull(&records->m_BatchInc))
			cell->SetNumber(records->m_BatchInc);
		else
			cell->SetNumber(0);
		break;
	case 1:
		if(!records->IsFieldNull(&records->m_runID))
			cell->SetNumber(records->m_runID);
		else
			cell->SetText("");
		break;
     case 2:
	    if(!records->IsFieldNull(&records->m_SIG_Station))
		{
			//str = records->m_ObsDate.Format();//"%m/%d/%y");
			cell->SetText(records->m_SIG_Station);
		}
		else 
			cell->SetText("Station");
		break; 
	case 3:
	    if(!records->IsFieldNull(&records->m_StartYear) &&
			!records->IsFieldNull(&records->m_EndYear))
		{
			
			CString temp;
			temp.Format("%4d-%4d",records->m_StartYear,
				                records->m_EndYear);
			cell->SetText(temp);
		}
		else
			cell->SetText("");
		break;
	case 4:
	    if(!records->IsFieldNull(&records->m_StartMonth) &&
            !records->IsFieldNull(&records->m_EndMonth) &&
            !records->IsFieldNull(&records->m_StartDay) &&
			!records->IsFieldNull(&records->m_EndDay))
		{
			
			CString temp;
			CString month1,month2;
			
			month1.Format("%s",records->m_StartMonth);
			month2.Format("%s",records->m_EndMonth);
			month1.Trim();
			month2.Trim();
			month1.Truncate(3);
			month2.Truncate(3);
			
			temp.Format("%-3s %2d - %-3s %2d",month1,
				                records->m_StartDay,
				                month2,
								records->m_EndDay);
			cell->SetText(temp);
		}
		else 
			cell->SetText("");
		break;
	case 5:
		if(!records->IsFieldNull(&records->m_Description))
			cell->SetText(records->m_Description);
		else
			cell->SetText("");
		break;
	case 6:
		if(!records->IsFieldNull(&records->m_CreatedOn))
		{
			//str = records->m_ObsDate.Format();//"%m/%d/%y");
			cell->SetText(records->m_CreatedOn);
		}
		else
			cell->SetText("");
		break;
	case 7:
		if(!records->IsFieldNull(&records->m_LastRun))
			cell->SetText(records->m_LastRun);
		else
			cell->SetText("");
		break;
	
	
	}
	virRec = row;
	return UG_SUCCESS;
}

void CBatchCreateDataSource::SetRecords(CRunSaveViewSet *_records,CRunsSet *_records2)
{
	records = _records;
	records2 = _records2;

	totalRecs = 0;
	virRec = 0;
	records->MoveFirst();
	while(!records->IsEOF())
	{
		totalRecs++;
		records->MoveNext();
	}
	if(totalRecs > 0)
		records->MoveFirst();
	
}




BatchCreateCUG::BatchCreateCUG()
{
}

BatchCreateCUG::~BatchCreateCUG()
{
}

void BatchCreateCUG::OnSetup()
{
	m_iCheckBoxIndex = AddCellType( &m_checkBoxCT );

	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(8);

    SetMultiSelectMode(false);

	int wid = rect.Width() / 33;

	SetColWidth(-1, 0);
	SetColWidth(0, 50); // include
	SetColWidth(1, 60); // runID
	SetColWidth(2, 60); // SIG/Sta
	SetColWidth(3, 65); // years
	SetColWidth(4, 87);  // annual filter
	SetColWidth(5,370);  // description
	SetColWidth(6,120);  // created on
	SetColWidth(7,120);  // last run
	
    QuickSetText(0, -1, "Include?");

	CUGCell cell;
	GetColDefault( 0, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetNumber(1);
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 0, &cell );


	GetColDefault( 1, &cell );
	cell.SetReadOnly(TRUE);
	cell.SetNumberDecimals(0);
	SetColDefault( 1, &cell );

	GetColDefault( 2, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 2, &cell );

	GetColDefault( 3, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 3, &cell );

	GetColDefault( 4, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 4, &cell );

	GetColDefault( 5, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 5, &cell );

	GetColDefault( 6, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 6, &cell );

	GetColDefault( 7, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 7, &cell );

	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 
	
    SetUserSizingMode(2);   /* user resizable */

    ((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );

	EnableUpdate(TRUE);

}


COLORREF BatchCreateCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}


int BatchCreateCUG::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	//UNREFERENCED_PARAMETER(ID);
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(msg);
	//UNREFERENCED_PARAMETER(param);
    CUGCell cell;
    
	if( msg == UGCT_CHECKBOXSET)
	{ 
		int checked;
  		//check box value has changed....
        GetCell(col,row,&cell);
		cell.GetNumber(&checked);
		
		if(row == 0)
			m_data.records->MoveFirst();
		else if(row == m_data.totalRecs - 1)
			m_data.records->MoveLast();
		else if(row != m_data.virRec)
			m_data.records->Move(row - m_data.virRec);

        m_data.virRec = row;

		m_data.records2->m_strFilter.Format("[runID] = %d", m_data.records->m_runID);
		
		m_data.records2->Requery();

		if (m_data.records2->IsEOF()){
            CString msg;
		    msg.Format("ERROR: The selected run ID (%d) was not found.", m_data.records->m_runID);
		    AfxMessageBox(msg);
		
		    return TRUE;

		}

		m_data.records2->Edit();


		if (checked < 1){
		   cell.SetNumber(1);
			m_data.records2->m_BatchInc = 1;
		} else {
			cell.SetNumber(0);
			m_data.records2->m_BatchInc = 0;
		}
		
		m_data.records2->Update();

	
        m_data.records->Requery();
		m_data.records->MoveFirst();

		
		m_data.SetRecords(m_data.records,m_data.records2);
		int index = GetDefDataSource();
		SetGridUsingDataSource(index); 	

		SetCell(col,row,&cell);
		RedrawAll();
		
		
	}
	return TRUE;
}


void BatchCreateCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void BatchCreateCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void BatchCreateCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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
// CBatchCreateDialog dialog


CBatchCreateDialog::CBatchCreateDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet, CRunSaveViewSet *_records /* = NULL*/, CRunsSet *_records2 /* = NULL */)
	: CDialog(CBatchCreateDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchCreateDialog)
	
	//}}AFX_DATA_INIT
	records = _records;
	records2 = _records2;

	fpSet = _fpSet;
	virRec = 0;
	totalRecs = 0;
	sortOrder = 0;
	m_grid.m_data.SetRecords(records,records2);
}





void CBatchCreateDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CBatchCreateDialog)
	DDX_Text(pDX, IDC_EDIT_BATCH_DESCRIPTION, Description);
	DDX_Text(pDX, IDC_EDIT_RUN_STATION,filterSta);
	DDX_Text(pDX, IDC_EDIT_RUN_DESCRIPTION, filterDescription);
	//}}AFX_DATA_MAP
	
	
	
}


BEGIN_MESSAGE_MAP(CBatchCreateDialog, CDialog)
	//{{AFX_MSG_MAP(CBatchCreateDialog)
	
	ON_BN_CLICKED(IDC_BATCH_SELECTALL, OnBatchSelectAll)
	ON_BN_CLICKED(IDC_BATCH_DESELECTALL, OnBatchDeselectAll)
    ON_BN_CLICKED(IDFILTER2, OnFilter)
	ON_BN_CLICKED(IDC_BATCH_SORT,OnReverseSort)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchCreateDialog message handlers





BOOL CBatchCreateDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	EnableToolTips(TRUE);


	reverseButton = (CButton*)GetDlgItem(IDC_BATCH_SORT);
	createButton = (CButton*)GetDlgItem(IDOK);
	cancelButton = (CButton*)GetDlgItem(IDCANCEL);
    selectAllButton = (CButton*)GetDlgItem(IDC_BATCH_SELECTALL);
    deselectAllButton = (CButton*)GetDlgItem(IDC_BATCH_DESELECTALL);

	rbCol = 1;
	CWaitCursor wait;
	records->MoveFirst();
	records2->MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void BatchCreateCUG::OnMenuCommand(int col,long row,int section,int item)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 1)
	{ 
		
	}
}

int  BatchCreateCUG::OnMenuStart(int col,long row,int section)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 1)
	{ 

		//****** Empty the Menu!!
		EmptyMenu(); 

		
	} 
	return TRUE;
}




void CBatchCreateDialog::OnOK() 
{
	CString BatchDesc;
	int numIncludes=0;

	UpdateData();

	// Description from the edit box at the top of the dialog
	BatchDesc = Description;



	EnableWindow(FALSE);
	
	if (BatchDesc.GetLength() < 2){
		CString strMsg;
		strMsg.Format("ERROR: You must create a description for the new batch.\n");
		AfxMessageBox(strMsg);
		return;

	}

	// how many runs included?
	
	CRunsSet runSet(fpSet->m_pDatabase);
	runSet.Open();
	
	// basic validation
	if (! runSet.IsEOF())
		 runSet.MoveFirst();

	
	while(!runSet.IsEOF())
	{
		if (runSet.m_BatchInc)
			numIncludes ++;

		runSet.MoveNext();
			
	} 

	if (numIncludes < 2){
		CString strMsg;
		strMsg.Format("ERROR: At least two (2) runs must be included in this batch.\n");
		AfxMessageBox(strMsg);
		return;

	}

	runSet.Close();

	CBatchConfirmDialog rld(this, fpSet, BatchDesc);

	if (rld.DoModal()== IDOK) 
		CDialog::OnOK();
}


void CBatchCreateDialog::OnBatchSelectAll(){
	CRunsSet runSet(fpSet->m_pDatabase);
	runSet.Open();

	// basic validation
	if (! runSet.IsEOF())
		 runSet.MoveFirst();

	
	while(!runSet.IsEOF())
	{
		if (runSet.m_BatchInc != 1){
			runSet.Edit();
			runSet.m_BatchInc = 1;
			runSet.Update();
		}
		runSet.MoveNext();
			
	} 


	runSet.Close();
	m_grid.RedrawAll();
}

void CBatchCreateDialog::OnBatchDeselectAll(){
CRunsSet runSet(fpSet->m_pDatabase);
	runSet.Open();

	// basic validation
	if (! runSet.IsEOF())
		 runSet.MoveFirst();

	
	while(!runSet.IsEOF())
	{
		if (runSet.m_BatchInc == 1){
			runSet.Edit();
			runSet.m_BatchInc = 0;
			runSet.Update();
		}
		runSet.MoveNext();
			
	} 


	runSet.Close();
	m_grid.RedrawAll();

}




void CBatchCreateDialog::OnReverseSort(){
   sortOrder = !sortOrder;
   OnFilter();

}

// filter button pressed
void CBatchCreateDialog::OnFilter(){
   UpdateData();

   CString stringFilter;
   if ((filterDescription.GetLength() > 0) && (filterSta.GetLength() > 0)){
	   // note: filter doesn't work on this query, since it joins 2 tables (@%#&* microsoft)
       //stringFilter.Format("[ffpRuns.Description] LIKE '%%%s%%'  AND [ffpRunDescription.SIG/Station] LIKE '%%%s%%'",filterDescription,filterSta);    
	   //records->m_strFilter=_T(stringFilter);
       //records->Requery();
	   records->Close();
       CString strSQL;
	   strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID and ffpRuns.Description LIKE '%%%s%%' and ffpRunDescription.SIG_Station LIKE '%%%s%%') ORDER BY ffpRuns.runID", filterDescription,filterSta);
       if (sortOrder) // reverse sort
		   strSQL.Append(" DESC");

	   records->Open(CRecordset::dynaset, strSQL);
	   if (!records->IsEOF()){
          m_grid.m_data.SetRecords(records,records2);
	  
	      int index = m_grid.GetDefDataSource();
	      m_grid.SetGridUsingDataSource(index); 	
		
			
	      m_grid.RedrawAll();
	      return;
	   }
	
    } else
   if (filterDescription.GetLength() > 0){
	   
       //stringFilter.Format("ffpRuns.Description LIKE '%%%s%%'",filterDescription);    
	   //records->m_strFilter=_T(stringFilter);
       //records->Requery();
	   CString strSQL;
	   records->Close();
	   strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID and ffpRuns.Description LIKE '%%%s%%' ) ORDER BY ffpRuns.runID", filterDescription);

	   if (sortOrder) // reverse sort
		   strSQL.Append(" DESC");
	   records->Open(CRecordset::dynaset, strSQL);
       if (!records->IsEOF()){
          m_grid.m_data.SetRecords(records,records2);
	  
	      int index = m_grid.GetDefDataSource();
	      m_grid.SetGridUsingDataSource(index); 	
		
			
	      m_grid.RedrawAll();
	      return;
	   }
	
    } else
    if (filterSta.GetLength() > 0){
	 
      // stringFilter.Format("ffpRunDescription.SIG/Station LIKE '%%%s%%'",filterSta);    
	  // records->m_strFilter=_T(stringFilter);
      // records->Requery();
       CString strSQL;
	   records->Close();
       strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID and ffpRunDescription.SIG_Station LIKE '%%%s%%') ORDER BY ffpRuns.runID", filterSta);
	   if (sortOrder) // reverse sort
		   strSQL.Append(" DESC");

       records->Open(CRecordset::dynaset, strSQL);
       if (!records->IsEOF()){

          m_grid.m_data.SetRecords(records,records2);
	  
   	      int index = m_grid.GetDefDataSource();
	      m_grid.SetGridUsingDataSource(index); 	
		
			
	      m_grid.RedrawAll();
		  return;
	   }
	} 


	// no filters 
	   // catchall
        CString strSQL;
	   if (records->IsOpen())
	      records->Close();
       strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID ) ORDER BY ffpRuns.runID");
       
	   if (sortOrder) // reverse sort
		   strSQL.Append(" DESC");

       records->Open(CRecordset::dynaset, strSQL);

       m_grid.m_data.SetRecords(records,records2);
	  
	   int index = m_grid.GetDefDataSource();
	   m_grid.SetGridUsingDataSource(index); 	
		
			
	   m_grid.RedrawAll();

	

}

void CBatchCreateDialog::OnSize(UINT nType, int cx, int cy)
{
	
	CWaitCursor wait;
	
	if (cx < 550)
		cx = 550;
	if (cy < 300)
		cy = 300;


    CDialog::OnSize(nType, cx, cy);

	

	int a,b;

	if (m_grid && m_grid.m_hWnd){
	    a = 170;
	    
		b = cy - (a + 50);

		m_grid.MoveWindow(10, a, cx -15, b);
	    
      
        if (reverseButton->m_hWnd)
   	       reverseButton->MoveWindow(10,cy - 30,80,25);
        if (selectAllButton->m_hWnd)
   	       selectAllButton->MoveWindow(150,cy - 30,80,25);
		if (deselectAllButton->m_hWnd)
		   deselectAllButton->MoveWindow(250,cy-30,80,25);
		if (createButton->m_hWnd)
   	       createButton->MoveWindow(cx - 200,cy - 30,80,25);
		if (cancelButton->m_hWnd)
   	       cancelButton->MoveWindow(cx - 100,cy - 30,80,25);

	}

}


/////////////////////////////////////////////////////////////////////////////
// CBatchConfirmDialog dialog


CBatchConfirmDialog::CBatchConfirmDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet , CString _Description /* = NULL*/)
	: CDialog(CBatchConfirmDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBatchCreateDialog)
	//}}AFX_DATA_INIT
	Description = _Description;
	fpSet = _fpSet;

}


void CBatchConfirmDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_BATCH_DESCRIPTION, Description);
}


BEGIN_MESSAGE_MAP(CBatchConfirmDialog, CDialog)
	//{{AFX_MSG_MAP(CBatchConfirmDialog)
	
	
	//}}AFX_MSG_MAP
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBatchCreateDialog message handlers





BOOL CBatchConfirmDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CBatchConfirmDialog::OnOK() 
{
	///EnableWindow(FALSE);

	CString strMsg;

	// basic validation

	// find record
	CString seek;
	CString str;
	

	// basic validation

	

	if (! Description){
		CString strMsg;
		strMsg.Format("ERROR: A valid description must be entered.\n");
		AfxMessageBox(strMsg);
		return;
	}

	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%Y/%m/%d-%H:%M:%S");

	CBatchSet batchSet(fpSet->m_pDatabase);
	batchSet.m_strFilter.Format("[Description] = '%s'", Description);
	batchSet.Open();

	//seek.Format("[Description] = '%s'", Description);

	if (!batchSet.IsEOF())
		//if(batchSet.Find(AFX_DAO_FIRST, seek) != 0)
		{
			CString msg;
			msg.Format("ERROR: That batch description is already in use.");
			AfxMessageBox(msg);
			batchSet.Close();
			return;
		}
	batchSet.m_strFilter.Format("");
	batchSet.Requery();
	if (! batchSet.IsEOF()){
		batchSet.MoveLast();
		curr_batchID = batchSet.m_batchID + 1;
	} else
		curr_batchID = 1;

	
	batchSet.AddNew();



	batchSet.m_batchID = curr_batchID;
	batchSet.m_Description = Description;
	batchSet.m_CreatedOn = dStr;
	// no lastRun (until batch is actually run the first time)

	batchSet.Update();
	batchSet.Close();

	/* create ffpBatchRuns relationships */

	CRunsSet runSet(fpSet->m_pDatabase);
	runSet.Open();
	runSet.m_strFilter = "[BatchInc] = 1";
	runSet.Requery();

	CBatchRunsSet brSet(fpSet->m_pDatabase);
	
	brSet.Open();

	while( !runSet.IsEOF( ) ){
		brSet.AddNew();
		brSet.m_batchID = curr_batchID;
		brSet.m_runID = runSet.m_runID;
		brSet.Update();
		runSet.MoveNext();

	}
	brSet.Close();

	runSet.Close();


	strMsg.Format("The new batch has been created.");
	AfxMessageBox(strMsg);
	CDialog::OnOK();
	
}

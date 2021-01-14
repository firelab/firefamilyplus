// RunRestoreDialog.cpp : implementation file
//
//
#include "stdafx.h"
#include "fireplus.h"
#include "RunRestoreDialog.h"
#include "RunsSet.h"
#include "RunSave.h"
#include "RunSaveViewSet.h"
#include "RunDescriptionSet.h"
#include "ugctrl.h"
#include "UTEdit.h"
#include ".\runrestoredialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRunRestoreDataSource::CRunRestoreDataSource(void)
{
	records = NULL;
	
    
	totalRecs = 0;
	virRec = 0;
}

CRunRestoreDataSource::~CRunRestoreDataSource(void)
{
}

long CRunRestoreDataSource::GetNumRows()
{
	return totalRecs;
}

int CRunRestoreDataSource::GetNumCols()
{
	return 7;
}

int CRunRestoreDataSource::SetCell(int col,long row,CUGCell *cell)
{
	
	return UG_SUCCESS;
}

int	CRunRestoreDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 0:
			cell->SetText("RunID");
			return UG_SUCCESS;
		case 1:
			cell->SetText("SIG/Sta");
			return UG_SUCCESS;	
		case 2:
			cell->SetText("Years");
			return UG_SUCCESS;	
		case 3:
			cell->SetText("Ann. Filter");
			return UG_SUCCESS;	
		case 4:
			cell->SetText("Description");
			return UG_SUCCESS;	
		case 5:
			cell->SetText("Created");
			return UG_SUCCESS;
		case 6:
			cell->SetText("LastRun");
			return UG_SUCCESS;	
		
		
		default:
			cell->SetText("");
			return UG_SUCCESS;
		}
	}


    
	if(row == 0){
		records->MoveFirst();
	
	} else if(row == totalRecs - 1){
		records->MoveLast();
	
	}else if(row != virRec){
		records->Move(row - virRec);
	
	}

	

	switch(col)
	{
	case -1:
		// cell->SetNumber(row + 1);
		break;
	case 0:
		if(!records->IsFieldNull(&records->m_runID))
			cell->SetNumber(records->m_runID);
		else
			cell->SetText("");
		break;
	case 1:
	    if(!records->IsFieldNull(&records->m_SIG_Station))
		{
			//str = records->m_ObsDate.Format();//"%m/%d/%y");
			cell->SetText(records->m_SIG_Station);
		}
		else 
			cell->SetText("Station");
		break; 
	case 2:
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
	case 3:
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
    case 4:
		if(!records->IsFieldNull(&records->m_Description))
			cell->SetText(records->m_Description);
		else
			cell->SetText("");
		break;
	case 5:
		if(!records->IsFieldNull(&records->m_CreatedOn))
		{
			//str = records->m_ObsDate.Format();//"%m/%d/%y");
			cell->SetText(records->m_CreatedOn);
		}
		else
			cell->SetText("");
		break;
	case 6:
		if(!records->IsFieldNull(&records->m_LastRun))
			cell->SetText(records->m_LastRun);
		else
			cell->SetText("");
		break;
	
	
	}
	virRec = row;
	return UG_SUCCESS;
}

void CRunRestoreDataSource::SetRecords(CRunSaveViewSet *_records)
{
	records = _records;
    

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




RunRestoreCUG::RunRestoreCUG()
{
}

RunRestoreCUG::~RunRestoreCUG()
{
}

void RunRestoreCUG::OnSetup()
{
	
	GetClientRect(&rect);
    //MoveWindow(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(7);

  
	int wid = rect.Width() / 30;

	SetColWidth(-1, 0);
	SetColWidth(0, 60); // runID
	SetColWidth(1, 60);  // SIG/Station
	SetColWidth(2,65);  // years
	SetColWidth(3,87);  // annual filter
	SetColWidth(4, 370);  // description
	SetColWidth(5, 120); // created on
	SetColWidth(6, 120); // last run
    //FitToWindow(0,6);
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


	GetColDefault( 4, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 4, &cell );


	GetColDefault( 5, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 5, &cell );

	GetColDefault( 6, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 6, &cell );


	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 
    SetUserSizingMode(2);   /* user resizable */

    ((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );

	EnableMenu(TRUE);
	

}

void RunRestoreCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	//DrawExcelFocusRect(dc,rect);
	
	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

COLORREF RunRestoreCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}


void RunRestoreCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void RunRestoreCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void RunRestoreCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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
// CRunRestoreDialog dialog


CRunRestoreDialog::CRunRestoreDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet, CRunSaveViewSet *_records /* = NULL*/)
	: CDialog(CRunRestoreDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRunRestoreDialog)
	textID = 0;
	//}}AFX_DATA_INIT
	records = _records;
	
	fpSet = _fpSet;
	virRec = 0;
	totalRecs = 0;
	sortOrder = 0;

	m_grid.m_data.SetRecords(records);
}


void CRunRestoreDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CRunRestoreDialog)
	DDX_Text(pDX, IDC_EDIT_RUN_ID, textID);
    DDX_Text(pDX, IDC_EDIT_RUN_STATION,filterSta);
	DDX_Text(pDX, IDC_EDIT_RUN_DESCRIPTION, filterDescription);
	
	//}}AFX_DATA_MAP
	
	
	
}


BEGIN_MESSAGE_MAP(CRunRestoreDialog, CDialog)
	//{{AFX_MSG_MAP(CRunRestoreDialog)
	ON_BN_CLICKED(IDFILTER, OnFilter)
	ON_BN_CLICKED(IDC_BATCH_SORT,OnReverseSort)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	

	ON_WM_SIZE()
	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRunRestoreDialog message handlers





BOOL CRunRestoreDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	EnableToolTips(TRUE);


	rbCol = 1;
	CWaitCursor wait;
	records->MoveFirst();
	
    reverseButton = (CButton*)GetDlgItem(IDC_BATCH_SORT);
	loadButton = (CButton*)GetDlgItem(IDOK);
	cancelButton = (CButton*)GetDlgItem(IDCANCEL);

	m_grid.AttachGrid(this, IDC_GRID);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}






void RunRestoreCUG::OnMenuCommand(int col,long row,int section,int item)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 0)
	{ 
		/* if (item == ID_SORT_SORTASCENDING)
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
		} */

	}
}

int  RunRestoreCUG::OnMenuStart(int col,long row,int section)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 0)
	{ 

		//****** Empty the Menu!!
		EmptyMenu(); 

		//******* Add the Menu Items
		/* AddMenuItem(ID_SORT_SORTASCENDING,"Sort &Ascending");
		AddMenuItem(ID_SORT_SORTDESCENDING, "Sort &Descending"); */
	} 
	return TRUE;
}


void RunRestoreCUG::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed){
	    
        CUGCell cell;
		GetCell(col, row, &cell);
		switch (col){
			 case 1:   // SIG/Station
              //    SetRowHeight(row,50);
			    break;


		}
		//cell.GetNumber(&restoreID);

	//StartMenu(col, row, point, 0);
}

void CRunRestoreDialog::OnOK() 
{
	
	int restoreID=0;
	UpdateData();
	if (textID > 0){	// get the run ID from the edit box at the top of the dialog
		restoreID = textID;
	} else {
		CUGCell cell;
		m_grid.GetCell(0, m_grid.GetCurrentRow(), &cell);
		cell.GetNumber(&restoreID);
	}
	


	EnableWindow(FALSE);
	
	
	// find record
	CString seek;
	CString str;
	CRunsSet runSet(fpSet->m_pDatabase);
	runSet.Open();

	// basic validation

	if (! runSet.IsEOF()){
			runSet.MoveLast();
			curr_runID = runSet.m_runID + 1;
	} else
		curr_runID = 1;

	if (restoreID < 1 || restoreID > curr_runID){
		CString strMsg;
		strMsg.Format("ERROR: A valid run ID was not selected.\n");
		AfxMessageBox(strMsg);
	}


	runSet.m_strFilter.Format("[runID] = %d", restoreID);
	runSet.Requery();
	if(runSet.IsEOF())
	{
		CString msg;
		msg.Format("ERROR: The selected run ID (%d) was not found.", restoreID);
		AfxMessageBox(msg);
		runSet.Close();
		return;
	}

	runSet.Close();

	CRunLoadDialog rld(this, fpSet, restoreID);

	if (rld.DoModal()== IDOK) 
		CDialog::OnOK();
}



void CRunRestoreDialog::OnReverseSort()
{
   sortOrder = !sortOrder;
   OnFilter();

}


// filter button pressed
void CRunRestoreDialog::OnFilter(){
   UpdateData();

   CString stringFilter;
   if ((filterDescription.GetLength() > 0) && (filterSta.GetLength() > 0)){
	   // note: filter doesn't work on this query, since it joins 2 tables (@%#&* microsoft)
       //stringFilter.Format("[ffpRuns.Description] LIKE '%%%s%%'  AND [ffpRunDescription.SIG/Station] LIKE '%%%s%%'",filterDescription,filterSta);    
	   //records->m_strFilter=_T(stringFilter);
       //records->Requery();
	   records->Close();
       CString strSQL;
	   if (sortOrder)  // reverse runID sort
          strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID and ffpRuns.Description LIKE '%%%s%%' and ffpRunDescription.SIG_Station LIKE '%%%s%%') ORDER BY ffpRuns.runID DESC", filterDescription,filterSta);
	   else
	      strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID and ffpRuns.Description LIKE '%%%s%%' and ffpRunDescription.SIG_Station LIKE '%%%s%%') ORDER BY ffpRuns.runID", filterDescription,filterSta);
       records->Open(CRecordset::dynaset, strSQL);
	   if (!records->IsEOF()){
          m_grid.m_data.SetRecords(records);
	  
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
	   if (sortOrder) // reverse runID sort
           strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID and ffpRuns.Description LIKE '%%%s%%' ) ORDER BY ffpRuns.runID DESC", filterDescription);
	   else
	      strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID and ffpRuns.Description LIKE '%%%s%%' ) ORDER BY ffpRuns.runID", filterDescription);
       records->Open(CRecordset::dynaset, strSQL);
       if (!records->IsEOF()){
          m_grid.m_data.SetRecords(records);
	  
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
	   if (sortOrder) // reverse runID sort
           strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID and ffpRunDescription.SIG_Station LIKE '%%%s%%') ORDER BY ffpRuns.runID DESC", filterSta);
	   else
          strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID and ffpRunDescription.SIG_Station LIKE '%%%s%%') ORDER BY ffpRuns.runID", filterSta);
       records->Open(CRecordset::dynaset, strSQL);
       if (!records->IsEOF()){

          m_grid.m_data.SetRecords(records);
	  
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
	   if (sortOrder) // reverse runID sort
strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID ) ORDER BY ffpRuns.runID DESC");
	   else
          strSQL.Format("SELECT *  FROM ffpRuns, ffpRunDescription WHERE (ffpRuns.runID = ffpRunDescription.runID ) ORDER BY ffpRuns.runID");
       records->Open(CRecordset::dynaset, strSQL);

       m_grid.m_data.SetRecords(records);
	  
	   int index = m_grid.GetDefDataSource();
	   m_grid.SetGridUsingDataSource(index); 	
		
			
	   m_grid.RedrawAll();

	

}



/////////////////////////////////////////////////////////////////////////////
// CRunLoadDialog dialog


CRunLoadDialog::CRunLoadDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet , int _restoreID /* = NULL*/)
	: CDialog(CRunLoadDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRunRestoreDialog)
	//}}AFX_DATA_INIT
	restoreID = _restoreID;
	fpSet = _fpSet;

}


void CRunLoadDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RUNID, restoreID);
}


BEGIN_MESSAGE_MAP(CRunLoadDialog, CDialog)
	//{{AFX_MSG_MAP(CRunLoadDialog)
	
	
	//}}AFX_MSG_MAP
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRunRestoreDialog message handlers





BOOL CRunLoadDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CRunLoadDialog::OnOK() 
{
	///EnableWindow(FALSE);

	CString strMsg;

	// basic validation

	// find record
	CString seek;
	CString str;
	CRunsSet runSet(fpSet->m_pDatabase);
	runSet.Open();

	// basic validation

	if (! runSet.IsEOF()){
		runSet.MoveLast();
		curr_runID = runSet.m_runID + 1;
	} else
		curr_runID = 1;

	if (restoreID < 1 || restoreID > curr_runID){
		CString strMsg;
		strMsg.Format("ERROR: A valid run ID was not selected.\n");
		AfxMessageBox(strMsg);
		return;
	}


	runSet.m_strFilter.Format("[runID] = %d", restoreID);

	if(runSet.IsEOF())
	{
		CString msg;
		msg.Format("ERROR: The selected run ID (%d) was not found.", restoreID);
		AfxMessageBox(msg);
		runSet.Close();
		return;
	}
	// load saved run settings
	// added for batch
	// single run mode
	CRunSave saveRun(fpSet);
	saveRun.RestoreGeneral(restoreID, fpSet);
	saveRun.RestoreClimateOptions(restoreID, fpSet);
	saveRun.RestoreWxStation(restoreID, fpSet);
	saveRun.RestoreFireOptions(restoreID, fpSet);
	//curr_runID = restoreID;
	strMsg.Format("Settings have been restored from saved run #%d.\n", restoreID);
	AfxMessageBox(strMsg);
	CDialog::OnOK();
	
}




void CRunRestoreDialog::OnSize(UINT nType, int cx, int cy)
{
	
	
	CWaitCursor wait;
	
	if (cx < 450)
		cx = 450;
	if (cy < 300)
		cy = 300;


    CDialog::OnSize(nType, cx, cy);

	

	int a,b;

	if (m_grid && m_grid.m_hWnd){
	    a = 100;
	    
		b = cy - (a + 50);

		m_grid.MoveWindow(10, a, cx -15, b);
	    
      
        if (reverseButton->m_hWnd)
   	       reverseButton->MoveWindow(10,cy - 30,80,25);
		if (loadButton->m_hWnd)
   	       loadButton->MoveWindow(cx - 300,cy - 30,80,25);
		if (cancelButton->m_hWnd)
   	       cancelButton->MoveWindow(cx - 120,cy - 30,80,25);

	}
}

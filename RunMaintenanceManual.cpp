// RunMaintenanceManual.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "RunMaintenanceManual.h"
#include ".\runmaintenancemanual.h"
#include "BatchRunsSet.h"
#include "BatchSet.h"
#include "RunsSet.h"
#include "RunDescriptionSet.h"
#include "RunFireAssocSet.h"
#include "RunClimateOptionsSet.h"
#include "RunWxStationSet.h"
#include "RunFireOptionsSet.h"
#include "RunOptionsSet.h"
#include "RunPocketCardSet.h"
#include "FFPOverlaysSet.h"

DelRec::DelRec()
{
	runID = -1;
	del = false;
	numBatches = 0;
	batchesStr = "";
}


CRunMaintenanceDataSource::CRunMaintenanceDataSource(void)
{
	records = NULL;
	//records2 = NULL;
	totalRecs = 0;
	virRec = 0;
	delRecs = NULL;	
}

CRunMaintenanceDataSource::~CRunMaintenanceDataSource(void)
{
	if(delRecs)
		delete[] delRecs;
}

long CRunMaintenanceDataSource::GetNumRows()
{
	return totalRecs;
}

int CRunMaintenanceDataSource::GetNumCols()
{
	return 9;
}

int CRunMaintenanceDataSource::SetCell(int col,long row,CUGCell *cell)
{
	
	return UG_SUCCESS;
}



int	CRunMaintenanceDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
	switch(col)
		{
		case 0:
			cell->SetText("Delete");
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
		case 8:
			cell->SetText("In Batches");
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
		if(delRecs)
		{
			cell->SetNumber((delRecs[row].del == true) ? 1 : 0);
		}
		else
			cell->SetNumber(0);
		/*if(!records->IsFieldNull(&records->m_BatchInc))
			cell->SetNumber(records->m_BatchInc);
		else
			cell->SetNumber(0);*/
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
	case 8:
		if(delRecs)
		{
			CString cellText;
			cellText.Format("%d ", delRecs[row].numBatches);
			if(delRecs[row].numBatches > 0)
				cellText += delRecs[row].batchesStr;
			cell->SetText(cellText);
		}
		else
			cell->SetText("0");
		/*if(!records->IsFieldNull(&records->m_BatchInc))
			cell->SetNumber(records->m_BatchInc);
		else
			cell->SetNumber(0);*/
		break;
	
	
	
	}
	virRec = row;
	return UG_SUCCESS;
}

void CRunMaintenanceDataSource::SetRecords(CRunSaveViewSet *_records)
{
	records = _records;
	//records2 = _records2;
	long oldRecs = totalRecs;
	totalRecs = 0;
	virRec = 0;
	records->MoveFirst();
	while(!records->IsEOF())
	{
		totalRecs++;
		records->MoveNext();
	}
	if(totalRecs > 0)
	{
		CBatchRunsSet brSet(records->m_pDatabase);
		brSet.Open();
		DelRec *tDelRecs = new DelRec[totalRecs];
		records->MoveFirst();
		long loc = 0;
		while(!records->IsEOF())
		{

			tDelRecs[loc].runID = records->m_runID;
			for(long i = 0; i < oldRecs; i++)
			{
				if(delRecs[i].runID == tDelRecs[loc].runID)
				{
					tDelRecs[loc].del = delRecs[i].del;
					//tDelRecs[loc].batchesStr = delRecs[i].batchesStr;
					//tDelRecs[loc].numBatches = delRecs[i].numBatches;
					break;
				}
			}
			records->MoveNext();
			loc++;
		}
		if(delRecs)
			delete[] delRecs;
		delRecs = tDelRecs;

		//now populate batch nums
		records->MoveFirst();
		loc = 0;
		while(!records->IsEOF())
		{
			brSet.m_strFilter.Format("[runID] = %d", records->m_runID);
			brSet.Requery();
			delRecs[loc].batchesStr = "";
			delRecs[loc].numBatches = 0;
			
			while(!brSet.IsEOF())
			{
				delRecs[loc].numBatches++;
				if(delRecs[loc].numBatches == 1)
				{
					delRecs[loc].batchesStr.Format("(%d", brSet.m_batchID);
				}
				else
				{
					CString tmp;
					tmp.Format(",%d", brSet.m_batchID);
					delRecs[loc].batchesStr += tmp;
				}
				brSet.MoveNext();
			}
			if(delRecs[loc].numBatches > 0)
			{
				delRecs[loc].batchesStr += ")";
			}
			records->MoveNext();
			loc++;
		}
		brSet.Close();
	}
	if(totalRecs > 0)
	{
		records->MoveFirst();
	}

}

void CRunMaintenanceDataSource::Sort(int sortID)
{
	switch(sortID)
	{
	case 0://runID
		records->m_strSort = "ffpRuns.[runID]";
		break;
	case 1://SIG/Sta
		records->m_strSort = "ffpRunDescription.[SIG_Station]";
		break;
	case 2://Created On
		records->m_strSort = "ffpRuns.[CreatedOn]";
		break;
	case 3://Last Run
		records->m_strSort = "ffpRuns.[LastRun]";
		break;
	case 4://Years
		records->m_strSort = "ffpRunDescription.[StartYear]";
		break;
	case 5://Annual Filter
		records->m_strSort = "ffpRunDescription.[StartMonth]";
		break;
	case 6://Descritpion
		records->m_strSort = "ffpRuns.[Description]";
		break;
	}
	//need to update delRecs to match new record order....
	records->Requery();
	if(totalRecs > 0)
	{
		DelRec *tDelRecs = new DelRec[totalRecs];
		//records->MoveFirst();
		long loc = 0;
		while(!records->IsEOF())
		{
			tDelRecs[loc].runID = records->m_runID;
			for(long i = 0; i < totalRecs; i++)
			{
				if(delRecs[i].runID == tDelRecs[loc].runID)
				{
					tDelRecs[loc].del = delRecs[i].del;
					tDelRecs[loc].numBatches = delRecs[i].numBatches;
					tDelRecs[loc].batchesStr = delRecs[i].batchesStr;
					break;
				}
			}
			records->MoveNext();
			loc++;
		}
		if(delRecs)
			delete[] delRecs;
		delRecs = tDelRecs;
	}


	records->MoveFirst();
	virRec = 0;
}


RunMaintenanceCUG::RunMaintenanceCUG()
{
}

RunMaintenanceCUG::~RunMaintenanceCUG()
{
}

void RunMaintenanceCUG::OnSetup()
{
	m_iCheckBoxIndex = AddCellType( &m_checkBoxCT );
	
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(9);

    SetMultiSelectMode(false);

	int wid = rect.Width() / 33;
	SetColWidth(-1, 0);
	SetColWidth(0, 50); // include
	SetColWidth(1, 60); // runID
	SetColWidth(2, 60); // SIG/Sta
	SetColWidth(3, 65); // years
	SetColWidth(4, 87);  // annual filter
	SetColWidth(5,188);  // description
	SetColWidth(6,120);  // created on
	SetColWidth(7,120);  // last run
	SetColWidth(8,90);  // Num batches
	
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
//	cell.SetParam(USE_COXNUMBER);
	SetColDefault( 4, &cell );

	GetColDefault( 8, &cell );
	cell.SetReadOnly(TRUE);
	cell.SetNumberDecimals(0);
	SetColDefault( 8, &cell );


	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 
	SetUserSizingMode(2);   /* user resizable */

    ((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );

	EnableUpdate(TRUE);

}


COLORREF RunMaintenanceCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}


int RunMaintenanceCUG::OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param)
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
		if(checked < 1)
		{
			cell.SetNumber(1);
			//m_data.records->Edit();
			//m_data.records->m_BatchInc = 1;//deletes[row] = true;
			//m_data.records->Update();
			m_data.delRecs[row].del = true;
		}
		else
		{
			cell.SetNumber(0);
			m_data.delRecs[row].del = false;
			//m_data.records->Edit();
			//m_data.records->m_BatchInc = 0;
			//m_data.records->Update();
		}
		RedrawAll();
		//m_data.records2->m_strFilter.Format("[runID] = %d", m_data.records->m_runID);
		
		//m_data.records2->Requery();

		//if (m_data.records2->IsEOF()){
         //   CString msg;
		//    msg.Format("ERROR: The selected run ID (%d) was not found.", m_data.records->m_runID);
		//    AfxMessageBox(msg);
		
		//    return TRUE;

		//}

		//m_data.records2->Edit();


		//if (checked < 1){
		//   cell.SetNumber(1);
		//	m_data.records2->m_BatchInc = 1;
		//} else {
		//	cell.SetNumber(0);
		//	m_data.records2->m_BatchInc = 0;
		//}
		
	//	m_data.records2->Update();

	
      /*  m_data.records->Requery();
		m_data.records->MoveFirst();

		SetCell(col,row,&cell);

		m_data.SetRecords(m_data.records);//,m_data.records2);
		int index = GetDefDataSource();
		SetGridUsingDataSource(index); 	

		RedrawAll();*/
		
		
	}
	return TRUE;
}




void RunMaintenanceCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void RunMaintenanceCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void RunMaintenanceCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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

void RunMaintenanceCUG::SelectAll(bool trueFalse)
{
	/*m_data.records->MoveFirst();
	while(!m_data.records->IsEOF())
	{
		m_data.records->Edit();
		m_data.records->m_BatchInc = (trueFalse == true) ? 1 : 0;
		m_data.records->Update();
		m_data.records->MoveNext();
	}
	m_data.records->MoveFirst();
	m_data.virRec = 0;*/
	for(long i = 0; i < m_data.totalRecs; i++)
		m_data.delRecs[i].del = trueFalse;
}


// CRunMaintenanceManual dialog

IMPLEMENT_DYNAMIC(CRunMaintenanceManual, CDialog)
CRunMaintenanceManual::CRunMaintenanceManual(CWnd* pParent, CDatabase *_pDB)
	: CDialog(CRunMaintenanceManual::IDD, pParent)
{
	m_pDB = _pDB;
	m_pRunsSet = new CRunSaveViewSet(m_pDB);
    CString strSQL = "SELECT *  FROM ffpRuns, ffpRunDescription WHERE ffpRuns.runID = ffpRunDescription.runID";
	m_pRunsSet->m_strSort.Format("ffpRuns.[runID]");
	m_pRunsSet->Open(CRecordset::dynaset, strSQL);
	/*while(!m_pRunsSet->IsEOF())
	{
		m_pRunsSet->Edit();
		m_pRunsSet->m_BatchInc = 0;
		m_pRunsSet->Update();
		m_pRunsSet->MoveNext();
	}*/
	m_grid.m_data.SetRecords(m_pRunsSet);
}

CRunMaintenanceManual::~CRunMaintenanceManual()
{
	if(m_pRunsSet)
	{
		m_pRunsSet->Close();
		delete m_pRunsSet;
	}
}

void CRunMaintenanceManual::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SORT_COMBO, m_sortCombo);
}


BEGIN_MESSAGE_MAP(CRunMaintenanceManual, CDialog)
	ON_BN_CLICKED(IDC_BATCH_SELECTALL, OnBnClickedBatchSelectall)
	ON_BN_CLICKED(IDC_BATCH_DESELECTALL, OnBnClickedBatchDeselectall)
	ON_CBN_SELCHANGE(IDC_SORT_COMBO, OnCbnSelchangeSortCombo)
END_MESSAGE_MAP()


// CRunMaintenanceManual message handlers

BOOL CRunMaintenanceManual::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_sortCombo.SetCurSel(0);
	m_grid.AttachGrid(this, IDC_GRID);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRunMaintenanceManual::OnOK()
{
	//Delete selected Runs
	//always prompt if in one or more batches, as we'll delete those batches as well (for now....)
	CList<int, int> delRuns;
	CList<int, int> delBatches;
	for(long i = 0; i < m_grid.m_data.totalRecs; i++)
	{
		if(m_grid.m_data.delRecs[i].del == true)
			delRuns.AddTail(m_grid.m_data.delRecs[i].runID);
	}
	if(delRuns.GetCount() <= 0)
	{
		AfxMessageBox("No runs selected to delete.");
		return;
	}

	//ok, walk thru the list deleting stuff (What fun!)
	CBatchRunsSet brSet(m_pDB);
	brSet.Open();
	int delRun;
	POSITION pos = delRuns.GetHeadPosition();
	POSITION lastPos;// = pos;
	int response = IDYES;
	while(pos)
	{
		lastPos = pos;
		delRun = delRuns.GetNext(pos);
		//check for inclusion in batches, if so PROMPT TO BE SURE
		brSet.m_strFilter.Format("[runID] = %d", delRun);
		brSet.Requery();
		if(!brSet.IsBOF() && !brSet.IsEOF())
		{
			CString bStr, tmp;
			int bCount = 0;
			while(!brSet.IsEOF())
			{
				if(bStr.GetLength() <= 0)
					bStr.Format("%d", brSet.m_batchID);
				else
				{
					tmp.Format(", %d", brSet.m_batchID);
					bStr += tmp;
				}
				bCount++;
				brSet.MoveNext();
			}
			CString msg;
			msg.Format("Run %d is included in the following batches:\n%s\n\nContinuing with run deletion with also delete these batches.\n"
				"Continue with deleting run %d?",
				delRun, bStr, delRun);
			response = AfxMessageBox(msg, MB_YESNOCANCEL);
			if(IDYES == response)
				delBatches.AddTail(brSet.m_batchID);
			else if(IDNO == response)
				delRuns.RemoveAt(lastPos);
			else
				break;
		}
	}

	//now go thru deleting runs and batches
	if(response != IDCANCEL)
	{
		pos = delRuns.GetHeadPosition();
		while(pos)
		{
			delRun = delRuns.GetNext(pos);
			CRunsSet runSet(m_pDB);
			runSet.m_strFilter.Format("[runID] = %d", delRun);
			runSet.Open();
			while(!runSet.IsEOF())
			{
				runSet.Delete();
				runSet.MoveNext();
			}
			runSet.Close();

			CRunOptionsSet roSet(m_pDB);
			roSet.m_strFilter.Format("[runID] = %d", delRun);
			roSet.Open();
			while(!roSet.IsEOF())
			{
				roSet.Delete();
				roSet.MoveNext();
			}
			roSet.Close();

			CRunWxStationSet rwxSet(m_pDB);
			rwxSet.m_strFilter.Format("[runID] = %d", delRun);
			rwxSet.Open();
			while(!rwxSet.IsEOF())
			{
				rwxSet.Delete();
				rwxSet.MoveNext();
			}
			rwxSet.Close();

			CRunFireOptionsSet rfSet(m_pDB);
			rfSet.m_strFilter.Format("[runID] = %d", delRun);
			rfSet.Open();
			while(!rfSet.IsEOF())
			{
				rfSet.Delete();
				rfSet.MoveNext();
			}
			rfSet.Close();

			CRunClimateOptionsSet rcSet(m_pDB);
			rcSet.m_strFilter.Format("[runID] = %d", delRun);
			rcSet.Open();
			while(!rcSet.IsEOF())
			{
				rcSet.Delete();
				rcSet.MoveNext();
			}
			rcSet.Close();

			CRunFireAssocSet rfaSet(m_pDB);
			rfaSet.m_strFilter.Format("[runID] = %d", delRun);
			rfaSet.Open();
			while(!rfaSet.IsEOF())
			{
				rfaSet.Delete();
				rfaSet.MoveNext();
			}
			rfaSet.Close();

			CRunDescriptionSet rdSet(m_pDB);
			rdSet.m_strFilter.Format("[runID] = %d", delRun);
			rdSet.Open();
			while(!rdSet.IsEOF())
			{
				rdSet.Delete();
				rdSet.MoveNext();
			}
			rdSet.Close();

			CRunPocketCardSet rpcSet(m_pDB);
			rpcSet.m_strFilter.Format("[runID] = %d", delRun);
			rpcSet.Open();
			while(!rpcSet.IsEOF())
			{
				rpcSet.Delete();
				rpcSet.MoveNext();
			}
			rpcSet.Close();

			CFFPOverlaysSet oSet(m_pDB);
			oSet.m_strFilter.Format("[runID] = %d", delRun);
			oSet.Open();
			if(!oSet.IsEOF())
				oSet.MoveFirst();
			while(!oSet.IsEOF())
			{
				if(oSet.m_runID > 0)//0 is for General FFP Options...don't kill them!
				{
					oSet.Delete();
				}
				oSet.MoveNext();
			}
			oSet.Close();
		}
		//delete any selected batches
		pos = delBatches.GetHeadPosition();
		while(pos)
		{
			int delBatch = delBatches.GetNext(pos);
			brSet.m_strFilter.Format("[batchID] = %d", delBatch);
			brSet.Requery();
			while(!brSet.IsEOF())
			{
				brSet.Delete();
				brSet.MoveNext();
			}
			CBatchSet batchSet(m_pDB);
			batchSet.m_strFilter.Format("[batchID] = %d", delBatch);
			batchSet.Open();
			while(!batchSet.IsEOF())
			{
				batchSet.Delete();
				batchSet.MoveNext();
			}
			batchSet.Close();
		}

	}
	brSet.Close();
	if(response != IDCANCEL)
		CDialog::OnOK(); //commeted out, stay in dialog...
	//but need to reset records!
	/*m_pRunsSet->Requery();
	m_grid.m_data.SetRecords(m_pRunsSet);
	m_grid.m_data.virRec = 0;
	m_grid.GotoRow(0);
	m_grid.RedrawAll();*/
}

void CRunMaintenanceManual::OnBnClickedBatchSelectall()
{
	
	m_grid.SelectAll(true);
	m_grid.RedrawAll();
}

void CRunMaintenanceManual::OnBnClickedBatchDeselectall()
{
	m_grid.SelectAll(false);
	m_grid.RedrawAll();
}

void CRunMaintenanceManual::OnCbnSelchangeSortCombo()
{
	m_grid.m_data.Sort(m_sortCombo.GetCurSel());
	m_grid.GotoRow(0);
	m_grid.RedrawAll();

}

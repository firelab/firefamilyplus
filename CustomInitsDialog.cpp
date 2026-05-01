// CustomInitsDialog.cpp : implementation file
//
#include "stdafx.h"
#include "fireplus.h"
#include "CustomInitSet.h"
#include "CustomInitsDialog.h"
#include "CustomInitEditDialog.h"
#include "CustomInitCreateDialog.h"


#include "ugctrl.h"
#include "UTEdit.h"
//#include "updialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CCustomInitsDataSource::CCustomInitsDataSource(void)
{
	records = NULL;
	//totalRecs = 0;
	virRec = 0;
	
}

CCustomInitsDataSource::~CCustomInitsDataSource(void)
{
}

long CCustomInitsDataSource::GetNumRows()
{
	return totalRecs;
}

int CCustomInitsDataSource::GetNumCols()
{
	return 9;
}

int CCustomInitsDataSource::SetCell(int col,long row,CUGCell *cell)
{
	
    if(col < 0 || row < 0)
		return UG_NA;
	//write the cells new contents to the database

	if(row >= totalRecs)
		return UG_NA;
	if(col == 4)//dummy
		return UG_NA;
	if(virRec != row)
	{
		records->Move(row - virRec);
		virRec = row;
	}

	records->Edit();
	CString str = cell->GetText();
	
	int var1;
	double var2;

	switch(col)
	{
	case 0: // year	
		break;
	case 1: // greenup
		int tsec,tmin,thour,tday,tmon,tyear;
		cell->GetTime(&tsec,&tmin,&thour,&tday,&tmon,&tyear);
		records->m_GreenJulian.SetDate(tyear,tmon,tday);
		break;
	case 2: // KBDI - int
	    var1 = cell->GetNumber();
		records->m_StartKBDI = var1;

		break;
	case 3:
		var2 = cell->GetNumber();
		records->m_Start1000 = var2;
		break;
	case 5: // start up
		{
			int tsec,tmin,thour,tday,tmon,tyear;
			cell->GetTime(&tsec,&tmin,&thour,&tday,&tmon,&tyear);
			records->m_StartUpDay.SetDate(tyear,tmon,tday);
		}
		break;
	case 6:
		var2 = cell->GetNumber();
		records->m_FFMC = var2;
		break;
	case 7:
		var2 = cell->GetNumber();
		records->m_DMC = var2;
		break;
	case 8:
		var2 = cell->GetNumber();
		records->m_DC = var2;
		break;

	}
	records->Update();

	return UG_SUCCESS;}



int	CCustomInitsDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 0:
			cell->SetText("Year");
			return UG_SUCCESS;
		case 1:
			cell->SetText("Greenup");
			return UG_SUCCESS;
		case 2:
			cell->SetText("StartKBDI");
			return UG_SUCCESS;
		case 3:
			cell->SetText("Start1000");
			return UG_SUCCESS;	
		case 4:
			//cell->SetBackColor(RGB(0, 0, 0));
			//cell->SetBorderColor(RGB(0, 0, 0));
			return UG_SUCCESS;
		case 5:
			cell->SetText("CFDRS Start Day");
			return UG_SUCCESS;
		case 6:
			cell->SetText("FFMC");
			return UG_SUCCESS;
		case 7:
			cell->SetText("DMC");
			return UG_SUCCESS;
		case 8:
			cell->SetText("DC");
			return UG_SUCCESS;

		
		
	
		}
	}
	if(row == 0)
		records->MoveFirst();
	else if(row == totalRecs - 1)
		records->MoveLast();
	else if(row != virRec)
		records->Move(row - virRec);

	CString tempstr;

	switch(col)
	{
	case -1:
		// cell->SetNumber(row + 1);
		break;
	case 0:
		
		if(!records->IsFieldNull(&records->m_Year))
			tempstr.Format("%d",records->m_Year);
		else
			tempstr.Format("0");
		cell->SetText(tempstr);

		break;
	case 1:
		if(!records->IsFieldNull(&records->m_GreenJulian)){
		
			tempstr.Format("%d/%d",records->m_GreenJulian.GetMonth(),records->m_GreenJulian.GetDay());
			cell->SetText(tempstr);
		} else
			cell->SetText("");
		break;
	case 2:
		if(!records->IsFieldNull(&records->m_StartKBDI))
			cell->SetNumber(records->m_StartKBDI);
		else
			cell->SetNumber(0);
		break;
		
	case 3:
		if(!records->IsFieldNull(&records->m_Start1000))
			cell->SetNumber(records->m_Start1000);
		else
			cell->SetNumber(0);
		break;
	case 5:
		if(!records->IsFieldNull(&records->m_StartUpDay)){
		
			tempstr.Format("%d/%d",records->m_StartUpDay.GetMonth(),records->m_StartUpDay.GetDay());
			cell->SetText(tempstr);
		} else
			cell->SetText("");
		break;
    case 6:
		if(!records->IsFieldNull(&records->m_FFMC))
			cell->SetNumber(records->m_FFMC);
		else
			cell->SetNumber(0);
		break;
	case 7:
		if(!records->IsFieldNull(&records->m_DMC))
			cell->SetNumber(records->m_DMC);
		else
			cell->SetNumber(0);
		break;
    case 8:
		if(!records->IsFieldNull(&records->m_DC))
			cell->SetNumber(records->m_DC);
		else
			cell->SetNumber(0);
		break;

	
	}
	virRec = row;
	return UG_SUCCESS;
}

void CCustomInitsDataSource::SetRecords(CCustomInitSet *_records)
{
	records = _records;
	totalRecs = 0;
	virRec = 0;
	
	// xxxx

	if (records->IsEOF())
		return;

	records->MoveFirst();
	while(!records->IsEOF())
	{
		totalRecs++;
		records->MoveNext();
	}
	if(totalRecs > 0)
		records->MoveFirst();
	
}



CustomInitsCUG::CustomInitsCUG()
{
}

CustomInitsCUG::~CustomInitsCUG()
{
}

void CustomInitsCUG::OnSetup()
{

	CRect rect;
	m_iDTPickerIndex = AddCellType( &m_dtPicker );
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(9);

    SetMultiSelectMode(false);

	int wid = rect.Width() / 10;

	SetColWidth(-1,0);
	SetColWidth(0, wid );
	SetColWidth(1, wid );
	SetColWidth(2, wid );
	SetColWidth(3, wid );
	SetColWidth(4, 2 );
	SetColWidth(5, wid *2);
	SetColWidth(6, wid );
	SetColWidth(7, wid );
	SetColWidth(8, wid);


	CUGCell cell;
	GetColDefault( 0, &cell );
	cell.SetReadOnly(FALSE);
	SetColDefault( 0, &cell );


	GetColDefault( 1, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetDataType(UGCELLDATA_TIME);
	cell.SetCellType( m_iDTPickerIndex );
	cell.SetCellTypeEx( UGCT_DROPLISTHIDEBUTTON );
	
	SetColDefault( 1, &cell );

	GetColDefault( 2, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetNumberDecimals(0);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault( 2, &cell );

	GetColDefault( 3, &cell );
	cell.SetNumberDecimals(0);
	cell.SetReadOnly(FALSE);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault( 3, &cell );

	GetColDefault( 4, &cell );
	cell.SetReadOnly(TRUE);
	cell.SetBackColor(RGB(0, 0, 0));
	cell.SetBorderColor(RGB(0, 0, 0));
	//cell.Set
	SetColDefault( 4, &cell );

	GetColDefault( 5, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetDataType(UGCELLDATA_TIME);
	cell.SetCellType( m_iDTPickerIndex );
	cell.SetCellTypeEx( UGCT_DROPLISTHIDEBUTTON );
	SetColDefault( 5, &cell );

	GetColDefault( 6, &cell );
	cell.SetNumberDecimals(2);
	cell.SetReadOnly(FALSE);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault( 6, &cell );

	GetColDefault( 7, &cell );
	cell.SetNumberDecimals(2);
	cell.SetReadOnly(FALSE);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault( 7, &cell );

	GetColDefault( 8, &cell );
	cell.SetNumberDecimals(2);
	cell.SetReadOnly(FALSE);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault( 8, &cell );

	

	((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );

	//LockColumns(1);

	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 
	
	EnableUpdate(TRUE);

}

/////////////////////////////////////////////////////////////////////////////
//	OnEditVerify
//		This notification is sent every time the user hits a key while in edit mode.
//		It is mostly used to create custom behavior of the edit contol, because it is
//		so eazy to allow or disallow keys hit.
//	Params:
//		col, row	- location of the edit cell
//		edit		-	pointer to the edit control
//		vcKey		- virtual key code of the pressed key
//	Return:
//		TRUE - to accept pressed key
//		FALSE - to do not accept the key
int CustomInitsCUG::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(*vcKey);

	//if(*vcKey == VK_SUBTRACT)
	//	return FALSE;

	return TRUE;
}


COLORREF CustomInitsCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

/////////////////////////////////////////////////////////////////////////////
//	OnEditFinish
//		This notification is sent when the edit is being finised
//	Params:
//		col, row	- coordinates of the edit cell
//		edit		- pointer to the edit control
//		string		- actual string that user typed in
//		cancelFlag	- indicates if the edit is being cancelled
//	Return:
//		TRUE - to allow the edit it proceede
//		FALSE - to force the user back to editing of that same cell
int CustomInitsCUG::OnEditFinish(int col, long row,CWnd *edit,LPCTSTR string,BOOL cancelFlag)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*edit);
	UNREFERENCED_PARAMETER(string);
	UNREFERENCED_PARAMETER(cancelFlag);
	CString str;
	//str.Format("OnEditFinish Row: %d Col %d Data: %s", row, col, string);
	//AfxMessageBox(str);
	switch(col)
	{
	//case 1://date control
		//this->RedrawAll();
		//break;
	case 2://KBDI
		{
			int val = atoi(string);
			if(val < 0 || val > 800)
			{
				AfxMessageBox("Error: KBDI must be between 0 and 800");
				edit->SetWindowText("100");
				return FALSE;
			}
		}
		break;
	case 3://FM 1000
		{
			int val = atoi(string);
			if(val < 0 || val > 100)
			{
				AfxMessageBox("Error: Start FM 1000 must be between 0 and 100");
				edit->SetWindowText("15");
				return FALSE;
			}
		}
		break;
	case 6://FFMC
		{
			int val = atoi(string);
			if(val < 0 || val > 101)
			{
				AfxMessageBox("Error: FFMC must be between 0 and 101");
				edit->SetWindowText("85");
				return FALSE;
			}
		}
		break;
	case 7://DMC
		{
			int val = atoi(string);
			if(val < 0 || val > 2000)
			{
				AfxMessageBox("Error: FFMC must be between 0 and 101");
				edit->SetWindowText("6");
				return FALSE;
			}
		}
		break;
	case 8://DC
		{
			int val = atoi(string);
			if(val < 0 || val > 2000)
			{
				AfxMessageBox("Error: FFMC must be between 0 and 2000");
				edit->SetWindowText("15");
				return FALSE;
			}
		}
		break;
	}
	return TRUE;
}


int CustomInitsCUG::OnEditStart(int col, long row,CWnd **edit)
{
	CUGCell cell;
	GetCellIndirect( col, row, &cell );
	DWORD dwStyle = 0;
	
	if(col == 0 )  // year or greenup
		return FALSE;

	if ( cell.GetParam() == USE_COXNUMBER )
	{
		if ( ! m_cutNumeric.GetSafeHwnd())
		{
			m_cutNumeric.Create( WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, COXMASK_ID );
			m_cutNumeric.m_ctrl = this;
		}
		m_cutNumeric.SetDecimalSeparator( '.' );
		m_cutNumeric.SetGroupSeparator( ',' );
		//m_cutNumeric.SetSetDecimalDigitCount(2);
		m_cutNumeric.SetShowLeadingZero(FALSE);
		switch(col)
		{
		case 2:
		case 3:
			m_cutNumeric.SetFractionalDigitCount(0);
			m_cutNumeric.SetDecimalDigitCount(3);
			//m_cutNumeric.SetR
			break;
		case 6:
			m_cutNumeric.SetFractionalDigitCount(2);
			m_cutNumeric.SetDecimalDigitCount(3);
			break;
		case 7:
		case 8:
			m_cutNumeric.SetFractionalDigitCount(2);
			m_cutNumeric.SetDecimalDigitCount(4);
			break;
		}
		

		*edit = &m_cutNumeric;
	}
	return TRUE;
}


void CustomInitsCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void CustomInitsCUG::OnCharDown(UINT *vcKey,BOOL processed)
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
			if(col == 4)
			{
				col = 3;
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
			if(col == 4)
			{
				col = 5;
			}
		}
		GotoCell( col, row );
	}
	else
		StartEdit( *vcKey ); 
}

void CustomInitsCUG::OnKeyDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);

/*	if ( *vcKey == VK_TAB )
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
			if(col == 4)
			{
				col = 3;
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
			if(col == 4)
			{
				col = 5;
			}
		}
		GotoCell( col, row );
	}*/
}

int CustomInitsCUG::OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param)
{
	if(ID == m_iDTPickerIndex)
	{
		if(msg == UGCT_CONVERTDATE)
		{
			UGCTCONVERTDATE* cd = (UGCTCONVERTDATE*)param;
			cd->string = cd->date.Format("%m/%d");
			//this->RedrawAll();
		}
	}
	
	return TRUE;
}

void CustomInitsCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

/////////////////////////////////////////////////////////////////////////////
// CCustomInitsDialog dialog


CCustomInitsDialog::CCustomInitsDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet, CCustomInitSet *_records /* = NULL*/)
	: CDialog(CCustomInitsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomInitsDialog)
	
	//}}AFX_DATA_INIT
	records = _records;
	fpSet = _fpSet;
	virRec = 0;
	totalRecs = 0;

	m_grid.m_data.SetRecords(records);

	stationTitle.Format("Custom Inits for Station: %s", fpSet->m_SIG_Station);


}





void CCustomInitsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	//{{AFX_DATA_MAP(CCustomInitsDialog)
	DDX_Control(pDX, IDC_CUSTOMINITCHECK, checkEnabled);
	DDX_Text(pDX,    IDC_INIT_STATION, stationTitle);

	//}}AFX_DATA_MAP
	
	
	
}


BEGIN_MESSAGE_MAP(CCustomInitsDialog, CDialog)
	//{{AFX_MSG_MAP(CCustomInitsDialog)
	ON_BN_CLICKED(IDC_CUSTOMINITEDIT, OnCustomInitEdit)
	ON_BN_CLICKED(IDC_CUSTOMINITDELETE,OnCustomInitDelete)
	ON_BN_CLICKED(IDC_CUSTOMINITNEW, OnCustomInitNew)
	ON_BN_CLICKED(IDC_CUSTOMINITCHECK, OnEnableButton)
	//}}AFX_MSG_MAP
	


	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomInitsDialog message handlers




BOOL CCustomInitsDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	EnableToolTips(TRUE);

    
	rbCol = 1;
	CWaitCursor wait;

	records->m_strFilter.Format("[SIG_Station]='%s'",fpSet->m_SIG_Station);
    records->Requery();

	if (! records->IsEOF()){
	   records->MoveFirst();
	   if (records->m_Enabled )
		   checkEnabled.SetCheck(true);
	   else
		   checkEnabled.SetCheck(false);
	}
	m_grid.AttachGrid(this, IDC_GRID);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CustomInitsCUG::OnMenuCommand(int col,long row,int section,int item)
{
	
}

int  CustomInitsCUG::OnMenuStart(int col,long row,int section)
{
	
	return TRUE;
}





void CCustomInitsDialog::OnCustomInitNew(){
	


	
	CCustomInitCreateDialog ccd(this, fpSet, records);
	
		
		if (ccd.DoModal()){
			 records->m_strFilter.Format("[SIG_Station]='%s'",fpSet->m_SIG_Station);
			 records->m_strSort.Format("[Year]");
		     records->Requery();

			 m_grid.m_data.SetRecords(records);
			 int index = m_grid.GetDefDataSource();
			 m_grid.SetGridUsingDataSource(index); 	
			
			 m_grid.RedrawAll();	
		
	

		}
		

}



	


void CCustomInitsDialog::OnCustomInitEdit(){

	int editYear = -1;

	CUGCell cell;
	m_grid.GetCell(0, m_grid.GetCurrentRow(), &cell);
	cell.GetNumber(&editYear);

	if (m_grid.GetCurrentRow() < 0 || editYear < 0){
		AfxMessageBox("Please select a Custom Init to edit.");
		return;

	}


	CString seek;
	seek.Format("[SIG_Station]='%s' AND [Year] = %d", fpSet->m_SIG_Station, editYear);
	
	
	records->m_strFilter = seek;
	records->m_strSort.Format("[Year]");

	if (! records->IsOpen())
    	records->Open();
	records->Requery();
	
	
	if(records->IsEOF()){
		AfxMessageBox("Please select a CustomInit to edit or view.");
	
		return;
	} else {


	
		CCustomInitEditDialog eed(this, fpSet, records,editYear);
	
		
		if (eed.DoModal()){
			 records->m_strFilter.Format("[SIG_Station]='%s'",fpSet->m_SIG_Station);
			 records->m_strSort.Format("[Year]");
		     records->Requery();
			
			 m_grid.m_data.SetRecords(records);
			 int index = m_grid.GetDefDataSource();
			 m_grid.SetGridUsingDataSource(index); 	
			
			 m_grid.RedrawAll();	
	

		}
	}


}


void CCustomInitsDialog::OnOK() {
	records->m_strFilter.Format("[SIG_Station]='%s'",fpSet->m_SIG_Station);
    records->Requery();

	if (! records->IsEOF()){
	   if (checkEnabled.GetCheck()==true){
	      records->MoveFirst();
	      while (! records->IsEOF()){
             records->Edit(); 
		     records->m_Enabled = true;
		     records->Update();
		     records->MoveNext();

	      }
	      records->MoveFirst();
	   } else {
          records->MoveFirst();
	      while (! records->IsEOF()){
              records->Edit(); 
		      records->m_Enabled = false;
		      records->Update();
		      records->MoveNext();

	      }
	      records->MoveFirst();

	   }
	}

	CDialog::OnOK();

}

void CCustomInitsDialog::OnEnableButton() {
	records->m_strFilter.Format("[SIG_Station]='%s'",fpSet->m_SIG_Station);
    records->Requery();

    if (records->IsEOF())
		return;

	if (checkEnabled.GetCheck()== TRUE){
	   records->MoveFirst();
	   while (! records->IsEOF()){
           records->Edit(); 
		   records->m_Enabled = true;
		   records->Update();
		   records->MoveNext();

	   }
	   records->MoveFirst();


	} else {
	   records->MoveFirst();
	   while (! records->IsEOF()){
           records->Edit(); 
		   records->m_Enabled = false;
		   records->Update();
		   records->MoveNext();

	   }
	   records->MoveFirst();

	  
	}

}


void CCustomInitsDialog::OnCustomInitDelete() 
{
	int editYear = -1;

	CUGCell cell;
	m_grid.GetCell(0, m_grid.GetCurrentRow(), &cell);
	cell.GetNumber(&editYear);
	
	long delRow = m_grid.GetCurrentRow();

	if (m_grid.GetCurrentRow() < 0 || editYear < 0){
		AfxMessageBox("Please select a Custom Init to delete.");
		return;

	}


	CString seek;
	seek.Format("([SIG_Station] = '%s' AND [Year] = %d)", fpSet->m_SIG_Station,editYear);

	
	records->m_strFilter = seek;
	if (! records->IsOpen())
    	records->Open();
	records->Requery();
	

	
	
	
	if(records->IsEOF()){
		AfxMessageBox("Please select a Custom Init to delete.");
	
		return;
	} else {

     

		CCustomInitDeleteConfirmDialog cdd(this, fpSet, editYear,records);
	
		
		if (cdd.DoModal()){
			 records->m_strFilter.Format("[SIG_Station]='%s'",fpSet->m_SIG_Station);
			 records->m_strSort.Format("[Year]");
		     records->Requery();

		   // totalRecs --;
            // m_grid.m_data.totalRecs --;
	       m_grid.m_data.SetRecords(records);
		   int index = m_grid.GetDefDataSource();
		   m_grid.SetGridUsingDataSource(index); 	
			//m_grid.DeleteRow(delRow);
			
			m_grid.RedrawAll();
	

		}
	}
	
}





/////////////////////////////////////////////////////////////////////////////
// CCustomInitDeleteConfirmDialog dialog


CCustomInitDeleteConfirmDialog::CCustomInitDeleteConfirmDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet ,   int _editYear, CCustomInitSet *_records)
	: CDialog(CCustomInitDeleteConfirmDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCustomInitEditDialog)
	//}}AFX_DATA_INIT

	records = _records;

	
	fpSet = _fpSet;
	editYear = _editYear;


}


void CCustomInitDeleteConfirmDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	CString editYearString;
	editYearString.Format("%d",editYear);

	DDX_Text(pDX, IDC_CUSTOMINIT_YEAR, editYearString);
}


BEGIN_MESSAGE_MAP(CCustomInitDeleteConfirmDialog, CDialog)
	//{{AFX_MSG_MAP(CCustomInitDeleteConfirmDialog)
	
	
	//}}AFX_MSG_MAP
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomInitDeleteDialog message handlers





BOOL CCustomInitDeleteConfirmDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CCustomInitDeleteConfirmDialog::OnOK() 
{
	///EnableWindow(FALSE);

	CString strMsg;

	// basic validation

	// find record
	CString seek;
	CString str;
	

	// basic validation

	
	//CCustomInitSet CustomInitSet(fpSet->m_pDatabase);
	if (! records->IsOpen())
	   records->Open();

	records->m_strFilter.Format("[SIG_Station]='%s' AND [Year] = %d", fpSet->m_SIG_Station,editYear);
	records->m_strSort.Format("[Year]");

	records->Requery();
	if (records->IsEOF())
	{
		CString msg;
		msg.Format("ERROR: Custom Init Year not found.");
		AfxMessageBox(msg);
			
		return;
	}

	

	
	records->Delete();
	//m_grid
	

	strMsg.Format("Custom Init deleted.");
	AfxMessageBox(strMsg);
	CDialog::OnOK();
	
}





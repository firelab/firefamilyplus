// DOICauseDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DOICauseSet.h"
#include "DOICauseDialog.h"
//#include "DelRecsDialog.h"



int GetNextDOICause(CDatabase *pDB);
extern char *stripCRLF(char *src);





CDOICauseDataSource::CDOICauseDataSource(void)
{
	records = NULL;
	totalRecs = 0;
	virRec = 0;
}

CDOICauseDataSource::~CDOICauseDataSource(void)
{
}

long CDOICauseDataSource::GetNumRows()
{
	return totalRecs;
}

int CDOICauseDataSource::GetNumCols()
{
	return 5;
}


int CDOICauseDataSource::SetCell(int col,long row,CUGCell *cell)
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
	str = str.Trim();
	switch(col)
	{
	case 0:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_GeneralCause);
		else
			records->m_GeneralCause = cell->GetNumber();
		break;
	case 1:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_SpecificCause);
		else
			records->m_SpecificCause = cell->GetNumber();
		break;
	case 2:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_GeneralDescription);
		else
			records->m_GeneralDescription = cell->GetText();;
		break;
	case 3:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_SpecificDescription);
		else
			records->m_SpecificDescription = cell->GetText();;
		break;
	case 4:

		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_Abbr);
		else
			records->m_Abbr = cell->GetText();;
		break;
	
	
	}

	records->Update();

	return UG_SUCCESS;
}

int	CDOICauseDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 0:
			cell->SetText("General");
			return UG_SUCCESS;
		case 1:
			cell->SetText("Specific");
			return UG_SUCCESS;
		case 2:
			cell->SetText("General Description");
			return UG_SUCCESS;
		case 3:
			cell->SetText("Specific Description");
			return UG_SUCCESS;	
		case 4:
			cell->SetText("Abbr");
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
	case 0:
		cell->SetNumber(records->m_GeneralCause);
		break;
	case 1:
		cell->SetNumber(records->m_SpecificCause);
		break;
	case 2:
		cell->SetText(records->m_GeneralDescription.Trim());
		break;
	case 3:
		cell->SetText(records->m_SpecificDescription.Trim());
		break;
	case 4:
		cell->SetText(records->m_Abbr.Trim());
		break;
	

	
	}
	virRec = row;
	return UG_SUCCESS;
}

void CDOICauseDataSource::SetRecords(CDOICauseSet *_records)
{
	records = _records;
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


DOICauseCUG::DOICauseCUG()
{
}

DOICauseCUG::~DOICauseCUG()
{
}

void DOICauseCUG::OnSetup()
{
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(4);
	int wid = rect.Width() / 10;
	//int wid = 22;
	SetColWidth(-1, 0);
	SetColWidth(0, wid * 1);
	SetColWidth(1, wid * 1);
	SetColWidth(2, wid * 3.25);
	SetColWidth(3, wid * 3.25);
	SetColWidth(4, wid * 1.5);
	

	CUGCell cell;
	for(int c = 0; c < GetNumberCols(); c++)
	{
		GetColDefault( c, &cell );
		if (c == 0)    // cannot modify numeric code
		   cell.SetReadOnly(TRUE);
		else
		   cell.SetReadOnly(FALSE);
		SetColDefault( c, &cell );
	}
	

	int index = AddDataSource(&m_data); 
	SetDefDataSource(index);
	SetGridUsingDataSource(index); 

	EnableMenu(TRUE);

}

COLORREF DOICauseCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

void DOICauseCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void DOICauseCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void DOICauseCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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

int DOICauseCUG::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	if (col > 0)
	{
		
		int limit = 0;
		switch (col)
		{
		case 4:
			limit = 4; break;
		case 2:
			limit = 15; break;
		case 3:
			limit = 35; break;
		default:
			limit = 35; break;

		}

          if ( edit->GetWindowTextLength() >= limit && *vcKey != 8 )
               return FALSE;
		
	}
	return CUGCtrl::OnEditVerify(col,row,edit,vcKey);
}


int DOICauseCUG::OnEditStart(int col, long row,CWnd **edit)
{
	// restrict editing of original codes
	CUGCell cell;
	GetCellIndirect( 0, row, &cell );
	int cause = cell.GetNumber();
	if (cause < 10)
		return FALSE;


	
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
		m_cutNumeric.SetDecimalDigitCount(0);
		m_cutNumeric.SetFractionalDigitCount(0);
		switch(col)
		{
			case 0:
			m_cutNumeric.SetDecimalDigitCount(2);
			break;
		
		}
		*edit = &m_cutNumeric;
	}
	return TRUE;
}

// CDOICauseDialog dialog

IMPLEMENT_DYNAMIC(CDOICauseDialog, CDialog)

CDOICauseDialog::CDOICauseDialog(CWnd* pParent /*=NULL*/, CDOICauseSet *pRecords /*=NULL*/,  CFireplusSet *_fpSet)
	: CDialog(CDOICauseDialog::IDD, pParent)
{
	
	m_fpSet = _fpSet;
	
	pDB = pRecords->m_pDatabase;
	records = pRecords;
	virRec = 0;
	totalRecs = 0;
	m_grid.m_data.SetRecords(records);
}

CDOICauseDialog::~CDOICauseDialog()
{
}


void CDOICauseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTON_EDIT, m_EditButton);
	DDX_Control(pDX, IDC_BUTTON_DELETE, m_DeleteButton);
}


BEGIN_MESSAGE_MAP(CDOICauseDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_NEW, &CDOICauseDialog::OnBnClickedButtonNew)
	//ON_BN_CLICKED(IDC_BUTTON_EDIT, &CDOICauseDialog::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDOICauseDialog::OnBnClickedButtonDelete)	
	//ON_BN_CLICKED(IDC_BUTTON_COPY, &CDOICauseDialog::OnBnClickedButtonCopy)
	
END_MESSAGE_MAP()


// CDOICauseDialog message handlers

void CDOICauseDialog::OnBnClickedButtonNew()
{
	
	
	records->AddNew();


	
	records->m_GeneralDescription = "";

	records->m_SpecificDescription = "";

    records->m_GeneralCause = 10;
	records->m_SpecificCause = GetNextDOICause(records->m_pDatabase);
	records->m_Abbr = "USER";


	records->Update();
	
	virRec = 0;
	m_grid.m_data.SetRecords(records);
	int index = m_grid.GetDefDataSource();
	m_grid.SetGridUsingDataSource(index); 
	m_grid.RedrawAll();
	m_grid.GotoCell( 0, m_grid.GetNumberRows() - 1 );
	
	

	
}


void CDOICauseDialog::OnBnClickedButtonDelete()
{
	if(m_grid.GetNumberRows() <= 0)
		return;
	long row = m_grid.GetCurrentRow();
	
	// restrict editing of original codes
	CUGCell cell;
	m_grid.GetCellIndirect( 0, row, &cell );
	int cause = cell.GetNumber();
	if (cause < 10)
		return;

	records->SetAbsolutePosition(row + 1);

	//CDelRecsDialog drd(this, records, m_grid.m_data.GetNumRows(), m_grid.GetCurrentRow() + 1);
	CString msg;
	msg.Format("Delete DOI Cause Code \"%s\"?", records->m_Abbr);
	if(IDYES == AfxMessageBox(msg, MB_YESNO))
	{
		
		
		records->Delete();
		//reset table contents
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
		//m_grid.Invalidate();
		//this->Invalidate();
		m_grid.RedrawAll();
	}
}

BOOL CDOICauseDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	SetWindowText(_T("Cause Codes - DOI"));
	
	CWaitCursor wait;
	if(!records->IsBOF() || !records->IsEOF())
		records->MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int GetNextDOICause(CDatabase *pDB)
{
	CDOICauseSet tSet(pDB);
	tSet.m_strSort = "[SpecificCause]";
	tSet.m_strFilter = "([GeneralCause] = 10)";
	int nextID = 1;
	tSet.Open();
	if(!tSet.IsBOF() && !tSet.IsEOF())
	{//has records
		if(tSet.m_SpecificCause > nextID)
		{
			tSet.Close();
			return nextID;
		}
		nextID++;
		tSet.MoveNext();
		while(!tSet.IsEOF())
		{
			if(tSet.m_SpecificCause > nextID)
			{
				tSet.Close();
				return nextID;
			}
			nextID++;
			tSet.MoveNext();
		}
	}
	tSet.Close();
	return nextID;
}




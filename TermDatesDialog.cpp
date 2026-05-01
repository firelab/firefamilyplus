// TermDatesDialog.cpp : implementation file
//
 
#include "stdafx.h"
#include "fireplus.h"
#include "TermDatesDialog.h"
#include "TermData.h"
#include <afxrich.h>
#include "richtermdoc.h"
#include "richview.h"
#include "BestFit.h"
#include "TermProbabilitiesDialog.h"

extern CFireplusApp theApp;
extern int intCompare( const void *arg1, const void *arg2 );

TermDatesData::TermDatesData()
{
	year = 0;
	comment = "";
}
TermDatesData::~TermDatesData()
{
}

typedef CTypedPtrList<CPtrList, TermDatesData *> TermDatesDataList;
/*CTermDatesDataSource::CTermDatesDataSource(void)
{
	records = NULL;
	totalRecs = 0;
	virRec = 0;
}

CTermDatesDataSource::~CTermDatesDataSource(void)
{
}

long CTermDatesDataSource::GetNumRows()
{
	return totalRecs;
}

int CTermDatesDataSource::GetNumCols()
{
	return 4;
}

int	CTermDatesDataSource::GetCell(int col,long row,CUGCell *cell)
{
	CString str;
	if(row < 0)
	{
		switch(col)
		{
		case 1:
			cell->SetText("Year");
			return UG_SUCCESS;
		case 2:
			cell->SetText("Term Date");
			return UG_SUCCESS;
		case 3:
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
	case 0:
		cell->SetNumber(row + 1);
		break;
	case 1:
		cell->SetNumber(records->m_Date.GetYear());
			;
		break;
	case 2:
		str = records->m_Date.Format("%m/%d");
		cell->SetText(str);
		break;
	case 3:
		cell->SetText(records->m_Comment.Trim());
		break;
	}
	virRec = row;
	return UG_SUCCESS;
}

void CTermDatesDataSource::SetRecords(CTermDatesSet *_records)
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
*/
//TermDatesCUG
TermDatesCUG::TermDatesCUG()
{
}

TermDatesCUG::~TermDatesCUG()
{
}

void TermDatesCUG::OnSetup()
{
	CUGCell cell;
	m_iDTPickerIndex = AddCellType( &m_dtPicker );
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(3);
	int wid = rect.Width() / 20;
	SetCurrentCellMode(2);
	SetNumberCols(3);
	SetColWidth(-1, 0);
	SetColWidth(0, wid * 2);
	SetColWidth(1, wid * 2);
	SetColWidth(2, wid * 16);
	QuickSetText(0, -1, "Year"); 
	QuickSetText(1, -1, "Day");
	QuickSetText(2, -1, "Comment");

	GetColDefault( 0, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault(0, &cell);

	GetColDefault( 1, &cell );
	cell.SetDataType(UGCELLDATA_TIME);
	cell.SetCellType( m_iDTPickerIndex );
	cell.SetCellTypeEx( UGCT_DROPLISTHIDEBUTTON );
	//cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 1, &cell );

	GetColDefault( 2, &cell );
	cell.SetReadOnly(FALSE);
	SetColDefault(2, &cell);

}

int TermDatesCUG::OnEditStart(int col, long row,CWnd **edit)
{
	/*CUGCell cell;
	GetCellIndirect( col, row, &cell );
	DWORD dwStyle = 0;
	if(col == 2 || col == 4 || col == 5 || col == 14 || col == 15)//droplists
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
		//m_cutNumeric.SetDecimalDigitCount(2);
		m_cutNumeric.SetShowLeadingZero(FALSE);
		switch(col)
		{
		case 8:
		case 17:
			m_cutNumeric.SetFractionalDigitCount(0);
			m_cutNumeric.SetDecimalDigitCount(3);
			break;
		case 9:
			m_cutNumeric.SetFractionalDigitCount(2);
			m_cutNumeric.SetDecimalDigitCount(2);
			break;
		case 10:
			m_cutNumeric.SetFractionalDigitCount(2);
			m_cutNumeric.SetDecimalDigitCount(3);
			break;
		case 16:
			m_cutNumeric.SetFractionalDigitCount(0);
			m_cutNumeric.SetDecimalDigitCount(5);
			break;
		}
		*edit = &m_cutNumeric;
	}*/
	return TRUE;
}

void TermDatesCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void TermDatesCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void TermDatesCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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

int TermDatesCUG::OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param)
{
	if(ID == m_iDTPickerIndex)
	{
		if(msg == UGCT_CONVERTDATE)
		{
			UGCTCONVERTDATE* cd = (UGCTCONVERTDATE*)param;
			cd->string = cd->date.Format("%m/%d");
		}
		if(msg == UGCT_DISPLAYMONTHCAL)
		{
			long row = GetCurrentRow();
			CString str;
			QuickGetText(0, row, &str);
			int year = atoi(str);
			UGCTMONTHCALSETTINGS *mcs = (UGCTMONTHCALSETTINGS*)param;
			if(mcs->dateCur.GetStatus() == COleDateTime::valid)
				mcs->dateCur.SetDate(year, mcs->dateCur.GetMonth(), mcs->dateCur.GetDay());
			else //day is invalid... pick a day!
			{
				mcs->dateCur.SetDate(year, 9, 30);
			}
		}
	}
	return TRUE;
}

COLORREF TermDatesCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

void TermDatesCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}


// CTermDatesDialog dialog

IMPLEMENT_DYNAMIC(CTermDatesDialog, CDialog)

CTermDatesDialog::CTermDatesDialog(CWnd* pParent /*=NULL*/, int termID /*=NULL*/, CDatabase *_pDB /*= NULL*/, CFireplusSet *_fpSet)
	: CDialog(CTermDatesDialog::IDD, pParent)
	, m_Name(_T(""))
	, m_Comment(_T(""))
	, m_StartYear(0)
	, m_EndYear(0)
{
	m_termID = termID;
	m_termsSet = new CTermsSet(_pDB);
	m_termsSet->m_strFilter.Format("[TermID] = %d", m_termID);
	m_termsSet->Open();
	
	m_fpSet = _fpSet;
	//m_fpSet->m_pTermDatesDialog = this;

	m_termDatesSet = new CTermDatesSet(_pDB);
	m_termDatesSet->m_strFilter.Format("[TermID] = %d", m_termID);
	m_termDatesSet->m_strSort = "[Year]";
	m_termDatesSet->Open();
	m_nID = CTermDatesDialog::IDD;
	m_Parent = pParent;
}

CTermDatesDialog::~CTermDatesDialog()
{
	m_termDatesSet->Close();
	delete 	m_termDatesSet;
	m_termsSet->Close();
	delete m_termsSet;
}

void CTermDatesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_Name);
	DDV_MaxChars(pDX, m_Name, 40);
	DDX_Control(pDX, IDC_DATETIMEPICKER_STARTDAY, m_ctlStartDay);
	//DDX_Text(pDX, IDC_EDIT_COMMENT, m_Comment);
	//DDV_MaxChars(pDX, m_Comment, 200);
	DDX_Text(pDX, IDC_EDIT_STARTYEAR, m_StartYear);
	DDX_Control(pDX, IDC_SPIN_STARTYEAR, m_spinStartYear);
	DDX_Text(pDX, IDC_EDIT_ENDYEAR, m_EndYear);
	DDX_Control(pDX, IDC_SPIN_ENDYEAR, m_spinEndYear);
//	DDX_Control(pDX, IDC_BUTTON_DELETE, m_btnDelete);
//	DDX_Control(pDX, IDC_BUTTON_INSERT, m_btnInsert);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Text(pDX, IDC_RICHEDIT21_COMMENT, m_Comment);
}

void CTermDatesDialog::RedisplayTable()
{
	int count = 0;
	m_grid.SetNumberRows(0);
	CString tmp;
	if(m_termDatesSet->IsOpen())
	{
		try
		{    //reset records for table - something has probably changed
			//m_termDatesSet->m_strFilter.Format("");

			m_termDatesSet->Requery();
			int rows = 0;
			while( !m_termDatesSet->IsEOF( ) )    
			{  
				rows++;
				m_termDatesSet->MoveNext();
			}
			if(rows > 0)
			{
				m_termDatesSet->MoveFirst();
				m_grid.SetNumberRows(rows);
				int row = 0;
				// Move through records
				while( !m_termDatesSet->IsEOF( ) )    
				{   
					//add a record to the table
					tmp.Format("%d", m_termDatesSet->m_Year);
					m_grid.QuickSetText(0, row, tmp);
					//m_grid.QuickSetNumber(0, row, m_termDatesSet->m_Year);
					if(!m_termDatesSet->IsFieldNull(&m_termDatesSet->m_Date))
						m_grid.QuickSetText(1, row, m_termDatesSet->m_Date.Format("%m/%d"));
					else
						m_grid.QuickSetText(1, row, "");
					m_grid.QuickSetText(2, row, m_termDatesSet->m_Comment.TrimRight());
					count++;
					row++;
					//go to next record
					m_termDatesSet->MoveNext( );
				}
			}
		}
		catch( CDBException* e )
		{
			// Do nothing--used for security violations     // when opening tables
			e->Delete( );
		}		
	}
	m_grid.RedrawAll();
}

BOOL CTermDatesDialog::OnInitDialog()
{
	////if(m_fpSet)
	//{
		CString str;
		if(m_termsSet->m_SIG_Station.GetLength() > 6) //SIG
			str.Format("Edit Term Data for %s", m_termsSet->m_SIG_Station);
		else
			str.Format("Edit Term Data for Station %s", m_termsSet->m_SIG_Station);
		SetWindowText(str);
	//}
	CUGCell cell;
	CDialog::OnInitDialog();
	m_Name = m_termsSet->m_Name.TrimRight();
	m_Comment = m_termsSet->m_Comment.TrimRight();
	m_ctlStartDay.SetFormat("MM/dd");
	m_ctlStartDay.SetTime(m_termsSet->m_StartDate);
	m_spinStartYear.SetRange(1900, 3000);
	m_spinEndYear.SetRange(1900, 3000);
	// 09/2012 modified FFP4.1 issue #21
	//m_StartYear = 1985;
	if (m_fpSet != NULL)
	    m_StartYear = m_fpSet->m_StartYear;
	else
		m_StartYear = 1985;
	//m_EndYear = 2010;
	if (m_fpSet != NULL)
	   m_EndYear = m_fpSet->m_EndYear;
	else
	   m_EndYear = 2010;
	m_spinStartYear.SetPos(m_StartYear);
	m_spinEndYear.SetPos(m_EndYear);
	CRect rect;
	m_grid.AttachGrid(this, IDC_GRID);
	m_grid.SetUserSizingMode(2);


	RedisplayTable();
	UpdateData(FALSE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CTermDatesDialog, CDialog)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER_STARTDAY, &CTermDatesDialog::OnDtnDatetimechangeDatetimepickerStartday)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CTermDatesDialog::OnBnClickedButtonApply)
	//ON_BN_CLICKED(IDC_BUTTON_APPEND, &CTermDatesDialog::OnBnClickedButtonAppend)
	//ON_BN_CLICKED(IDC_BUTTON_INSERT, &CTermDatesDialog::OnBnClickedButtonInsert)
	//ON_BN_CLICKED(IDC_BUTTON_DELETE, &CTermDatesDialog::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CTermDatesDialog::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_PROBABILITIES, &CTermDatesDialog::OnBnClickedButtonProbabilities)
END_MESSAGE_MAP()



// CTermDatesDialog message handlers

void CTermDatesDialog::OnDtnDatetimechangeDatetimepickerStartday(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CTermDatesDialog::OnOK()
{
	//UpdateData();
	//prepare data for nonlinearfit
	OnBnClickedButtonSave();
	m_termDatesSet->Requery();
	int rows = 0;
	int *termDays = NULL;
	while( !m_termDatesSet->IsEOF( ) )    
	{  
		if(!m_termDatesSet->IsFieldNull(&m_termDatesSet->m_Date))
			rows++;
		m_termDatesSet->MoveNext();
	}
	if(rows <= 0)
	{
		AfxMessageBox("Error! At least one date needed to run Term module.");
		return;
	}
	COleDateTime start;
	m_ctlStartDay.GetTime(start);

	termDays = new int[rows];
	m_termDatesSet->MoveFirst();
	int row = 0;
	// Move through records
	while( !m_termDatesSet->IsEOF( ) )    
	{   
		if(!m_termDatesSet->IsFieldNull(&m_termDatesSet->m_Date))//has a date, use it
		{
			termDays[row] = m_termDatesSet->m_Date.GetDayOfYear() - start.GetDayOfYear() + 1;
			//safey!!!!!
			if(termDays[row] <= 0)
				termDays[row] += 365;
			//go to next record
			row++;
		}
		m_termDatesSet->MoveNext( );
	}
	//sort the days...
	qsort(termDays, rows, sizeof(int), intCompare);
	BestFit wiebull;
	//add the sorted days
	for(int r = 0; r < rows; r++)
	{
		wiebull.AddPoint(termDays[r], ((double)(r + 1)) / rows);
		//TRACE2("%d, %f\n", termDays[r], ((double)(r + 1)) / rows);
	}
	delete[] termDays;
	wiebull.ComputeBestFit();
	//wiebull.ShowArray();
	m_termsSet->Edit();
	m_termsSet->m_Name = m_Name;
	m_termsSet->m_Comment = m_Comment;
	m_ctlStartDay.GetTime(m_termsSet->m_StartDate);
	m_termsSet->m_Coeff1 = wiebull.getBestFitAlpha();
	m_termsSet->m_Coeff2 = wiebull.getBestFitBeta();
	m_termsSet->m_Coeff3 = wiebull.BestFitRSquared();

	m_termsSet->Update();
	//OnBnClickedButtonSave();

	CTermData *tData = new CTermData();
	if(!tData->Create(m_termsSet, m_termDatesSet))
	{
		AfxMessageBox("Error: No valid dates");
		delete tData;
		return;
	}
	CString tFileName = tData->TermReport();
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();
		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate = 
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("Term"))
			{
				CRichTermDoc *fDoc = (CRichTermDoc *)curTemplate->OpenDocumentFile(tFileName);
				fDoc->SetTermData(tData);
				fDoc->isTemp = true;
				fDoc->m_pDB = m_termsSet->m_pDatabase;//m_fpSet;
				fDoc->m_fpSet = m_fpSet;
				CString tStr;
				tStr.Format("%s - Term Report", tData->staStr);//m_fpSet->m_SIG_Station);
				fDoc->SetTitle(tStr);
				//fDoc->SetViewFires();
			}
		}
		//m_fpSet->m_pTermDatesDialog = NULL;
		DestroyWindow();
	//CDialog::OnOK();
}

void CTermDatesDialog::OnBnClickedButtonApply()
{
	UpdateData();
	int nRows = m_EndYear - m_StartYear + 1;
	if(nRows < 0)
	{
		AfxMessageBox("Error! End Year must be greater than Start Year.");
		return;
	}
	TermDatesDataList dataList;
	CString str;
	for(int r = 0; r < m_grid.GetNumberRows(); r++)
	{
		m_grid.QuickGetText(1, r, &str);
		if(str.GetLength() > 0)//has month/day, add it to the list
		{
			TermDatesData *tData = new TermDatesData();
			COleDateTime dt;
			dt.ParseDateTime(str);
			m_grid.QuickGetText(0, r, &str);
			tData->year = atoi(str);
			tData->date.SetDate(tData->year, dt.GetMonth(), dt.GetDay());
			m_grid.QuickGetText(2, r, &str);
			tData->comment = str;
			dataList.AddTail(tData);
		}
	}
	m_grid.SetNumberRows(nRows);
	CString tmp;
	for(int row = 0; row < nRows; row++)
	{
		//add a record to the table
		tmp.Format("%d", row + m_StartYear);
		m_grid.QuickSetText(0, row, tmp);
		m_grid.QuickSetText(1, row, "");
		m_grid.QuickSetText(2, row, "");
		POSITION pos = dataList.GetHeadPosition();
		{
			while(pos)
			{
				TermDatesData *tData = dataList.GetNext(pos);
				if(tData->year == row + m_StartYear)
				{
					//add this data to the grid
					m_grid.QuickSetText(2, row, tData->comment);
					m_grid.QuickSetText(1, row, tData->date.Format("%m/%d"));
				}
			}
		}
	}
	//delete stored list objects
	POSITION pos = dataList.GetHeadPosition();
	{
		while(pos)
		{
			delete dataList.GetNext(pos);
		}
	}
	//m_btnInsert.EnableWindow();
	//m_btnDelete.EnableWindow();
	m_grid.RedrawAll();


}

/*void CTermDatesDialog::OnBnClickedButtonAppend()
{
	// TODO: Add your control notification handler code here
}

void CTermDatesDialog::OnBnClickedButtonInsert()
{
	// TODO: Add your control notification handler code here
}

void CTermDatesDialog::OnBnClickedButtonDelete()
{
	// TODO: Add your control notification handler code here
}*/

void CTermDatesDialog::OnBnClickedButtonSave()
{
	UpdateData();
	m_termsSet->Edit();
	m_termsSet->m_Name = m_Name;
	m_termsSet->m_Comment = m_Comment;
	m_ctlStartDay.GetTime(m_termsSet->m_StartDate);
	m_termsSet->Update();
	//wipe out old records and insert new ones
	this->m_termDatesSet->Requery();
	while(!m_termDatesSet->IsEOF())
	{
		m_termDatesSet->Delete();
		m_termDatesSet->MoveNext();
	}
	//save grid contents to the database
	CString str;
	for(int r = 0; r < m_grid.GetNumberRows(); r++)
	{
		m_termDatesSet->AddNew();
		m_termDatesSet->m_TermID = m_termsSet->m_TermID;
		m_grid.QuickGetText(0, r, &str);
		int yr = atoi(str);
		m_termDatesSet->m_Year = yr;
		m_grid.QuickGetText(1, r, &str);
		if(str.GetLength() > 0)//has month/day
		{
			COleDateTime dt;
			dt.ParseDateTime(str);
			m_termDatesSet->m_Date.SetDate(yr, dt.GetMonth(), dt.GetDay());
		}
		else
			m_termDatesSet->SetFieldNull(&m_termDatesSet->m_Date);
		m_grid.QuickGetText(2, r, &str);
		if(str.GetLength() > 150)
			str.Truncate(150);
		m_termDatesSet->m_Comment = str;
		m_termDatesSet->Update();
	}
}

void CTermDatesDialog::OnBnClickedButtonProbabilities()
{
	CTermProbabilitiesDialog tpd(this, m_termID, m_termsSet->m_pDatabase);
	tpd.DoModal();
}

void CTermDatesDialog::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class

	//m_fpSet->m_pTermDatesDialog = NULL;
	delete this;
}

void CTermDatesDialog::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//m_fpSet->m_pTermDatesDialog = NULL;
	DestroyWindow();
}

bool CTermDatesDialog::Create()
{
	int ret = CDialog::Create(m_nID, m_Parent);
	if(ret == 0)
		return false;
	return true;
}


// TermProbabilitiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "TermProbabilitiesDialog.h"

//TermProbabilitiesCUG
TermProbabilitiesCUG::TermProbabilitiesCUG()
{
}

TermProbabilitiesCUG::~TermProbabilitiesCUG()
{
}

void TermProbabilitiesCUG::OnSetup()
{
	CUGCell cell;
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(1);
	int wid = rect.Width() - 1;
	SetCurrentCellMode(2);
	SetColWidth(-1, 0);
	SetColWidth(0, wid);
	QuickSetText(0, -1, "Probability"); 

	GetColDefault( 0, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetNumberDecimals(2);
	//cell.SetMask("90");
	SetColDefault(0, &cell);

	/*GetColDefault( 1, &cell );
	cell.SetDataType(UGCELLDATA_TIME);
	cell.SetCellType( m_iDTPickerIndex );
	cell.SetCellTypeEx( UGCT_DROPLISTHIDEBUTTON );
	//cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 1, &cell );

	GetColDefault( 2, &cell );
	cell.SetReadOnly(FALSE);
	SetColDefault(2, &cell);*/

}

int TermProbabilitiesCUG::OnEditStart(int col, long row,CWnd **edit)
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

void TermProbabilitiesCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void TermProbabilitiesCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void TermProbabilitiesCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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

int TermProbabilitiesCUG::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	/*if(ID == m_iDTPickerIndex)
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
	}*/
	return TRUE;
}

COLORREF TermProbabilitiesCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

void TermProbabilitiesCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}




// CTermProbabilitiesDialog dialog

IMPLEMENT_DYNAMIC(CTermProbabilitiesDialog, CDialog)

CTermProbabilitiesDialog::CTermProbabilitiesDialog(CWnd* pParent /*=NULL*/, int _termID/* = -1*/, CDatabase *pDB/* = NULL*/)
	: CDialog(CTermProbabilitiesDialog::IDD, pParent), m_pcntSet(pDB)
{
	m_TermID = _termID;
	m_pcntSet.m_strFilter.Format("[TermID] = %d", m_TermID);
	m_pcntSet.m_strSort = "[Percentile]";
	m_pcntSet.Open();
}

CTermProbabilitiesDialog::~CTermProbabilitiesDialog()
{
	m_pcntSet.Close();
}

void CTermProbabilitiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTermProbabilitiesDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_INSERT, &CTermProbabilitiesDialog::OnBnClickedButtonInsert)
	ON_BN_CLICKED(IDC_BUTTON_APPEND, &CTermProbabilitiesDialog::OnBnClickedButtonAppend)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CTermProbabilitiesDialog::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_ASCENDING, &CTermProbabilitiesDialog::OnBnClickedButtonAscending)
	ON_BN_CLICKED(IDC_BUTTON_DESCENDING, &CTermProbabilitiesDialog::OnBnClickedButtonDescending)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_DEFAULTS, &CTermProbabilitiesDialog::OnBnClickedButtonLoadDefaults)
	ON_BN_CLICKED(IDC_BUTTON_SET_DEFAULTS, &CTermProbabilitiesDialog::OnBnClickedButtonSetDefaults)
END_MESSAGE_MAP()


// CTermProbabilitiesDialog message handlers

BOOL CTermProbabilitiesDialog::OnInitDialog()
{
	CUGCell cell;
	CDialog::OnInitDialog();

	m_grid.AttachGrid(this, IDC_GRID);
	int row = 0;
	while(!m_pcntSet.IsEOF())
	{
		m_grid.AppendRow();
		m_grid.QuickSetNumber(0, row, m_pcntSet.m_Percentile);
		row++;
		m_pcntSet.MoveNext();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTermProbabilitiesDialog::OnOK()
{
	if(!ValidateProbabilities())
		return;
	m_pcntSet.Requery();
	while(!m_pcntSet.IsEOF())
	{
		m_pcntSet.Delete();
		m_pcntSet.MoveNext();
	}
	for(int r = 0; r < m_grid.GetNumberRows(); r++)
	{
		m_pcntSet.AddNew();
		m_pcntSet.m_TermID = this->m_TermID;
		m_pcntSet.m_Percentile = atof(m_grid.QuickGetText(0, r));
		m_pcntSet.Update();
	}

	CDialog::OnOK();
}

void CTermProbabilitiesDialog::OnBnClickedButtonInsert()
{
	int row = m_grid.GetCurrentRow();
	m_grid.InsertRow(row);
	m_grid.RedrawAll();
}

void CTermProbabilitiesDialog::OnBnClickedButtonAppend()
{
	m_grid.AppendRow();
	m_grid.RedrawAll();
}

void CTermProbabilitiesDialog::OnBnClickedButtonDelete()
{
	int row = m_grid.GetCurrentRow();
	m_grid.DeleteRow(row);
	m_grid.RedrawAll();
}

void CTermProbabilitiesDialog::OnBnClickedButtonAscending()
{
	m_grid.SortBy(0, UG_SORT_ASCENDING);
	m_grid.RedrawAll();
}

void CTermProbabilitiesDialog::OnBnClickedButtonDescending()
{
	m_grid.SortBy(0, UG_SORT_DESCENDING);
	m_grid.RedrawAll();
}

void CTermProbabilitiesDialog::OnBnClickedButtonLoadDefaults()
{
	m_grid.SetNumberRows(0);
	CTermPercentilesSet defSet(m_pcntSet.m_pDatabase);
	defSet.m_strFilter.Format("[TermID] = -1");
	defSet.Open();
	int row = 0;
	while(!defSet.IsEOF())
	{
		m_grid.AppendRow();
		m_grid.QuickSetNumber(0, row, defSet.m_Percentile);
		row++;
		defSet.MoveNext();
	}
	defSet.Close();
	m_grid.RedrawAll();
}

void CTermProbabilitiesDialog::OnBnClickedButtonSetDefaults()
{
	if(!ValidateProbabilities())
		return;
	CTermPercentilesSet defSet(m_pcntSet.m_pDatabase);
	defSet.m_strFilter.Format("[TermID] = -1");
	defSet.Open();
	while(!defSet.IsEOF())
	{
		defSet.Delete();
		defSet.MoveNext();
	}
	for(int r = 0; r < m_grid.GetNumberRows(); r++)
	{
		defSet.AddNew();
		defSet.m_TermID = -1;
		defSet.m_Percentile = atof(m_grid.QuickGetText(0, r));
		defSet.Update();
	}
	defSet.Close();
}

bool CTermProbabilitiesDialog::ValidateProbabilities()
{
	CString str, msg;
	for(int r = 0; r < m_grid.GetNumberRows(); r++)
	{
		str = m_grid.QuickGetText(0, r);
		if(str.GetLength() <= 0)
		{
			msg.Format("Error! Row %d cannot be blank!", r + 1);
			AfxMessageBox(msg);
			return false;
		}
		double p = atof(str);
		if(p < 0.0)
		{
			msg.Format("Error! Row %d cannot be less than 0.00!", r + 1);
			AfxMessageBox(msg);
			return false;
		}
		if(p > 1.0)
		{
			msg.Format("Error! Row %d cannot be greater than 1.00!", r + 1);
			AfxMessageBox(msg);
			return false;
		}
	}
	return true;
}

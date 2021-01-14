// WeightsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "StationInSIGSet.h"
#include "WeightsDialog.h"
#include ".\weightsdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//grid class
WeightsCUG::WeightsCUG()
{
}

WeightsCUG::~WeightsCUG()
{
}

int WeightsCUG::OnEditStart(int col, long row,CWnd **edit)
{
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(**edit);
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
		//m_cutNumeric.SetGroupSeparator( ',' );
		m_cutNumeric.SetGroupLength(0);
		m_cutNumeric.SetDecimalDigitCount(-1);
		m_cutNumeric.SetShowLeadingZero(FALSE);
		m_cutNumeric.SetFractionalDigitCount(2);
		*edit = &m_cutNumeric;
	}
	return TRUE;
}

void WeightsCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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
		if(row < 0)
			row = GetNumberRows() - 1;
		if(row >= GetNumberRows())
			row = 0;

		GotoCell( col, row );
	}
}


void WeightsCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}	

void WeightsCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}
	

/////////////////////////////////////////////////////////////////////////////
// CWeightsDialog dialog


CWeightsDialog::CWeightsDialog(CStationInSIGSet *_staSet, CWnd* pParent /*=NULL*/)
	: CDialog(CWeightsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWeightsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	staSet = _staSet;
}


void CWeightsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWeightsDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWeightsDialog, CDialog)
	//{{AFX_MSG_MAP(CWeightsDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWeightsDialog message handlers

BOOL CWeightsDialog::OnInitDialog()
{
	CUGCell cell;
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);
	CRect rect;
	GetDlgItem(IDC_GRID)->GetClientRect(&rect);
	m_grid.SetDefColWidth(rect.Width() / 2);
	m_grid.SetCurrentCellMode(2);
	m_grid.SetNumberCols(1);
	m_grid.SetColWidth(-1, rect.Width() / 2);
	m_grid.QuickSetText(-1, -1, "Station ID"); 
	m_grid.QuickSetText(0, -1, "Weight");
	m_grid.GetColDefault(0, &cell);
	cell.SetNumberDecimals(2);
	cell.SetParam(USE_COXNUMBER);
	m_grid.SetColDefault(0, &cell);
	if(staSet->IsOpen())
	{
		try
		{    
			//set up rowdata structure
			staSet->MoveFirst();
			// Move through records
			int row = 0;
			while( !staSet->IsEOF( ) )    
			{   
				//add a record to the table
				m_grid.AppendRow();
				m_grid.QuickSetText(-1, row, staSet->m_StationID);
				m_grid.QuickSetNumber(0, row, staSet->m_WeightFactor);
				staSet->MoveNext( );   
				row++;
			}
		}
		catch( CDBException* e )
		{
			// Do nothing--used for security violations     // when opening tables
			e->Delete( );
		}		
	}

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWeightsDialog::OnOK() 
{
	//write values straight to recordset
	staSet->MoveFirst();
	double val;
	CUGCell cell;
	for (int i = 0; i < m_grid.GetNumberRows(); i++)
	{
		m_grid.GetCell(0, i, &cell);
		val = cell.GetNumber();//pNumCell->GetNumber();
		if( val <= 0)
		{
			AfxMessageBox("ERROR: All weights must be greater than zero!");
			return;
		}
	}
	int loc = 0;
	// Move through records
	while( !staSet->IsEOF( ) )    
	{   
		staSet->Edit();
		m_grid.GetCell(0, loc, &cell);
		val = cell.GetNumber();//pNumCell->GetNumber();
		staSet->m_WeightFactor = val;
		staSet->Update();
		staSet->MoveNext( ); 
		loc++;
	}
	CDialog::OnOK();
}


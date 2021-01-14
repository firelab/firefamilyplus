// DPDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DPDialog.h"
#include "DPDoc.h"
#include "DPFrame.h"
#include "UserVarSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DecisionCUG::DecisionCUG()
{
}

DecisionCUG::~DecisionCUG()
{
}

void DecisionCUG::OnSetup()
{
	CUGCell cell;
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(2);
	QuickSetText(0, -1, "Class"); 
	QuickSetText(1, -1, "Value");
	int wid = rect.Width() / 7;
	SetColWidth(-1, 0);
	SetColWidth(0, 3 * wid);
	SetColWidth(1, 3 * wid);
	GetColDefault(0, &cell);
	cell.SetReadOnly(TRUE);
	SetColDefault(0,&cell);
	GetColDefault(1, &cell);
	cell.SetNumberDecimals(2);
	SetColDefault(1,&cell);

}

int DecisionCUG::OnEditStart(int col, long row,CWnd **edit)
{
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

void DecisionCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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


void DecisionCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}	

void DecisionCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}
	
COLORREF DecisionCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}


/////////////////////////////////////////////////////////////////////////////
// CDPDialog dialog


CDPDialog::CDPDialog(CWnd* pParent /*=NULL*/, CDocument *_pDoc)
	: CDialog(CDPDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDPDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	//pDoc = pParent->Ge
	m_Parent = (CDPFrame *)pParent;
	pDoc = (CDPDoc *)_pDoc;
	Create(CDPDialog::IDD, pParent);
}


void CDPDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDPDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDPDialog, CDialog)
	//{{AFX_MSG_MAP(CDPDialog)
	ON_BN_CLICKED(IDC_DELETEBUTTON, OnDeletebutton)
	ON_BN_CLICKED(IDC_NEWBUTTON, OnNewbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDPDialog message handlers

BOOL CDPDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);
	
	intFormat = false;
	if(pDoc->fires)
	{
		if(pDoc->fires->varID >= STATICVARIDS)
		{//user var...check number decimals
			CUserVarSet uvs(pDoc->fires->pDB);
			uvs.m_strFilter.Format("[VarID] = %d", pDoc->fires->varID);
			uvs.Open();
			if(!uvs.IsBOF() && !uvs.IsEOF())
			{
				if(uvs.m_Decimals == 0)
				{
					intFormat = true;
				}
				else
				{
					intFormat = false;
				}
			}
			else//default to integer format
			{
				intFormat = true;
			}
			int loc = 0;
			uvs.Close();
		}
		else if(pDoc->fires->varID == 9)//precip
		{
			intFormat = false;
		}
		else
		{
			intFormat = true;
		}
	}
	CUGCell cell;
	for(int i = 0; i < pDoc->nDP; i++)
	{
		m_grid.AppendRow();
		m_grid.QuickSetNumber(0, i, i + 1);
		m_grid.GetCell(1, i, &cell);
		cell.SetNumberDecimals((intFormat == true) ? 0 : 2);
		cell.SetNumber(pDoc->lowers[i]);
		m_grid.SetCell(1, i, &cell);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDPDialog::OnOK() 
{
	// apply the values to doc/views;
	double prevVal, nextVal;
	if(m_grid.GetNumberRows() < 1)
	{
		pDoc->nDP = 0;
		pDoc->UpdateAllViews(NULL);
		return;
	}
	CUGCell cell;
	m_grid.GetCell(1, 0, &cell);
	prevVal = cell.GetNumber();//pNumCell->GetNumber();
	if(pDoc->fires->nv > 0)
	{
		if(!pDoc->goUp)
		{
			if(prevVal > pDoc->fires->allVals[pDoc->fires->nv - 1])
			{
				CString msg;
				msg.Format("ERROR: Class 1 value %.2f is greater than\n maximum observed value of %.2f.\n",
					prevVal, pDoc->fires->allVals[pDoc->fires->nv - 1]);
				AfxMessageBox(msg);
				return;
			}
		}
		else
		{
		}
	//	if(prevVal //check for if less than 0 or min Value?
	}
	int i;
	for(i = 1; i < m_grid.GetNumberRows(); i++)
	{
		m_grid.GetCell(1, i, &cell);
		nextVal = cell.GetNumber();
		if(!pDoc->goUp)
		{
			if(prevVal >= nextVal)
			{
				CString msg;
				msg.Format("ERROR: Class %d value must be greater than Class %d value.\n"
					"Class values are lower limits for the class.\n"
					"Class values should increase sequentially.",
					i + 1, i);
				AfxMessageBox(msg);
				return;
			}
			if(pDoc->fires->nv > 0)
			{
				if(nextVal > pDoc->fires->allVals[pDoc->fires->nv - 1])
				{
					CString msg;
					msg.Format("ERROR: Class %d value %.2f is greater than\n maximum observed value of %.2f.\n",
						i + 1, nextVal, pDoc->fires->allVals[pDoc->fires->nv - 1]);
					AfxMessageBox(msg);
					return;
				}
			}
		}
		else
		{
			if(prevVal <= nextVal)
			{
				CString msg;
				msg.Format("ERROR: Class %d value must be less than Class %d value.\n"
					"Class values are upper limits for the class.\n"
					"Class values should decrease sequentially.",
					i + 1, i);
				AfxMessageBox(msg);
				return;
			}
			/*if(pDoc->fires->nv > 0)
			{
				if(nextVal > pDoc->fires->allVals[pDoc->fires->nv - 1])
				{
					CString msg;
					msg.Format("ERROR: Class %d value %.2f is greater than\n maximum observed value of %.2f.\n",
						i + 1, nextVal, pDoc->fires->allVals[pDoc->fires->nv - 1]);
					AfxMessageBox(msg);
					return;
				}
			}*/
		}
		prevVal = nextVal;
	}
	//values all ok, set pDoc values and update
	pDoc->nDP = m_grid.GetNumberRows();//(int)table.GetRowSet().GetCount();
	for(i = 0; i < pDoc->nDP; i++)
	{
		m_grid.GetCell(1, i, &cell);
		nextVal = cell.GetNumber();//pNumCell->GetNumber();
		pDoc->lowers[i] = nextVal;
	}
	pDoc->CalcClasses();
	pDoc->UpdateAllViews(NULL);
}

void CDPDialog::OnDeletebutton() 
{
	//remove current row
	if(m_grid.GetNumberRows() > 0)
	{
		int loc = m_grid.GetCurrentRow();
		if(loc >= 0 && loc < m_grid.GetNumberRows())
		{
			m_grid.DeleteRow(loc);
		}
		//renumber the classes
		for(int r = 0; r < m_grid.GetNumberRows(); r++)
		{
			m_grid.QuickSetNumber(0, r, r + 1);
		}
		m_grid.RedrawCol(0);
	}
	
}

void CDPDialog::OnNewbutton() 
{
	if(m_grid.GetNumberRows() >= MAXDPS)
	{
		AfxMessageBox("Limit of nine(9) classes reached.");
		return;
	}
	CUGCell cell;
	
	m_grid.AppendRow();	

	int loc = m_grid.GetNumberRows()-1;

	m_grid.QuickSetNumber(0, loc, loc+1);
	m_grid.GetCell(1, loc, &cell);
	cell.SetNumber(0);
	cell.SetNumberDecimals((intFormat == true) ? 0 : 2);
	m_grid.SetCell(1, loc, &cell);
	m_grid.RedrawAll();
}

void CDPDialog::PostNcDestroy() 
{
	m_Parent->dpDialog = NULL;
	delete this;
}


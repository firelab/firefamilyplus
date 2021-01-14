// FMParamsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NFDRSCalculator2.h"
#include "FMParamsDlg.h"

FMPViewCUG::FMPViewCUG()
{
}

FMPViewCUG::~FMPViewCUG()
{
}

void FMPViewCUG::OnSetup()
{
	m_iCheckBoxIndex = AddCellType( &m_checkBoxCT );
	CRect rect;
	GetClientRect(&rect);
	int cWid = rect.Width() / 12;
	SetCurrentCellMode(2);
	SetNumberCols(20);
	SetColWidth(-1, 0);//10 * cWid);
	CUGCell cell;
	GetHeadingDefault(&cell); 
	cell.SetCellTypeEx(UGCT_NORMALMULTILINE ); 
	SetHeadingDefault(&cell); 
	CFont *rFont = GetFont(0);
	if (rFont)
	{
		CSize fontSize;
		CDC *pDC = GetDC();
		CFont *oldFont = pDC->SelectObject(rFont);
		fontSize = pDC->GetTextExtent("ABC");
		pDC->SelectObject(oldFont);
		ReleaseDC(pDC);
		SetRowHeight(-1, fontSize.cy * 2 + 4);
	}
	//SetRowHeight(-1, 34);//2 * hgt);
	SetColWidth(0, cWid);
	QuickSetText(0, -1, "Fuel\nModel");
	SetColWidth(1, cWid);
	QuickSetText(1, -1, "88\nModel");
	SetColWidth(2, cWid);
	QuickSetText(2, -1, "1 Hour\nLoading");
	SetColWidth(3, cWid);
	QuickSetText(3, -1, "10 Hour\nLoading");
	SetColWidth(4, cWid);
	QuickSetText(4, -1, "100 Hour\nLoading");
	SetColWidth(5, cWid);
	QuickSetText(5, -1, "1000 Hour\nLoading");
	SetColWidth(6, cWid);
	QuickSetText(6, -1, "Herb\nLoading");
	SetColWidth(7, cWid);
	QuickSetText(7, -1, "Woody\nLoading");
	SetColWidth(8, cWid);
	QuickSetText(8, -1, "Drought\nLoading");
	SetColWidth(9, cWid);
	QuickSetText(9, -1, "1 Hour\nSA : Vol");
	SetColWidth(10, cWid);
	QuickSetText(10, -1, "10 Hour\nSA : Vol");
	SetColWidth(11, cWid);
	QuickSetText(11, -1, "100 Hour\nSA : Vol");
	SetColWidth(12, cWid);
	QuickSetText(12, -1, "1000 Hour\nSA : Vol");
	SetColWidth(13, cWid);
	QuickSetText(13, -1, "Herb\nSA : Vol");
	SetColWidth(14, cWid);
	QuickSetText(14, -1, "Woody\nSA : Vol");
	SetColWidth(15, cWid);
	QuickSetText(15, -1, "Heating\n#");
	SetColWidth(16, cWid);
	QuickSetText(16, -1, "Moisture\nExtinction");
	SetColWidth(17, cWid);
	QuickSetText(17, -1, "\nDepth");
	SetColWidth(18, cWid);
	QuickSetText(18, -1, "Wind\nFactor");
	SetColWidth(19, cWid);
	QuickSetText(19, -1, "Max\nSC");

	GetColDefault( 1, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 1, &cell );

	LockColumns(2); 
	GetColDefault(0, &cell);
	cell.SetAlignment(UG_ALIGNCENTER);
	SetColDefault(0, &cell);
	for(int r = 0; r < 20; r++)
	{
		GetColDefault(r, &cell);
		cell.SetReadOnly(TRUE);
		SetColDefault(r, &cell);
	}
}

void FMPViewCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}	

void FMPViewCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}

void FMPViewCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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


// CFMParamsDlg dialog

IMPLEMENT_DYNAMIC(CFMParamsDlg, CDialog)
CFMParamsDlg::CFMParamsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFMParamsDlg::IDD, pParent)
	, m_use88(FALSE)
{
}

CFMParamsDlg::~CFMParamsDlg()
{
}

void CFMParamsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FUELMODEL, m_FMCombo);
	DDX_Check(pDX, IDC_USE88, m_use88);
//	DDX_Control(pDX, IDC_NFDRS4CTRL1, m_nfdrs4);
}


BEGIN_MESSAGE_MAP(CFMParamsDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_TO_TABLE, OnBnClickedAddToTable)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CFMParamsDlg message handlers

void CFMParamsDlg::OnBnClickedAddToTable()
{
	UpdateData();
	int loc = m_FMCombo.GetCurSel();
	if(loc < 0)
	{
		AfxMessageBox("Error: No Fuel Model selected!");
		return;
	}
	bool isNFDRS2016 = false;
	CString model;
	m_FMCombo.GetWindowText(model);
	char temp;
	temp = model[0];
	AddRow(temp, m_use88);
	m_grid.RedrawAll();
	/*m_nfdrs.SetNFDRSVersion(m_use88 ? 88 : 78);
	m_nfdrs.iSetFuelModel(model[0], 0);
	int row = m_grid.GetNumberRows();
	m_grid.AppendRow();
	char temp[64];
	strncpy(temp, model, 1);
	temp[1] = 0;
	m_grid.QuickSetText(0, row, temp);
	m_grid.GetCell(1, row, &cell);
	cell.SetNumber(m_use88);
	m_grid.SetCell(1, row, &cell);
	m_grid.GetCell(2, row, &cell);
	cell.SetNumber(m_nfdrs.GetL1());
	m_grid.SetCell(2, row, &cell);
	m_grid.GetCell(3, row, &cell);
	cell.SetNumber(m_nfdrs.GetL10());
	m_grid.SetCell(3, row, &cell);
	m_grid.GetCell(4, row, &cell);
	cell.SetNumber(m_nfdrs.GetL100());
	m_grid.SetCell(4, row, &cell);
	m_grid.GetCell(5, row, &cell);
	cell.SetNumber(m_nfdrs.GetL1000());
	m_grid.SetCell(5, row, &cell);
	m_grid.GetCell(6, row, &cell);
	cell.SetNumber(m_nfdrs.GetLHerb());
	m_grid.SetCell(6, row, &cell);
	m_grid.GetCell(7, row, &cell);
	cell.SetNumber(m_nfdrs.GetLWood());
	m_grid.SetCell(7, row, &cell);
	m_grid.GetCell(8, row, &cell);
	cell.SetNumber(m_nfdrs.GetLDrought());
	m_grid.SetCell(8, row, &cell);
	m_grid.GetCell(9, row, &cell);
	cell.SetNumber(m_nfdrs.GetSG1());
	m_grid.SetCell(9, row, &cell);
	m_grid.GetCell(10, row, &cell);
	cell.SetNumber(m_nfdrs.GetSG10());
	m_grid.SetCell(10, row, &cell);
	m_grid.GetCell(11, row, &cell);
	cell.SetNumber(m_nfdrs.GetSG100());
	m_grid.SetCell(11, row, &cell);
	m_grid.GetCell(12, row, &cell);
	cell.SetNumber(m_nfdrs.GetSG1000());
	m_grid.SetCell(12, row, &cell);
	m_grid.GetCell(13, row, &cell);
	cell.SetNumber(m_nfdrs.GetSGHerb());
	m_grid.SetCell(13, row, &cell);
	m_grid.GetCell(14, row, &cell);
	cell.SetNumber(m_nfdrs.GetSGWood());
	m_grid.SetCell(14, row, &cell);
	m_grid.GetCell(15, row, &cell);
	cell.SetNumber(m_nfdrs.GetHD());
	m_grid.SetCell(15, row, &cell);
	m_grid.GetCell(16, row, &cell);
	cell.SetNumber(m_nfdrs.GetMXD());
	m_grid.SetCell(16, row, &cell);
	m_grid.GetCell(17, row, &cell);
	cell.SetNumber(m_nfdrs.GetDepth());
	m_grid.SetCell(17, row, &cell);
	m_grid.GetCell(18, row, &cell);
	cell.SetNumber(m_nfdrs.GetWindFC());
	m_grid.SetCell(18, row, &cell);
	m_grid.GetCell(19, row, &cell);
	cell.SetNumber(m_nfdrs.GetSCM());
	m_grid.SetCell(19, row, &cell);
	m_grid.RedrawAll();*/
}

BOOL CFMParamsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect nRect(0, 0, 1, 1);
	//m_nfdrs.Create(NULL, WS_CHILD,//WS_DISABLED,
	//	nRect, this, IDC_NFDRS2);
	m_grid.AttachGrid(this, IDC_GRID);

	m_grid.SetNumberRows(0);
	if(theApp.pOptions)
	{
		for(int m = 0; m < theApp.pOptions->nModels; m++)
		{
			AddRow(theApp.pOptions->models[m], theApp.pOptions->use88s[m]);
		}
	}
	m_grid.GetClientRect(&origRect);
	m_grid.GetWindowRect(&origWRect);
	GetWindowRect(&origWinRect);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CFMParamsDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_grid.m_hWnd)
	{
		CRect tRect, rect;
		m_grid.GetWindowRect(&tRect);
		ScreenToClient(tRect);
		rect.left = tRect.left;
		rect.top = tRect.top;
		rect.right = rect.left + cx - 14;
		rect.bottom = cy - 10;
		m_grid.MoveWindow(&rect);
	}
}

void CFMParamsDlg::OnBnClickedButtonRemove()
{
	int loc = m_grid.GetCurrentRow();
	if(loc >= 0)
	{
		m_grid.DeleteRow(loc);
	}
}

void CFMParamsDlg::OnBnClickedButtonClear()
{
	m_grid.SetNumberRows(0);

}

void CFMParamsDlg::OnClose()
{
	UpdateConfig();
	CDialog::OnClose();
}

void CFMParamsDlg::AddRow(char _model, int _use88)
{
	CUGCell cell;
	bool b_isNFDRS2016 = isNFDRS2016(_model);
	if (!b_isNFDRS2016)
	{
		m_nfdrs.SetNFDRSVersion(_use88 ? 88 : 78);
		m_nfdrs.iSetFuelModel(_model, 0);
		int row = m_grid.GetNumberRows();
		m_grid.AppendRow();
		char temp[64];
		temp[0] = _model;
		temp[1] = 0;
		m_grid.QuickSetText(0, row, temp);
		m_grid.GetCell(1, row, &cell);
		cell.SetNumber(_use88);
		m_grid.SetCell(1, row, &cell);
		m_grid.GetCell(2, row, &cell);
		cell.SetNumber(m_nfdrs.GetL1());
		m_grid.SetCell(2, row, &cell);
		m_grid.GetCell(3, row, &cell);
		cell.SetNumber(m_nfdrs.GetL10());
		m_grid.SetCell(3, row, &cell);
		m_grid.GetCell(4, row, &cell);
		cell.SetNumber(m_nfdrs.GetL100());
		m_grid.SetCell(4, row, &cell);
		m_grid.GetCell(5, row, &cell);
		cell.SetNumber(m_nfdrs.GetL1000());
		m_grid.SetCell(5, row, &cell);
		m_grid.GetCell(6, row, &cell);
		cell.SetNumber(m_nfdrs.GetLHerb());
		m_grid.SetCell(6, row, &cell);
		m_grid.GetCell(7, row, &cell);
		cell.SetNumber(m_nfdrs.GetLWood());
		m_grid.SetCell(7, row, &cell);
		m_grid.GetCell(8, row, &cell);
		cell.SetNumber(m_nfdrs.GetLDrought());
		m_grid.SetCell(8, row, &cell);
		m_grid.GetCell(9, row, &cell);
		cell.SetNumber(m_nfdrs.GetSG1());
		m_grid.SetCell(9, row, &cell);
		m_grid.GetCell(10, row, &cell);
		cell.SetNumber(m_nfdrs.GetSG10());
		m_grid.SetCell(10, row, &cell);
		m_grid.GetCell(11, row, &cell);
		cell.SetNumber(m_nfdrs.GetSG100());
		m_grid.SetCell(11, row, &cell);
		m_grid.GetCell(12, row, &cell);
		cell.SetNumber(m_nfdrs.GetSG1000());
		m_grid.SetCell(12, row, &cell);
		m_grid.GetCell(13, row, &cell);
		cell.SetNumber(m_nfdrs.GetSGHerb());
		m_grid.SetCell(13, row, &cell);
		m_grid.GetCell(14, row, &cell);
		cell.SetNumber(m_nfdrs.GetSGWood());
		m_grid.SetCell(14, row, &cell);
		m_grid.GetCell(15, row, &cell);
		cell.SetNumber(m_nfdrs.GetHD());
		m_grid.SetCell(15, row, &cell);
		m_grid.GetCell(16, row, &cell);
		cell.SetNumber(m_nfdrs.GetMXD());
		m_grid.SetCell(16, row, &cell);
		m_grid.GetCell(17, row, &cell);
		cell.SetNumber(m_nfdrs.GetDepth());
		m_grid.SetCell(17, row, &cell);
		m_grid.GetCell(18, row, &cell);
		cell.SetNumber(m_nfdrs.GetWindFC());
		m_grid.SetCell(18, row, &cell);
		m_grid.GetCell(19, row, &cell);
		cell.SetNumber(m_nfdrs.GetSCM());
		m_grid.SetCell(19, row, &cell);
	}
	else//NFDRS2016
	{
		//m_nfdrs2016.SetNFDRSVersion(_use88 ? 88 : 78);
		m_nfdrs2016.iSetFuelModel(_model);
		int row = m_grid.GetNumberRows();
		m_grid.AppendRow();
		char temp[64];
		temp[0] = _model;
		temp[1] = 0;
		m_grid.QuickSetText(0, row, temp);
		m_grid.GetCell(1, row, &cell);
		cell.SetNumber(-1);
		m_grid.SetCell(1, row, &cell);
		m_grid.GetCell(2, row, &cell);
		cell.SetNumber(m_nfdrs2016.L1);// .GetL1());
		m_grid.SetCell(2, row, &cell);
		m_grid.GetCell(3, row, &cell);
		cell.SetNumber(m_nfdrs2016.L10);//GetL10());
		m_grid.SetCell(3, row, &cell);
		m_grid.GetCell(4, row, &cell);
		cell.SetNumber(m_nfdrs2016.L100);//GetL100());
		m_grid.SetCell(4, row, &cell);
		m_grid.GetCell(5, row, &cell);
		cell.SetNumber(m_nfdrs2016.L1000);//.GetL1000());
		m_grid.SetCell(5, row, &cell);
		m_grid.GetCell(6, row, &cell);
		cell.SetNumber(m_nfdrs2016.LHERB);// .GetLHerb());
		m_grid.SetCell(6, row, &cell);
		m_grid.GetCell(7, row, &cell);
		cell.SetNumber(m_nfdrs2016.LWOOD);// .GetLWood());
		m_grid.SetCell(7, row, &cell);
		m_grid.GetCell(8, row, &cell);
		cell.SetNumber(m_nfdrs2016.LDROUGHT);// .GetLDrought());
		m_grid.SetCell(8, row, &cell);
		m_grid.GetCell(9, row, &cell);
		cell.SetNumber(m_nfdrs2016.SG1);// .GetSG1());
		m_grid.SetCell(9, row, &cell);
		m_grid.GetCell(10, row, &cell);
		cell.SetNumber(m_nfdrs2016.SG10);//.GetSG10());
		m_grid.SetCell(10, row, &cell);
		m_grid.GetCell(11, row, &cell);
		cell.SetNumber(m_nfdrs2016.SG100);//.GetSG100());
		m_grid.SetCell(11, row, &cell);
		m_grid.GetCell(12, row, &cell);
		cell.SetNumber(m_nfdrs2016.SG1000);//.GetSG1000());
		m_grid.SetCell(12, row, &cell);
		m_grid.GetCell(13, row, &cell);
		cell.SetNumber(m_nfdrs2016.SGHERB);//.GetSGHerb());
		m_grid.SetCell(13, row, &cell);
		m_grid.GetCell(14, row, &cell);
		cell.SetNumber(m_nfdrs2016.SGWOOD);
		m_grid.SetCell(14, row, &cell);
		m_grid.GetCell(15, row, &cell);
		cell.SetNumber(m_nfdrs2016.HD);// .GetHD());
		m_grid.SetCell(15, row, &cell);
		m_grid.GetCell(16, row, &cell);
		cell.SetNumber(m_nfdrs2016.MXD);// .GetMXD());
		m_grid.SetCell(16, row, &cell);
		m_grid.GetCell(17, row, &cell);
		cell.SetNumber(m_nfdrs2016.DEPTH);// .GetDepth());
		m_grid.SetCell(17, row, &cell);
		m_grid.GetCell(18, row, &cell);
		cell.SetNumber(m_nfdrs2016.WNDFC);// .GetWindFC());
		m_grid.SetCell(18, row, &cell);
		m_grid.GetCell(19, row, &cell);
		cell.SetNumber(m_nfdrs2016.SCM);// .GetSCM());
		m_grid.SetCell(19, row, &cell);

	}
}

void CFMParamsDlg::UpdateConfig()
{
	CUGCell cell;
	CString tmpStr;
	if(theApp.pOptions)
	{
		theApp.pOptions->nModels = m_grid.GetNumberRows();
		if(theApp.pOptions->models)
		{
			delete[] theApp.pOptions->models;
			theApp.pOptions->models = NULL;
		}
		if(theApp.pOptions->use88s)
		{
			delete[] theApp.pOptions->use88s;
			theApp.pOptions->use88s = NULL;
		}
		if(theApp.pOptions->nModels > 0)
		{
			theApp.pOptions->models = new char[theApp.pOptions->nModels];
			theApp.pOptions->use88s = new int[theApp.pOptions->nModels];
			for(int r = 0; r < m_grid.GetNumberRows(); r++)
			{
				m_grid.QuickGetText(0, r, &tmpStr);
				theApp.pOptions->models[r] = tmpStr[0];
				m_grid.GetCell(1, r, &cell);
				theApp.pOptions->use88s[r] = cell.GetNumber();
			}
		}
	}
}

void CFMParamsDlg::OnDestroy()
{
	UpdateConfig();
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

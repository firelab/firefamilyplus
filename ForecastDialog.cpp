// ForecastDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "ForecastDialog.h"
#include "ForecastOptionsDialog.h"
#include "fireplusSet.h"
#include "ClimateSet.h"
#include "SIGStationSet.h"
#include "wxSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "splitterframe.h"
#include "FireSumSet.h"
#include "ClimAnalysis.h"
#include "stationInSIGSet.h"
#include "ForcastGraphsDialog.h"

#include "fireplusSet.h"
#include "fireplusDoc.h"
#include "fireplusView.h"

#include <afxrich.h>
#include "richdoc.h"

#include "FFPfxOptsSet.h"
#include "FFPfxVarsSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern char *abbrevs[];
extern char *varFormats[];

FxWxCUG::FxWxCUG()
{
}

FxWxCUG::~FxWxCUG()
{
}

void FxWxCUG::OnSetup()
{
	CUGCell cell;
	CRect rect;
	GetClientRect(&rect);
	int wid = rect.Width() / 22;
	SetCurrentCellMode(2);
	SetDefColWidth(2 * wid);
	SetNumberCols(14);
	QuickSetText(0, -1, "Date"); 
	QuickSetText(1, -1, "Temp");
	QuickSetText(2, -1, "MaxTmp");
	QuickSetText(3, -1, "MinTmp"); 
	QuickSetText(4, -1, "RH");
	QuickSetText(5, -1, "MaxRH");
	QuickSetText(6, -1, "MinRH"); 
	QuickSetText(7, -1, "Wind Spd");
	QuickSetText(8, -1, "Pcp Amt");
	QuickSetText(9, -1, "Pcp Dur"); 
	QuickSetText(10, -1, "SOW");
	QuickSetText(11, -1, "Herb");
	QuickSetText(12, -1, "Woody"); 
	QuickSetText(13, -1, "Season");
	SetColWidth(-1, 0);
	SetColWidth(0, 2 * wid);
	GetColDefault(0, &cell);
	cell.SetReadOnly(TRUE);
	cell.SetMask("mm/dd/YYYY");
	SetColDefault(0,&cell);
	GetColDefault(1, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(1,&cell);
	GetColDefault(2, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(2,&cell);
	GetColDefault(3, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(3,&cell);
	GetColDefault(4, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(4,&cell);
	GetColDefault(5, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(5,&cell);
	GetColDefault(6, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(6,&cell);
	GetColDefault(7, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(7,&cell);
	GetColDefault(8, &cell);
	cell.SetNumberDecimals(2);
	SetColDefault(8,&cell);
	GetColDefault(9, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(9,&cell);
	GetColDefault(10, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(10,&cell);
	GetColDefault(11, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(11,&cell);
	GetColDefault(12, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(12,&cell);
	GetColDefault(13, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(13,&cell);
	((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );
	//BestFit(1, 13,
/*
	CRect rect;
	m_wxGrid.GetClientRect(&rect);
	m_wWid = rect.Width() / 15;
	m_wxGrid.SetVirtualMode(FALSE);
	m_wxGrid.SetHeaderSort(FALSE);
	m_wxGrid.SetListMode(FALSE);
	m_wxGrid.SetFrameFocusCell(FALSE);
	m_wxGrid.SetTrackFocusCell(FALSE);
	m_wxGrid.SetDefCellWidth(m_wWid);
	m_wxGrid.SetColumnCount(14); 
	m_wxGrid.SetRowCount(1);
	m_wxGrid.SetFixedRowCount(1);
	m_wxGrid.SetFixedColumnCount(1);
	m_wxGrid.SetItemText(0, 0, "Date");
	m_wxGrid.SetColumnWidth(0, 2 * m_wWid);
	m_wxGrid.SetItemText(0, 1, "Temp");
	m_wxGrid.SetItemText(0, 2, "MaxTmp");
	m_wxGrid.SetItemText(0, 3, "MinTmp");
	m_wxGrid.SetItemText(0, 4, "RH");
	m_wxGrid.SetItemText(0, 5, "MaxRH");
	m_wxGrid.SetItemText(0, 6, "MinRH");
	m_wxGrid.SetItemText(0, 7, "Wind Speed");
	m_wxGrid.SetItemText(0, 8, "Precip Amt");
	m_wxGrid.SetItemText(0, 9, "Hours Precip");
	m_wxGrid.SetItemText(0, 10, "SOW");
	m_wxGrid.SetItemText(0, 11, "Herb Green");
	m_wxGrid.SetItemText(0, 12, "Woody Green");
	m_wxGrid.SetItemText(0, 13, "Season");
	m_wxGrid.EnableTitleTips(FALSE);
	m_wxGrid.EnableToolTips(FALSE);
	m_wxGrid.SetGridLines(GVL_BOTH);
*/
}

int FxWxCUG::OnEditStart(int col, long row,CWnd **edit)
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

void FxWxCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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


void FxWxCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}	

void FxWxCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}
	
COLORREF FxWxCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}

FxVarCUG::FxVarCUG()
{
}

FxVarCUG::~FxVarCUG()
{
}

void FxVarCUG::OnSetup()
{
	CUGCell cell;
	CRect rect;
	GetClientRect(&rect);
	int wid = rect.Width() / 15;
	SetCurrentCellMode(2);
	SetDefColWidth(2 * wid);
	SetNumberCols(20);
	QuickSetText(0, -1, "Date"); 
	QuickSetText(1, -1, "SC");
	QuickSetText(2, -1, "ERC");
	QuickSetText(3, -1, "BI"); 
	QuickSetText(4, -1, "KBDI");
	QuickSetText(5, -1, "IC");
	QuickSetText(6, -1, "1 Hr"); 
	QuickSetText(7, -1, "10 Hr");
	QuickSetText(8, -1, "100 Hr");
	QuickSetText(9, -1, "1000 Hr"); 
	QuickSetText(10, -1, "Herb FM");
	QuickSetText(11, -1, "Woody FM");
	QuickSetText(12, -1, "X1000"); 
	QuickSetText(13, -1, "FFMC");
	QuickSetText(14, -1, "DMC");
	QuickSetText(15, -1, "DC"); 
	QuickSetText(16, -1, "ISI");
	QuickSetText(17, -1, "BUI");
	QuickSetText(18, -1, "FWI"); 
	QuickSetText(19, -1, "DSR");
	SetColWidth(-1, 0);
	SetColWidth(0, 2 * wid);
	GetColDefault(0, &cell);
	cell.SetReadOnly(TRUE);
	SetColDefault(0,&cell);
	GetColDefault(1, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(1,&cell);
	GetColDefault(2, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(2,&cell);
	GetColDefault(3, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(3,&cell);
	GetColDefault(4, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(4,&cell);
	GetColDefault(5, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(5,&cell);
	GetColDefault(6, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(6,&cell);
	GetColDefault(7, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(7,&cell);
	GetColDefault(8, &cell);
	cell.SetNumberDecimals(2);
	SetColDefault(8,&cell);
	GetColDefault(9, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(9,&cell);
	GetColDefault(10, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(10,&cell);
	GetColDefault(11, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(11,&cell);
	GetColDefault(12, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(12,&cell);
	GetColDefault(13, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(13,&cell);
	GetColDefault(14, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(14,&cell);
	GetColDefault(15, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(15,&cell);
	GetColDefault(16, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(16,&cell);
	GetColDefault(17, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(17,&cell);
	GetColDefault(18, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(18,&cell);
	GetColDefault(19, &cell);
	cell.SetNumberDecimals(0);
	SetColDefault(19,&cell);
	((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );
/*
	//variables grid
	m_varGrid.SetVirtualMode(FALSE);
	m_varGrid.SetHeaderSort(FALSE);
	m_varGrid.SetListMode(FALSE);
	m_varGrid.SetFrameFocusCell(FALSE);
	m_varGrid.SetTrackFocusCell(FALSE);
	m_varGrid.SetDefCellWidth(m_wWid);
	m_varGrid.EnableHiddenColUnhide(FALSE);
	m_varGrid.EnableColumnHide(FALSE);
	m_varGrid.SetColumnCount(20); 
	m_varGrid.SetRowCount(1);
	m_varGrid.SetFixedRowCount(1);
	m_varGrid.SetFixedColumnCount(1);
	m_varGrid.SetItemText(0, 0, "Date");
	m_varGrid.SetColumnWidth(0, 2 * m_wWid);
	m_varGrid.SetItemText(0, 1, "SC");
	m_varGrid.SetItemText(0, 2, "ERC");
	m_varGrid.SetItemText(0, 3, "BI");
	m_varGrid.SetItemText(0, 4, "KBDI");
	m_varGrid.SetItemText(0, 5, "IC");
	m_varGrid.SetItemText(0, 6, "1 Hr FM");
	m_varGrid.SetItemText(0, 7, "10 Hr FM");
	m_varGrid.SetItemText(0, 8, "100 Hr FM");
	m_varGrid.SetItemText(0, 9, "1000 Hr FM");
	m_varGrid.SetItemText(0, 10, "Herb FM");
	m_varGrid.SetItemText(0, 11, "Woody FM");
	m_varGrid.SetItemText(0, 12, "X1000");
	m_varGrid.SetItemText(0, 13, "FFMC");
	m_varGrid.SetItemText(0, 14, "DMC");
	m_varGrid.SetItemText(0, 15, "DC");
	m_varGrid.SetItemText(0, 16, "ISI");
	m_varGrid.SetItemText(0, 17, "BUI");
	m_varGrid.SetItemText(0, 18, "FWI");
	m_varGrid.SetItemText(0, 19, "DSR");
	m_varGrid.EnableTitleTips(FALSE);
	m_varGrid.EnableToolTips(FALSE);
	m_varGrid.SetGridLines(GVL_BOTH);

*/
}

int FxVarCUG::OnEditStart(int col, long row,CWnd **edit)
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

void FxVarCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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


void FxVarCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}	

void FxVarCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey ); 
}
	
COLORREF FxVarCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);	
}



/////////////////////////////////////////////////////////////////////////////
// CForecastDialog dialog

int VarIDtoVTCol(int varID)
{
	if(varID >= 12 && varID <= 23)
		return varID - 11;
	if(varID >= 26 && varID <= 32)
		return varID - 13;
	return -1;
}

int VTColToVarID(int vtCol)
{
	if(vtCol >= 1 && vtCol <= 12)
		return vtCol + 11;
	if(vtCol >= 13 && vtCol <= 19)
		return vtCol + 13;
	return -1;
}

int VarIDtoWxCol(int varID)
{
	return -1;
}

int WxColToVarID(int wxCol)
{
	switch(wxCol)
	{
	case 1:
		return 1;
	case 2:
		return 4;
	case 3:
		return 3;
	case 4:
		return 5;
	case 5:
		return 8;
	case 6:
		return 7;
	case 7:
		return 11;
	case 8:
		return 9;
	case 9:
		return 10;
	case 10:
		return 25;
	case 11:
		return 21;
	case 12:
		return 22;
	case 13:
		return 22;//dummy...only to reference formatting
	default:
		return -1;
	}
}

CForecastDialog::CForecastDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet, CClimAnalysis *_analysis)
	: CDialog(CForecastDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CForecastDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	fpSet = _fpSet;
	m_nID = CForecastDialog::IDD;
	m_Parent = pParent;
	analysis = _analysis;
}

bool CForecastDialog::Create()
{
	return (CDialog::Create(m_nID, m_Parent) == 0) ? false : true;
}

void CForecastDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CForecastDialog)
	DDX_Control(pDX, IDC_GRAPH, graphButton);
	//	DDX_Control(pDX, IDC_DATATABLEDB1, wxTable);
	//	DDX_Control(pDX, IDC_DATATABLEDB2, varTable);
	//DDX_Control(pDX, IDC_NFDRSCTRL1, m_NFDRS);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_DATETIMEPICKER1, startDateCtl);
	//DDX_Control(pDX, IDC_GRID1, m_wxGrid);
	//DDX_Control(pDX, IDC_GRID2, m_varGrid);
	//DDX_Control(pDX, IDC_NFDRS4CTRL1, m_NFDRS);
}


BEGIN_MESSAGE_MAP(CForecastDialog, CDialog)
	//{{AFX_MSG_MAP(CForecastDialog)
	ON_BN_CLICKED(IDC_REMOVE, OnRemove)
	ON_BN_CLICKED(IDC_GRAPH, OnGraph)
	ON_BN_CLICKED(IDC_ADDDAY, OnAddday)
	ON_BN_CLICKED(IDC_OPTIONS, OnOptions)
	ON_BN_CLICKED(IDC_CALCULATE, OnCalculate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CForecastDialog message handlers

//BEGIN_EVENTSINK_MAP(CForecastDialog, CDialog)
    //{{AFX_EVENTSINK_MAP(CForecastDialog)
	//}}AFX_EVENTSINK_MAP
//END_EVENTSINK_MAP()

BOOL CForecastDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_wxGrid.AttachGrid(this, IDC_GRID1);
	m_varGrid.AttachGrid(this, IDC_GRID2);


	//varTable.GetRowSet().Reset();
	//wxTable.GetRowSet().Reset();
//	AfxGetMainWnd()->EnableWindow(TRUE);
	//get last valid wx day
	analysis->GetLastDay(lastDay);
	COleDateTimeSpan day(1, 0, 0, 0);
	//COleDateTime nextDay;
	beginDay = lastDay + day;
	startDateCtl.SetTime(beginDay);
	if(options.Read(fpSet->m_pDatabase) != 0)
		OnOptions();
	else
		ConfigureOutputs();
	CSIGStationSet staSet(fpSet->m_pDatabase);
	if(strncmp(fpSet->m_SIG_Station, "SIG", 3) == 0)//SIG
	{
		char temp[64], sig[128];
		strcpy_s(sig, fpSet->m_SIG_Station);
		CStationInSIGSet sSet(fpSet->m_pDatabase);
		sprintf(temp, "[SIG] = '%-20.20s'", &sig[6]);
		sSet.m_strFilter = _T(temp);
		sSet.Open();
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
		sSet.Close();
	}
	else
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	staSet.Open();//have station or representative
	if(!staSet.m_Use88)
	{
		//for(int v = 11; v < wxTable.GetColumnSet().GetCount(); v++)
		for(int v = 11; v < m_wxGrid.GetNumberCols(); v++)
		{
			m_wxGrid.SetColWidth(v, 0);
			//wxTable.GetColumnSet().GetItem(v).SetHidden(TRUE);
		}
	}
	//analysis->nfdrsState.Copy(&lastState);
	//nfdrs.LoadState(&lastState);
//	CString sfName;
//	sfName.Format("%s\\%s", theApp.workDir, STATEFILENAME);
//	m_NFDRS.LoadState(sfName);
	analysis->canadaState.Copy(&canState);
	canadian.LoadState(&canState);
	//sync models to last date
/*	nfdrs.iInitialize(staSet.m_Use88 ? 88 : 78, 
			staSet.m_ClimateCls, DayOfYear(staSet.m_FreezeJulian), DayOfYear(staSet.m_GreenJulian),
			staSet.m_LatDegrees, staSet.m_HerbAnnual, staSet.m_Deciduous, staSet.m_FM1Eq10, 
			staSet.m_Start1000 - 5.0, staSet.m_Start1000, staSet.m_StartKBDI, staSet.m_AvgPrecip);
	nfdrs.iSetFuelModel(staSet.m_NFDRSFM[0], 0);
	canadian.Initialize();
	CWxSet wxSet(fpSet->m_pDatabase);
	wxSet.m_strFilter.Format("[StationID] = '%6.6s' and (Year([ObsDate]) = %d"
		" or ((Year([ObsDate]) = %d and Month([ObsDate]) < %d)" 
		" or (Month([ObsDate]) = %d and Day([ObsDate]) <= %d)))", 
		staSet.m_StationID, lastDay.GetYear() - 1, lastDay.GetYear(), lastDay.GetMonth(),
		lastDay.GetMonth(), lastDay.GetDay());
	wxSet.Open();
	//crunch data...
	int iSC, iFIL, iBI;
	int iIC, iStage, iKBDI, iRainEvent, init = 1, iDeclareGreenUp = 0, iDeclareFreeze = 0;
	double f1, f10, f100, f1000, fHerb, fWood, fGren, fX1000, fROS, fERC, fFL;
		//ffmc, dmc, dc, bui, isi, fwi, dsr;
	COleDateTime yesterday;//used to skip duplicates
	while(!wxSet.IsEOF())
	{
		if(wxSet.IsFieldNull(&wxSet.m_Temp) 
			|| wxSet.IsFieldNull(&wxSet.m_RH)
			|| wxSet.IsFieldNull(&wxSet.m_ObsDate))
		{
			wxSet.MoveNext();
			continue;
		}
		if(!init && yesterday >= wxSet.m_ObsDate)
		{
			wxSet.MoveNext();
			continue;
		}
		yesterday = wxSet.m_ObsDate;
		//working variables
		int tmp, tmpMax, tmpMin, rh, rhMax, rhMin, pptDur, sow, greenHerb, greenShrub,
			season, windSpd, windDir, slopeClass, wxMonth, wxDay;
		double pptAmt, omc10, ffmc, dmc, dc, bui, isi, fwi, dsr; 
		tmp = wxSet.m_Temp;
		tmpMax = wxSet.IsFieldNull(&wxSet.m_TmpMax) ? -10000 : wxSet.m_TmpMax;
		tmpMin = wxSet.IsFieldNull(&wxSet.m_TmpMin) ? -10000 : wxSet.m_TmpMin;
		rh = wxSet.m_RH;
		rhMax = wxSet.IsFieldNull(&wxSet.m_RHMax) ? -10000 : wxSet.m_RHMax;
		rhMin = wxSet.IsFieldNull(&wxSet.m_RHMin) ? -10000 : wxSet.m_RHMin;
		pptDur = wxSet.IsFieldNull(&wxSet.m_PPTDUR) ? 0 : wxSet.m_PPTDUR;
		sow = wxSet.IsFieldNull(&wxSet.m_SOW) ? 0 : wxSet.m_SOW;
		greenHerb = wxSet.IsFieldNull(&wxSet.m_GreenHerb) ? 0 : wxSet.m_GreenHerb;
		greenShrub = wxSet.IsFieldNull(&wxSet.m_GreenShrub) ? 0 : wxSet.m_GreenShrub;
		season = wxSet.IsFieldNull(&wxSet.m_Season) ? 0 : wxSet.m_Season;
		windSpd = wxSet.IsFieldNull(&wxSet.m_WS) ? 0 : wxSet.m_WS;
		windDir = wxSet.IsFieldNull(&wxSet.m_WDir) ? 0 : wxSet.m_WDir;
		slopeClass = staSet.m_SlopeCls;
		slopeClass = max(slopeClass, 1);
		slopeClass = min(slopeClass, 5);
		pptAmt = wxSet.IsFieldNull(&wxSet.m_PPTAMT) ? 0.0 : wxSet.m_PPTAMT;
		omc10 = wxSet.IsFieldNull(&wxSet.m_OMC10) ? -1000.0 : wxSet.m_OMC10;
		wxMonth = wxSet.m_ObsDate.GetMonth();
		wxDay = wxSet.m_ObsDate.GetDay();
		//calculate model outputs
		//NFDRS
		nfdrs.iCalcMoist(init, tmp, rh, 
			tmpMax, 
			rhMax, 
			tmpMin, 
			rhMin, 
			pptDur, 
			pptAmt, 
			sow, 
			omc10, 
			DayOfYear(wxSet.m_ObsDate), 
			wxSet.m_ObsDate.GetYear(), 
			iDeclareGreenUp, 
			iDeclareFreeze, 
			greenHerb, 
			greenShrub,
			season,
			&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI);
		nfdrs.iCalcIndexes(windSpd, slopeClass, &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
		nfdrs.iCalcIC(tmp, sow, f1, iSC, &iIC);
		canadian.CalcIndices(DayOfYear(wxSet.m_ObsDate), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetYear(), 
					tmp, rh, windSpd * 1.15, pptAmt,
					&ffmc, &dmc, &dc, &bui, &isi, &fwi, &dsr);
		init = 0;
		wxSet.MoveNext();
	}*/
	staSet.Close();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CForecastDialog::DoReport() 
{
	CWaitCursor wait;
	//CPVDTCurCell curCell = wxTable.GetCurCell();
	//if(curCell)
	//	curCell.EndEdit();
	bool isSIG = false;
	CSIGStationSet staSet(fpSet->m_pDatabase);
	if(strncmp(fpSet->m_SIG_Station, "SIG", 3) == 0)//SIG
	{
		char temp[64], sig[128];
		isSIG = true;
		strcpy_s(sig, fpSet->m_SIG_Station);
		CStationInSIGSet sSet(fpSet->m_pDatabase);
		sprintf(temp, "[SIG] = '%-20.20s'", &sig[6]);
		sSet.m_strFilter = _T(temp);
		sSet.Open();
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
		sSet.Close();
	}
	else
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	staSet.Open();//have station or representative
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");
	fprintf(stream, "FireFamily Plus Fire Danger Projection Report\n\n");
	if(!isSIG)
	{
		fprintf(stream, " Station: %6.6s: %-20.20s\n",
				fpSet->m_SIG_Station, staSet.m_Name);
		fprintf(stream, " Model: %d%s%d%c%c%d\n", 
			staSet.m_Use88 ? 8 : 7, staSet.m_NFDRSFM, staSet.m_SlopeCls,
			staSet.m_HerbAnnual ? 'A' : 'P', staSet.m_Deciduous ? 'D' : 'E', staSet.m_ClimateCls);
	}
	else
	{
		fprintf(stream, " %s\n",
				fpSet->m_SIG_Station);
		fprintf(stream, " Model: %d%s\n", 
			staSet.m_Use88 ? 8 : 7, staSet.m_NFDRSFM);
	}
	fprintf(stream, "\n Projected Weather:\n");
	fprintf(stream, " Date     Temp MaxTmp MinTmp   RH MaxRH MinRH  Wind Precip Hours  SOW");
	if(staSet.m_Use88)
		fprintf(stream, "   Herb Woody Seas");
	fprintf(stream, "\n---------------------------------------------------------------------");
	if(staSet.m_Use88)
		fprintf(stream, "------------------");
	fprintf(stream, "\n");

	//dump data
	char frmt[16];
	double val;
	//CGridCellNumeric *pNumCell;
	//for(long i = 0; i < wxTable.GetRowSet().GetCount(); i++)
	CUGCell cell;
	int second, minute, hour, day, month, year;
	long i;
	for(i = 0; i < m_wxGrid.GetNumberRows(); i++)
	{
		//CDTData wxData = wxTable.GetRowSet().GetItem(i).GetValue();
		//COleDateTime day;
		//wxData.GetItem(day, 0);
		//CGridCellDateTime *pDateCell = (CGridCellDateTime *)m_wxGrid.GetCell(i + 1, 0);
		//CTime *ctime = pDateCell->GetTime();
		m_wxGrid.GetCell(0, i, &cell);
		cell.GetTime(&second, &minute, &hour, &day, &month, &year);
		CString str;
		str.Format("%d/%d/%d", month, day, year);
		//CString str = day.Format("%m/%d/%Y");
		fprintf(stream, "%s", str);
		for(int v = 1; v < 11; v++)
		{
			int loc = WxColToVarID(v);//atoi(wxTable.GetColumnSet().GetItem(v).GetName()) - 1;
			//wxData.GetItem(&val, v);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, v);
			m_wxGrid.GetCell(v, i, &cell);
			val = cell.GetNumber();
			sprintf(frmt, "%%%s ", varFormats[loc]);
			fprintf(stream, frmt, val);
		}
		if(staSet.m_Use88)
		{
			//for(int v = 11; v < wxTable.GetColumnSet().GetCount(); v++)
			for(int v = 11; v < m_wxGrid.GetNumberCols(); v++)
			{
				int loc = WxColToVarID(v);
				//int loc = atoi(wxTable.GetColumnSet().GetItem(v).GetName()) - 1;
				//wxData.GetItem(&val, v);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, v);
				//val = pNumCell->GetNumber();
				m_wxGrid.GetCell(v, i, &cell);
				val = cell.GetNumber();
				sprintf(frmt, "%%%s ", varFormats[loc]);
				fprintf(stream, frmt, val);
			}
		}
		fprintf(stream, "\n");
	}

	fprintf(stream, "\n Calculated Outputs:\n Date     ");
	POSITION pos = options.outputsList.GetHeadPosition();
	while(pos)
	{
		int val = options.outputsList.GetNext(pos);
		fprintf(stream, " %5.5s", abbrevs[val - 1]);
	}
	fprintf(stream, "\n----------");
	for(int v = 0; v < options.outputsList.GetCount(); v++)
		fprintf(stream, "------");
	fprintf(stream, "\n");
		//calculated outputs
	//for(i = 0; i < varTable.GetRowSet().GetCount(); i++)
	for(i = 0; i < m_varGrid.GetNumberRows(); i++)
	{
		//CDTData varData = varTable.GetRowSet().GetItem(i).GetValue();
		//if(varData)
		//{
			//COleDateTime day;
			//varData.GetItem(day, 0);
		//CTime *ctime;
		//CGridCellDateTime *pDateCell = (CGridCellDateTime *)m_varGrid.GetCell(i + 1, 0);
		//ctime = pDateCell->GetTime();
		m_varGrid.GetCell(0, i, &cell);
		cell.GetTime(&second, &minute, &hour, &day, &month, &year);
		CString str;
		str.Format("%d/%d/%d", month, day, year);
		fprintf(stream, "%s", str);
		//for(int c = 1; c < varTable.GetColumnSet().GetCount(); c++)
		for(int c = 1; c < m_varGrid.GetNumberCols(); c++)
		{
			//if(varTable.GetColumnSet().GetItem(c).GetHidden() == FALSE)
			if(m_varGrid.GetColWidth(c) > 0)
			{
				int loc = VTColToVarID(c);//atoi(varTable.GetColumnSet().GetItem(c).GetName()) - 1;
				//varData.GetItem(&val, c);
				//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(i + 1, c);
				//val = pNumCell->GetNumber();
				m_varGrid.GetCell(c, i, &cell);
				val = cell.GetNumber();
				sprintf(frmt, " %%%s", varFormats[loc]);
				fprintf(stream, frmt, val);
			}
		}
		fprintf(stream, "\n");
	}
	fprintf(stream, "\n");
	if(isSIG)
		analysis->AddSIGDescription(stream);
	fprintf(stream, "\n%s\n", analysis->AddDateStamp());
	fclose(stream);
	staSet.Close();
	
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("TextFile"))
		{
			CRichDoc *pDoc = (CRichDoc *)curTemplate->OpenDocumentFile(fName);
			pDoc->isTemp = true;
			CString tTitle;
			//GetWindowText(tTitle);
			tTitle.Format("%s - Fire Danger Projection", fpSet->m_SIG_Station);
			pDoc->SetTitle(tTitle);
			break;;
		}
	}
	free(fName);
}

void CForecastDialog::OnRemove() 
{
//	CPVDTCurCell curCell = wxTable.GetCurCell();
//	if(curCell)
		//curCell.EndEdit();
	int cnt = m_wxGrid.GetNumberRows() - 1;//wxTable.GetRowSet().GetCount();
	if(cnt >= 0)
		//wxTable.GetRowSet().Remove(cnt - 1);
		m_wxGrid.DeleteRow(cnt);
	if(cnt <= 1)
		graphButton.EnableWindow(FALSE);
}

void CForecastDialog::OnGraph() 
{
//	CPVDTCurCell curCell = wxTable.GetCurCell();
//	if(curCell)
//		curCell.EndEdit();
	CUGCell cell;
	int second, minute, hour, day, month, year;
	CForcastGraphsDialog fgd(this);
	if(fgd.DoModal() == IDOK)
	{
		if(fgd.nVars > 0)
		{
			//run climanalysis on selected variables and graph,
			//adding predicted values to graphs and highlighting predicted year
			CClimAnalysis *tanalysis = new CClimAnalysis(fpSet->m_pDatabase);
			//EnableWindow(FALSE);
			int ret = tanalysis->RunAnalyze(fgd.varIDs, fgd.nVars, fpSet, false);
				//Analyze(this, fgd.varIDs, fgd.nVars, fpSet, false);
			//EnableWindow(TRUE);
			if(ret != IDOK)
				return;
			COleDateTime sTime;
			startDateCtl.GetTime(sTime);
			theApp.overLayYears[0] = sTime.GetYear();//pdDoc->forecast.dates[0].GetYear();
			theApp.overLayColors[0] = RGB(255, 0, 255);
			theApp.lineWidths[0] = 1;
			theApp.lineStyles[0] = 0;
			CPeriodDoc *pdDoc = NULL;
			CClimateSet climSet(fpSet->m_pDatabase);
			climSet.Open();
			tanalysis->climSet = &climSet;
			for(int i = 0; i < fgd.nVars; i++)
			{
				climSet.m_strFilter.Format("[VarID] = %d", fgd.varIDs[i] + 1);
				climSet.Requery();
				pdDoc = tanalysis->AddGraphView(pdDoc, climSet.m_VarID - 1, climSet.m_OptionType);
			}
			climSet.Close();
			if(pdDoc)
			{	//ensure data kept live 
				tanalysis->killPeriods = false;
				//add forecast data to pdDoc
				//int fDays = varTable.GetRowSet().GetCount();
				int fDays = m_varGrid.GetNumberRows();// - 1;///varTable.GetRowSet().GetCount();
				pdDoc->m_pAnalysis->m_opts.forecast.CreateStorage(fDays, fgd.nVars, fgd.varIDs);
				pdDoc->m_pAnalysis->m_opts.forecast.lineColor = options.lineColor;
				pdDoc->m_pAnalysis->m_opts.forecast.lineWidth = options.lineWidth;
				pdDoc->m_pAnalysis->m_opts.forecast.lineStyle = options.lineStyle;
				for(int d = 0; d < pdDoc->m_pAnalysis->m_opts.forecast.nDays; d++)
				{
					//CDTData varData = varTable.GetRowSet().GetItem(d).GetValue();
					//COleDateTime day;
					//varData.GetItem(day, 0);
					m_varGrid.GetCell(0, d, &cell);
					cell.GetTime(&second, &minute, &hour, &day, &month, &year);
					//CString str;
					//str.Format("%d/%d/%d", month, day, year);
					//CGridCellDateTime *pDateCell = (CGridCellDateTime *)m_varGrid.GetCell(d + 1, 0);
					//CTime *ctime = pDateCell->GetTime();
					//pdDoc->m_pAnalysis->m_opts.forecast.dates[d].SetDateTime(day.GetYear(), day.GetMonth(), day.GetDay(), 14, 0, 0);
					pdDoc->m_pAnalysis->m_opts.forecast.dates[d].SetDateTime(year, month, day, 14, 0, 0);
					for(int v = 0; v < fgd.nVars; v++)
					{
						//for(int c = 1; c < varTable.GetColumnSet().GetCount(); c++)
						for(int c = 1; c < m_varGrid.GetNumberCols(); c++)
						{
							//if(atoi(varTable.GetColumnSet().GetItem(c).GetName()) == fgd.varIDs[v] + 1)
							if(VTColToVarID(c) == fgd.varIDs[v] + 1)
							{
								//double val;
								//varData.GetItem(&val, c);
								m_varGrid.GetCell(c, d, &cell);
								//CGridCellNumeric *pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(d + 1, c);
								pdDoc->m_pAnalysis->m_opts.forecast.values[fgd.varIDs[v]][d] = cell.GetNumber();//pNumCell->GetNumber();//val;
							}
						}
					}
				}
				pdDoc->m_pAnalysis->m_opts.forecast.FillBins(tanalysis);
			}
		}
		if(fgd.bReport == TRUE)
			DoReport();
		if(fgd.bCloseForecast == TRUE)
			PostMessage(WM_CLOSE, 0, 0);
			//OnCancel();
			//EndDialog(1);
	}
}

void CForecastDialog::OnAddday() 
{
	int days = m_wxGrid.GetNumberRows();//wxTable.GetRowSet().GetCount();
	/*if(days > 0)
	{
		CPVDTCurCell curCell = wxTable.GetCurCell();
		if(curCell)
			curCell.EndEdit();
	}*/
	//CTime start2;
	//startDateCtl.GetTime(start2);
	COleDateTime start;//(start2.GetYear(), start2.GetMonth(), start2.GetDay(), 14, 0, 0);
	startDateCtl.GetTime(start);
	COleDateTimeSpan span(days, 0, 0, 0);
	start += span;
	CUGCell cell;
//	CDTData rowData;
//	rowData.CreateArray(14);
//	rowData.PutItem(start, 0);
	if(days == 0)
		beginDay = start;
	int row = m_wxGrid.GetNumberRows();
	m_wxGrid.AppendRow();//InsertRow("");
	m_wxGrid.GetCell(0, row, &cell);
	cell.SetTime(0, row, 14, start.GetDay(), start.GetMonth(), start.GetYear());
	cell.SetText(start.Format(VAR_DATEVALUEONLY));
	m_wxGrid.SetCell(0, row, &cell);
	//m_wxGrid.SetCellType(row, 0, RUNTIME_CLASS(CGridCellDateTime));
	//CGridCellDateTime *pDateCell = (CGridCellDateTime *)m_wxGrid.GetCell(row, 0);
	//CTime ctime(start.GetYear(), start.GetMonth(), start.GetDay(), 14, 0, 0);
	//pDateCell->SetTime(ctime);
	//m_wxGrid.GetCell(1, row, &cell);
	//cell.SetNumber(
	/*m_wxGrid.SetCellType(row, 1, RUNTIME_CLASS(CGridCellNumeric));
	CGridCellNumeric *pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 1);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 2, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 2);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 3, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 3);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 4, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 4);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 5, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 5);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 6, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 6);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 7, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 7);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 8, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 8);
	pNumCell->SetFlags(CGridCellNumeric::Real);
	m_wxGrid.SetCellType(row, 9, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 9);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 10, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 10);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 11, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 11);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 12, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 12);
	pNumCell->SetFlags(CGridCellNumeric::Integer);
	m_wxGrid.SetCellType(row, 13, RUNTIME_CLASS(CGridCellNumeric));
	pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 13);
	pNumCell->SetFlags(CGridCellNumeric::Integer);*/
	switch(options.addType)
	{
	case Same:
		if(days > 0)
		{
			//CDTData prevData = wxTable.GetRowSet().GetItem(days - 1).GetValue();
			for(int i = 1; i < 14; i++)
			{
				//m_wxGrid.GetCell(row, i)->SetText(m_wxGrid.GetCell(row - 1, i)->GetText());
				CUGCell cell;
				m_wxGrid.GetCell(i, row - 1, &cell);
				m_wxGrid.QuickSetNumber(i, row, cell.GetNumber());
				/*if(i != 8)
				{
					long ival;
					prevData.GetItem(&ival, i);
					rowData.PutItem(ival, i);
				}
				else
				{
					double dval;
					prevData.GetItem(&dval, i);
					rowData.PutItem(dval, i);
				}*/

			}
		}
		else//use last day from analysis
		{
			int p = analysis->GetPeriod(lastDay);
			if(p > 0)
			{
				int yr = lastDay.GetYear() - analysis->periods[0][p]->baseYear;
				//rowData.PutItem(analysis->periods[0][p]->yStats[yr].Mean(), 1);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 1);
				m_wxGrid.QuickSetNumber(1, row, analysis->periods[0][p]->yStats[yr].Mean());
				//pNumCell->SetNumber(analysis->periods[0][p]->yStats[yr].Mean());
				//rowData.PutItem(analysis->periods[3][p]->yStats[yr].Mean(), 2);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 2);
				m_wxGrid.QuickSetNumber(2, row, analysis->periods[3][p]->yStats[yr].Mean());
				//pNumCell->SetNumber(analysis->periods[3][p]->yStats[yr].Mean());
				//rowData.PutItem(analysis->periods[2][p]->yStats[yr].Mean(), 3);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 3);
				//pNumCell->SetNumber(analysis->periods[2][p]->yStats[yr].Mean());
				m_wxGrid.QuickSetNumber(3, row, analysis->periods[2][p]->yStats[yr].Mean());
				//rowData.PutItem(analysis->periods[4][p]->yStats[yr].Mean(), 4);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 4);
				//pNumCell->SetNumber(analysis->periods[4][p]->yStats[yr].Mean());
				m_wxGrid.QuickSetNumber(4, row, analysis->periods[4][p]->yStats[yr].Mean());
				//rowData.PutItem(analysis->periods[7][p]->yStats[yr].Mean(), 5);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 5);
				//pNumCell->SetNumber(analysis->periods[7][p]->yStats[yr].Mean());
				m_wxGrid.QuickSetNumber(5, row, analysis->periods[7][p]->yStats[yr].Mean());
				//rowData.PutItem(analysis->periods[6][p]->yStats[yr].Mean(), 6);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 6);
				//pNumCell->SetNumber(analysis->periods[6][p]->yStats[yr].Mean());
				m_wxGrid.QuickSetNumber(6, row, analysis->periods[6][p]->yStats[yr].Mean());
				//rowData.PutItem(analysis->periods[10][p]->yStats[yr].Mean(), 7);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 7);
				//pNumCell->SetNumber(analysis->periods[10][p]->yStats[yr].Mean());
				m_wxGrid.QuickSetNumber(7, row, analysis->periods[10][p]->yStats[yr].Mean());
				//rowData.PutItem(analysis->periods[8][p]->yStats[yr].Mean(), 8);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 8);
				//pNumCell->SetNumber(analysis->periods[8][p]->yStats[yr].Mean());
				m_wxGrid.QuickSetNumber(8, row, analysis->periods[8][p]->yStats[yr].Mean());
				//rowData.PutItem(analysis->periods[9][p]->yStats[yr].Mean(), 9);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 9);
				//pNumCell->SetNumber(analysis->periods[9][p]->yStats[yr].Mean());
				m_wxGrid.QuickSetNumber(9, row, analysis->periods[9][p]->yStats[yr].Mean());
				//rowData.PutItem((long)1, 10);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 10);
				//pNumCell->SetNumber(1.0);
				m_wxGrid.QuickSetNumber(10, row, 1.0);
				//rowData.PutItem(0.0, 11);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 11);
				//pNumCell->SetNumber(0.0);
				m_wxGrid.QuickSetNumber(11, row, 0.0);
				//rowData.PutItem(0.0, 12);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 12);
				//pNumCell->SetNumber(0.0);
				m_wxGrid.QuickSetNumber(12, row, 0.0);
				//rowData.PutItem((long)3, 13);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 13);
				//pNumCell->SetNumber(3.0);
				m_wxGrid.QuickSetNumber(13, row, 3.0);
			}
		}
		break;
	case Historic:
	{
		//use historic selected percentiles from analysis
		int p = analysis->GetPeriod(start);
		if(p > 0)
		{
			//int yr = lastDay.GetYear() - analysis->periods[0][p]->baseYear;
			//rowData.PutItem((long)analysis->periods[0][p]->Percentile(options.tempPcnt), 1);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 1);
			//pNumCell->SetNumber((long)analysis->periods[0][p]->Percentile(options.tempPcnt));
			m_wxGrid.QuickSetNumber(1, row, (long)analysis->periods[0][p]->Percentile(options.tempPcnt));
			//rowData.PutItem((long)analysis->periods[3][p]->Percentile(options.tempPcnt), 2);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 2);
			//pNumCell->SetNumber((long)analysis->periods[3][p]->Percentile(options.tempPcnt));
			m_wxGrid.QuickSetNumber(2, row, (long)analysis->periods[3][p]->Percentile(options.tempPcnt));
			//rowData.PutItem((long)analysis->periods[2][p]->Percentile(options.tempPcnt), 3);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 3);
			//pNumCell->SetNumber((long)analysis->periods[2][p]->Percentile(options.tempPcnt));
			m_wxGrid.QuickSetNumber(3, row, (long)analysis->periods[2][p]->Percentile(options.tempPcnt));
			//rowData.PutItem((long)analysis->periods[4][p]->Percentile(options.rhPcnt), 4);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 4);
			//pNumCell->SetNumber((long)analysis->periods[4][p]->Percentile(options.rhPcnt));
			m_wxGrid.QuickSetNumber(4, row, (long)analysis->periods[4][p]->Percentile(options.rhPcnt));
			//rowData.PutItem((long)analysis->periods[7][p]->Percentile(options.rhPcnt), 5);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 5);
			//pNumCell->SetNumber((long)analysis->periods[7][p]->Percentile(options.rhPcnt));
			m_wxGrid.QuickSetNumber(5, row, (long)analysis->periods[7][p]->Percentile(options.rhPcnt));
			//rowData.PutItem((long)analysis->periods[6][p]->Percentile(options.rhPcnt), 6);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 6);
			//pNumCell->SetNumber((long)analysis->periods[6][p]->Percentile(options.rhPcnt));
			m_wxGrid.QuickSetNumber(6, row, (long)analysis->periods[6][p]->Percentile(options.rhPcnt));
			//rowData.PutItem((long)analysis->periods[10][p]->Percentile(options.windPcnt), 7);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 7);
			//pNumCell->SetNumber((long)analysis->periods[10][p]->Percentile(options.windPcnt));
			m_wxGrid.QuickSetNumber(7, row, (long)analysis->periods[10][p]->Percentile(options.windPcnt));
			//rowData.PutItem(0.0, 8);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 8);
			//pNumCell->SetNumber(0.0);
			m_wxGrid.QuickSetNumber(8, row, 0.0);
			//rowData.PutItem((long)0, 9);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 9);
			//pNumCell->SetNumber(0.0);
			m_wxGrid.QuickSetNumber(9, row, 0.0);
			//rowData.PutItem((long)1, 10);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 10);
			//pNumCell->SetNumber(1.0);
			m_wxGrid.QuickSetNumber(10, row, 1.0);
			//rowData.PutItem(0.0, 11);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 11);
			//pNumCell->SetNumber(0.0);
			m_wxGrid.QuickSetNumber(11, row, 0.0);
			//rowData.PutItem(0.0, 12);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 12);
			//pNumCell->SetNumber(0.0);
			m_wxGrid.QuickSetNumber(12, row, 0.0);
			//rowData.PutItem((long)3, 13);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(row, 13);
			//pNumCell->SetNumber(3.0);
			m_wxGrid.QuickSetNumber(13, row, 3.0);
		}
	}
		break;
	default://blank
		break;
	}
	//wxTable.GetRowSet().Add(dtAfterLast, rowData);
	//wxTable.Refresh();
	m_wxGrid.Invalidate();
}

void CForecastDialog::PostNcDestroy() 
{
//	if(
//	options.Write();
	delete analysis;
	((CFireplusView *)m_Parent)->forecastDialog = NULL;
	//CDialog::PostNcDestroy();
	delete this;
}

void CForecastDialog::OnOptions() 
{
	//CPVDTCurCell curCell = wxTable.GetCurCell();
	//if(curCell)
		//curCell.EndEdit();
	CForecastOptionsDialog fod(this, &options, fpSet->m_pDatabase);
	if(IDOK == fod.DoModal())
	{
		ConfigureOutputs();
		options.Write(fpSet->m_pDatabase);
	}
}

void CForecastDialog::ConfigureOutputs()
{
	//for(int c = 1; c < varTable.GetColumnSet().GetCount(); c++)
	int cWid = 10;
	CRect rect;
	m_wxGrid.GetClientRect(&rect);
	cWid = rect.Width() / 15;
	for(int c = 1; c < m_varGrid.GetNumberCols(); c++)
	{
		//varTable.GetColumnSet().GetItem(c).SetHidden(TRUE);
		m_varGrid.SetColWidth(c, 0);//SetColumnWidth(c, 0);//hide the column
		int seek = VTColToVarID(c);//atoi(varTable.GetColumnSet().GetItem(c).GetName());
		POSITION pos = options.outputsList.GetHeadPosition();
		while(pos)
		{
			int id = options.outputsList.GetNext(pos);
			if(id == seek)
			{
				//varTable.GetColumnSet().GetItem(c).SetHidden(FALSE);
				m_varGrid.SetColWidth(c, cWid);//show the column
				break;
			}
		}
	}
}

ForecastOptions::ForecastOptions()
{
	addType = Same;
	tempPcnt = rhPcnt = windPcnt = 0.50;
	lineColor = RGB(0,0,0);
	lineWidth = 3;
	lineStyle = 0;
}

void ForecastOptions::Copy(ForecastOptions *options)
{
	addType = options->addType;
	tempPcnt = options->tempPcnt;
	rhPcnt = options->rhPcnt;
	windPcnt = options->windPcnt;
	lineColor = options->lineColor;
	lineWidth = options->lineWidth;
	lineStyle = options->lineStyle;
	outputsList.RemoveAll();
	POSITION pos = options->outputsList.GetHeadPosition();
	while(pos)
	{
		int val = options->outputsList.GetNext(pos);
		outputsList.AddTail(val);
	}

}

ForecastOptions::~ForecastOptions()
{
	outputsList.RemoveAll();
}

int ForecastOptions::Read(CDatabase *pDB)
{
	CFFPfxOptsSet optsSet(pDB);
	optsSet.Open();
	if(!optsSet.IsEOF())
	{
		lineColor = optsSet.m_LineColor;
		lineWidth = optsSet.m_LineWidth;
		lineStyle = optsSet.m_LineStyle;
		addType = (AddType) optsSet.m_AddType;
		rhPcnt = optsSet.m_RHPct;
		tempPcnt = optsSet.m_TempPct;
		windPcnt = optsSet.m_WindPct;
	}
	optsSet.Close();
	CFFPfxVarsSet varsSet(pDB);
	varsSet.Open();
	while(!varsSet.IsEOF())
	{
		outputsList.AddTail(varsSet.m_VarID);
		varsSet.MoveNext();
	}
	varsSet.Close();
	/*char buf[32];
	CString fName;
	fName.Format("%s\\fxopts.dat", theApp.workDir);
	FILE *stream = fopen(fName, "rt");
	if(!stream)
		return -1;
	fgets(buf, 16, stream);
	lineColor = atol(buf);
	fgets(buf, 16, stream);
	lineWidth = atoi(buf);
	fgets(buf, 16, stream);
	lineStyle = atoi(buf);
	fgets(buf, 16, stream);
	addType = (AddType)atoi(buf);
	fgets(buf, 16, stream);
	rhPcnt = atof(buf);
	fgets(buf, 16, stream);
	tempPcnt = atof(buf);
	fgets(buf, 16, stream);
	windPcnt = atof(buf);
	fgets(buf, 16, stream);
	while(!feof(stream))
	{
		outputsList.AddTail(atoi(buf));
		fgets(buf, 16, stream);
	}
	fclose(stream);*/
	return 0;
}

void ForecastOptions::Write(CDatabase *pDB)
{
	CFFPfxOptsSet optsSet(pDB);
	optsSet.Open();
	while(!optsSet.IsEOF())
	{
		optsSet.Delete();
		optsSet.MoveNext();
	}
	optsSet.AddNew();
	optsSet.m_AddType = addType;
	optsSet.m_TempPct = tempPcnt;
	optsSet.m_RHPct = rhPcnt;
	optsSet.m_WindPct = windPcnt;
	optsSet.m_LineColor = lineColor;
	optsSet.m_LineStyle = lineStyle;
	optsSet.m_LineWidth = lineWidth;
	optsSet.Update();
	optsSet.Close();

	CFFPfxVarsSet varsSet(pDB);
	varsSet.Open();
	while(!varsSet.IsEOF())
	{
		varsSet.Delete();
		varsSet.MoveNext();
	}

	/*CString fName;
	fName.Format("%s\\fxopts.dat", theApp.workDir);
	FILE *stream = fopen(fName, "wt");
	fprintf(stream, "%ld\n", lineColor);
	fprintf(stream, "%d\n", lineWidth);
	fprintf(stream, "%d\n", lineStyle);
	fprintf(stream, "%d\n", addType);
	fprintf(stream, "%.2f\n", rhPcnt);
	fprintf(stream, "%.2f\n", tempPcnt);
	fprintf(stream, "%.2f\n", windPcnt);*/

	//list of selected varIDs last
	POSITION pos = outputsList.GetHeadPosition();
	while(pos)
	{
		varsSet.AddNew();
		varsSet.m_VarID = outputsList.GetNext(pos);
		//fprintf(stream, "%d\n", outputsList.GetNext(pos));
		varsSet.Update();
	}
	varsSet.Close();
	//fclose(stream);
}


void CForecastDialog::OnCalculate() 
{
	CWaitCursor wait;
//	CPVDTCurCell curCell = wxTable.GetCurCell();
//	if(curCell)
//		curCell.EndEdit();
//	m_NFDRS.LoadState(&lastState);
	canadian.LoadState(&canState);
	//varTable.GetRowSet().Reset();
	m_varGrid.SetNumberRows(0, FALSE);
	CSIGStationSet staSet(fpSet->m_pDatabase);
//	CGridCellDateTime *pDateCell;
//	CGridCellNumeric *pNumCell;
	if(strncmp(fpSet->m_SIG_Station, "SIG", 3) == 0)//SIG
	{
		CString sfName;
		int nDays = m_wxGrid.GetNumberRows();//wxTable.GetRowSet().GetCount();
		double *sig_iKBDI, *sig_iBI, *sig_iSC, *sig_iIC, *sig_f1, *sig_f10, *sig_f100, 
			*sig_f1000, *sig_fHerb, *sig_fWood, *sig_fX1000, *sig_ffmc, *sig_dmc, *sig_dc, 
			*sig_bui, *sig_isi, *sig_fwi, *sig_dsr, *sig_fERC, weightSum = 0.0;
		sig_iKBDI = new double[nDays];
		sig_iBI = new double[nDays];
		sig_iSC = new double[nDays];
		sig_iIC = new double[nDays];
		sig_f1 = new double[nDays];
		sig_f10 = new double[nDays];
		sig_f100 = new double[nDays];
		sig_f1000 = new double[nDays];
		sig_fHerb = new double[nDays];
		sig_fWood = new double[nDays];
		sig_fX1000 = new double[nDays];
		sig_ffmc = new double[nDays];
		sig_dmc = new double[nDays];
		sig_dc = new double[nDays];
		sig_bui = new double[nDays];
		sig_isi = new double[nDays];
		sig_fwi = new double[nDays];
		sig_dsr = new double[nDays];
		sig_fERC = new double[nDays];
		memset(sig_iKBDI, 0, nDays * sizeof(double));
		memset(sig_iBI, 0, nDays * sizeof(double));
		memset(sig_iSC, 0, nDays * sizeof(double));
		memset(sig_iIC, 0, nDays * sizeof(double));
		memset(sig_f1, 0, nDays * sizeof(double));
		memset(sig_f10, 0, nDays * sizeof(double));
		memset(sig_f100, 0, nDays * sizeof(double));
		memset(sig_f1000, 0, nDays * sizeof(double));
		memset(sig_fHerb, 0, nDays * sizeof(double));
		memset(sig_fWood, 0, nDays * sizeof(double));
		memset(sig_fX1000, 0, nDays * sizeof(double));
		memset(sig_ffmc, 0, nDays * sizeof(double));
		memset(sig_dmc, 0, nDays * sizeof(double));
		memset(sig_dc, 0, nDays * sizeof(double));
		memset(sig_bui, 0, nDays * sizeof(double));
		memset(sig_isi, 0, nDays * sizeof(double));
		memset(sig_fwi, 0, nDays * sizeof(double));
		memset(sig_dsr, 0, nDays * sizeof(double));
		memset(sig_fERC, 0, nDays * sizeof(double));
		char temp[64], sig[128];
		strcpy_s(sig, fpSet->m_SIG_Station);
		CStationInSIGSet sSet(fpSet->m_pDatabase);
		sprintf(temp, "[SIG] = '%-20.20s'", &sig[6]);
		sSet.m_strFilter = _T(temp);
		sSet.Open();
		staSet.Open();
		while(!sSet.IsEOF())
		{
			sfName.Format("%s\\%6.6s.nfd", theApp.dbDir, sSet.m_StationID);
			m_NFDRS.LoadState(sfName);
			canadian.LoadState(&canState);
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			staSet.Requery();
			weightSum += sSet.m_WeightFactor;
			//for(long i = 0; i < wxTable.GetRowSet().GetCount(); i++)
			for(long i = 0; i < m_wxGrid.GetNumberRows(); i++)
			{

				//CDTData wxData = wxTable.GetRowSet().GetItem(i).GetValue();
				long t, tMin, tMax, r, rMin, rMax, pDur, w, sow, wg, hg, seas;
				int iStage, iRainEvent, iKBDI, iFIL, iBI, iSC, iIC;
				double pcp, f1, f10, f100, f1000, fHerb, fWood, fGren, fX1000, ffmc, dmc, 
					dc, bui, isi, fwi, dsr, fROS, fERC, fFL;
				int second, minute, hour, day, month, year;
				CUGCell cell;
				//CTime *day;
				//wxData.GetItem(day, 0);
				//pDateCell = (CGridCellDateTime *)m_wxGrid.GetCell(i + 1, 0);
				m_wxGrid.GetCell(0, i, &cell);
				cell.GetTime(&second, &minute, &hour, &day, &month, &year);
				//day = pDateCell->GetTime();
				COleDateTime oleDay(year, month, day, 14, 0, 0);
				//wxData.GetItem(&t, 1);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 1);
				m_wxGrid.GetCell(1, i, &cell);
				t = cell.GetNumber();//pNumCell->GetNumber();
				//wxData.GetItem(&tMin, 3);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 3);
				m_wxGrid.GetCell(3, i, &cell);
				tMin = cell.GetNumber();//pNumCell->GetNumber();
				//wxData.GetItem(&tMax, 2);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 2);
				m_wxGrid.GetCell(2, i, &cell);
				tMax = cell.GetNumber();//pNumCell->GetNumber();
				//wxData.GetItem(&r, 4);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 4);
				m_wxGrid.GetCell(4, i, &cell);
				r = cell.GetNumber();
				//wxData.GetItem(&rMin, 6);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 6);
				m_wxGrid.GetCell(6, i, &cell);
				rMin = cell.GetNumber();
				//wxData.GetItem(&rMax, 5);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 5);
				m_wxGrid.GetCell(5, i, &cell);
				rMax = cell.GetNumber();
				//wxData.GetItem(&w, 7);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 7);
				m_wxGrid.GetCell(7, i, &cell);
				w = cell.GetNumber();
				//wxData.GetItem(&pcp, 8);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 8);
				m_wxGrid.GetCell(8, i, &cell);
				pcp = cell.GetNumber();
				//wxData.GetItem(&pDur, 9);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 9);
				m_wxGrid.GetCell(9, i, &cell);
				pDur = cell.GetNumber();
				//wxData.GetItem(&sow, 10);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 10);
				m_wxGrid.GetCell(10, i, &cell);
				sow = cell.GetNumber();
				//wxData.GetItem(&hg, 11);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 11);
				m_wxGrid.GetCell(11, i, &cell);
				hg = cell.GetNumber();
				//wxData.GetItem(&wg, 12);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 12);
				m_wxGrid.GetCell(12, i, &cell);
				wg = cell.GetNumber();
				//wxData.GetItem(&seas, 13);
				//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 13);
				m_wxGrid.GetCell(13, i, &cell);
				seas = cell.GetNumber();
				m_NFDRS.iCalcMoist(0, (short)t, (short)r, (short)tMax, (short)rMax, (short)tMin, (short)rMin, (short)pDur, pcp, (short)sow, -1000.0, 
					DayOfYear(oleDay), oleDay.GetYear(), 0, 0, (short)hg, (short)wg, (short)seas,
					&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI); 
				m_NFDRS.iCalcIndexes((short)w, staSet.m_SlopeCls, &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
				m_NFDRS.iCalcIC((short)t, (short)sow, f1, iSC, &iIC);
				canadian.CalcIndices(DayOfYear(oleDay), oleDay.GetMonth(), oleDay.GetYear(),(double)t, (double)r, (double)w * 1.15, pcp,
					&ffmc, &dmc, &dc, &bui, &isi, &fwi, &dsr);
				sig_iKBDI[i] += sSet.m_WeightFactor * iKBDI;
				sig_iBI[i] += sSet.m_WeightFactor * iBI;
				sig_iSC[i] += sSet.m_WeightFactor * iSC;
				sig_iIC[i] += sSet.m_WeightFactor * iIC;
				sig_f1[i] += sSet.m_WeightFactor * f1;
				sig_f10[i] += sSet.m_WeightFactor * f10;
				sig_f100[i] += sSet.m_WeightFactor * f100;
				sig_f1000[i] += sSet.m_WeightFactor * f1000;
				sig_fHerb[i] += sSet.m_WeightFactor * fHerb;
				sig_fWood[i] += sSet.m_WeightFactor * fWood;
				sig_fX1000[i] += sSet.m_WeightFactor * fX1000;
				sig_ffmc[i] += sSet.m_WeightFactor * ffmc;
				sig_dmc[i] += sSet.m_WeightFactor * dmc;
				sig_dc[i] += sSet.m_WeightFactor * dc;
				sig_bui[i] += sSet.m_WeightFactor * bui;
				sig_isi[i] += sSet.m_WeightFactor * isi;
				sig_fwi[i] += sSet.m_WeightFactor * fwi;
				sig_dsr[i] += sSet.m_WeightFactor * dsr;
				sig_fERC[i] += sSet.m_WeightFactor * fERC;

			}
			sSet.MoveNext();
		}
		sSet.Close();
		//for(long i = 0; i < wxTable.GetRowSet().GetCount(); i++)
		for(long i = 0; i < m_wxGrid.GetNumberRows(); i++)
		{
			int second, minute, hour, cDay, month, year;
			CUGCell cell;
			//CDTData wxData = wxTable.GetRowSet().GetItem(i).GetValue();
			m_wxGrid.GetCell(0, i, &cell);
			cell.GetTime(&second, &minute, &hour, &cDay, &month, &year);
			//pDateCell = (CGridCellDateTime *)m_wxGrid.GetCell(i + 1, 0);
			//CTime *ctime = pDateCell->GetTime();
			//COleDateTime day(ctime->GetYear(), ctime->GetMonth(), ctime->GetDay(), 14, 0, 0);
			COleDateTime day(year, month, cDay, 14, 0, 0);
			//wxData.GetItem(day, 0);
			sig_iKBDI[i] /= weightSum;
			sig_iBI[i] /= weightSum;
			sig_iSC[i] /= weightSum;
			sig_iIC[i] /= weightSum;
			sig_f1[i] /= weightSum;
			sig_f10[i] /= weightSum;
			sig_f100[i] /= weightSum;
			sig_f1000[i] /= weightSum;
			sig_fHerb[i] /= weightSum;
			sig_fWood[i] /= weightSum;
			sig_fX1000[i] /= weightSum;
			sig_ffmc[i] /= weightSum;
			sig_dmc[i] /= weightSum;
			sig_dc[i] /= weightSum;
			sig_bui[i] /= weightSum;
			sig_isi[i] /= weightSum;
			sig_fwi[i] /= weightSum;
			sig_dsr[i] /= weightSum;
			sig_fERC[i] /= weightSum;
			//CDTData rowData;
			//rowData.CreateArray(20);
			int row = m_varGrid.GetNumberRows();
			m_varGrid.AppendRow();//.InsertRow("");
			m_varGrid.GetCell(0, i, &cell);
			cell.SetTime(second, minute, hour, cDay, month, year);
			m_varGrid.SetCell(0, i, &cell);
			//rowData.PutItem(day, 0);		
			//m_varGrid.SetCellType(row, 0, RUNTIME_CLASS(CGridCellDateTime));
			//pDateCell = (CGridCellDateTime *)m_varGrid.GetCell(row, 0);
			//pDateCell->SetTime(*ctime);
			//rowData.PutItem((long)sig_iSC[i], 1);
			//m_varGrid.SetCellType(row, 1, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 1);
			//pNumCell->SetNumber(sig_iSC[i]);
			m_varGrid.GetCell(1, i, &cell);
			cell.SetNumber(sig_iSC[i]);
			m_varGrid.SetCell(1, i, &cell);
			//rowData.PutItem(Round(sig_fERC[i]), 2);
			//m_varGrid.SetCellType(row, 2, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 2);
			//pNumCell->SetNumber(sig_iSC[i]);
			m_varGrid.GetCell(2, i, &cell);
			cell.SetNumber(sig_fERC[i]);
			m_varGrid.SetCell(2, i, &cell);
			//rowData.PutItem((long)sig_fERC[i], 3);
			//m_varGrid.SetCellType(row, 3, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 3);
			//pNumCell->SetNumber(sig_iSC[i]);
			m_varGrid.GetCell(3, i, &cell);
			cell.SetNumber(sig_iBI[i]);
			m_varGrid.SetCell(3, i, &cell);
			//rowData.PutItem((long)sig_iKBDI[i], 4);
			//m_varGrid.SetCellType(row, 4, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 4);
			//pNumCell->SetNumber(sig_iKBDI[i]);
			m_varGrid.GetCell(4, i, &cell);
			cell.SetNumber(sig_iKBDI[i]);
			m_varGrid.SetCell(4, i, &cell);
			//rowData.PutItem((long)sig_iIC[i], 5);
			//m_varGrid.SetCellType(row, 5, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 5);
			//pNumCell->SetNumber(sig_iIC[i]);
			m_varGrid.GetCell(5, i, &cell);
			cell.SetNumber(sig_iIC[i]);
			m_varGrid.SetCell(5, i, &cell);
			//rowData.PutItem(sig_f1[i], 6);
			//m_varGrid.SetCellType(row, 6, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 6);
			//pNumCell->SetNumber(sig_f1[i]);
			m_varGrid.GetCell(6, i, &cell);
			cell.SetNumber(sig_f1[i]);
			m_varGrid.SetCell(6, i, &cell);
			//rowData.PutItem(sig_f10[i], 7);
			//m_varGrid.SetCellType(row, 7, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 7);
			//pNumCell->SetNumber(sig_f10[i]);
			m_varGrid.GetCell(7, i, &cell);
			cell.SetNumber(sig_f10[i]);
			m_varGrid.SetCell(7, i, &cell);
			//rowData.PutItem(sig_f100[i], 8);
			//m_varGrid.SetCellType(row, 8, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 8);
			//pNumCell->SetNumber(sig_f100[i]);
			m_varGrid.GetCell(8, i, &cell);
			cell.SetNumber(sig_f100[i]);
			m_varGrid.SetCell(8, i, &cell);
			//rowData.PutItem(sig_f1000[i], 9);
			//m_varGrid.SetCellType(row, 9, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 9);
			//pNumCell->SetNumber(sig_f1000[i]);
			m_varGrid.GetCell(9, i, &cell);
			cell.SetNumber(sig_f1000[i]);
			m_varGrid.SetCell(9, i, &cell);
			//rowData.PutItem(sig_fHerb[i], 10);
			//m_varGrid.SetCellType(row, 10, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 10);
			//pNumCell->SetNumber(sig_fHerb[i]);
			m_varGrid.GetCell(10, i, &cell);
			cell.SetNumber(sig_fHerb[i]);
			m_varGrid.SetCell(10, i, &cell);
			//rowData.PutItem(sig_fWood[i], 11);
			//m_varGrid.SetCellType(row, 11, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 11);
			//pNumCell->SetNumber(sig_fWood[i]);
			m_varGrid.GetCell(11, i, &cell);
			cell.SetNumber(sig_fWood[i]);
			m_varGrid.SetCell(11, i, &cell);
			//rowData.PutItem(sig_fX1000[i], 12);
			//m_varGrid.SetCellType(row, 12, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 12);
			//pNumCell->SetNumber(sig_fX1000[i]);
			m_varGrid.GetCell(12, i, &cell);
			cell.SetNumber(sig_fX1000[i]);
			m_varGrid.SetCell(12, i, &cell);
			//rowData.PutItem(sig_ffmc[i], 13);
			//m_varGrid.SetCellType(row, 13, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 13);
			//pNumCell->SetNumber(sig_ffmc[i]);
			m_varGrid.GetCell(13, i, &cell);
			cell.SetNumber(sig_ffmc[i]);
			m_varGrid.SetCell(13, i, &cell);
			//rowData.PutItem(sig_dmc[i], 14);
			//m_varGrid.SetCellType(row, 14, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 14);
			//pNumCell->SetNumber(sig_dmc[i]);
			m_varGrid.GetCell(14, i, &cell);
			cell.SetNumber(sig_dmc[i]);
			m_varGrid.SetCell(14, i, &cell);
			//rowData.PutItem(sig_dc[i], 15);
			//m_varGrid.SetCellType(row, 15, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 15);
			//pNumCell->SetNumber(sig_dc[i]);
			m_varGrid.GetCell(15, i, &cell);
			cell.SetNumber(sig_dc[i]);
			m_varGrid.SetCell(15, i, &cell);
			//rowData.PutItem(sig_isi[i], 16);
			//m_varGrid.SetCellType(row, 16, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 16);
			//pNumCell->SetNumber(sig_isi[i]);
			m_varGrid.GetCell(16, i, &cell);
			cell.SetNumber(sig_isi[i]);
			m_varGrid.SetCell(16, i, &cell);
			//rowData.PutItem(sig_bui[i], 17);
			//m_varGrid.SetCellType(row, 17, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 17);
			//pNumCell->SetNumber(sig_bui[i]);
			m_varGrid.GetCell(17, i, &cell);
			cell.SetNumber(sig_bui[i]);
			m_varGrid.SetCell(17, i, &cell);
			//rowData.PutItem(sig_fwi[i], 18);
			//m_varGrid.SetCellType(row, 18, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 18);
			//pNumCell->SetNumber(sig_fwi[i]);
			m_varGrid.GetCell(18, i, &cell);
			cell.SetNumber(sig_fwi[i]);
			m_varGrid.SetCell(18, i, &cell);
			//rowData.PutItem(sig_dsr[i], 19);
			//m_varGrid.SetCellType(row, 19, RUNTIME_CLASS(CGridCellNumeric));
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 19);
			//pNumCell->SetNumber(sig_dsr[i]);
			m_varGrid.GetCell(19, i, &cell);
			cell.SetNumber(sig_dsr[i]);
			m_varGrid.SetCell(19, i, &cell);
			//varTable.GetRowSet().Add(dtAfterLast, rowData);
		}
		delete[] sig_iKBDI;
		delete[] sig_iBI;
		delete[] sig_iSC;
		delete[] sig_iIC;
		delete[] sig_f1;
		delete[] sig_f10;
		delete[] sig_f100;
		delete[] sig_f1000;
		delete[] sig_fHerb;
		delete[] sig_fWood;
		delete[] sig_fX1000;
		delete[] sig_ffmc;
		delete[] sig_dmc;
		delete[] sig_dc;
		delete[] sig_bui;
		delete[] sig_isi;
		delete[] sig_fwi;
		delete[] sig_dsr;
		delete[] sig_fERC;
	}
	else
	{
		CString sfName;
		sfName.Format("%s\\%6.6s.nfd", theApp.dbDir, fpSet->m_SIG_Station);
		m_NFDRS.LoadState(sfName);
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		staSet.Open();//have station or representative

		//for(long i = 0; i < wxTable.GetRowSet().GetCount(); i++)
		for(long i = 0; i < m_wxGrid.GetNumberRows(); i++)
		{
			int second, minute, hour, cDay, month, year;
			CUGCell cell;
			//CDTData wxData = wxTable.GetRowSet().GetItem(i).GetValue();
			long t, tMin, tMax, r, rMin, rMax, pDur, w, sow, wg, hg, seas;
			int iStage, iRainEvent, iKBDI, iFIL, iBI, iSC, iIC;
			double pcp, f1, f10, f100, f1000, fHerb, fWood, fGren, fX1000, ffmc, dmc, dc, bui, isi, fwi, dsr,
				fROS, fERC, fFL;
			//pDateCell = (CGridCellDateTime *)m_wxGrid.GetCell(i + 1, 0);
			//CTime *ctime = pDateCell->GetTime();
			//COleDateTime day(ctime->GetYear(), ctime->GetMonth(), ctime->GetDay(), 14, 0, 0);
			m_wxGrid.GetCell(0, i, &cell);
			cell.GetTime(&second, &minute, &hour, &cDay, &month, &year);
			COleDateTime day(year, month, cDay, 14, 0, 0);
			/*wxData.GetItem(day, 0);
			wxData.GetItem(&t, 1);
			wxData.GetItem(&tMin, 3);
			wxData.GetItem(&tMax, 2);
			wxData.GetItem(&r, 4);
			wxData.GetItem(&rMin, 6);
			wxData.GetItem(&rMax, 5);
			wxData.GetItem(&w, 7);
			wxData.GetItem(&pcp, 8);
			wxData.GetItem(&pDur, 9);
			wxData.GetItem(&sow, 10);
			wxData.GetItem(&hg, 11);
			wxData.GetItem(&wg, 12);
			wxData.GetItem(&seas, 13);*/
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 1);
			m_wxGrid.GetCell(1, i, &cell);
			t = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&tMin, 3);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 3);
			m_wxGrid.GetCell(3, i, &cell);
			tMin = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&tMax, 2);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 2);
			m_wxGrid.GetCell(2, i, &cell);
			tMax = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&r, 4);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 4);
			m_wxGrid.GetCell(4, i, &cell);
			r = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&rMin, 6);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 6);
			m_wxGrid.GetCell(6, i, &cell);
			rMin = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&rMax, 5);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 5);
			m_wxGrid.GetCell(5, i, &cell);
			rMax = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&w, 7);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 7);
			m_wxGrid.GetCell(7, i, &cell);
			w = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&pcp, 8);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 8);
			m_wxGrid.GetCell(8, i, &cell);
			pcp = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&pDur, 9);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 9);
			m_wxGrid.GetCell(9, i, &cell);
			pDur = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&sow, 10);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 10);
			m_wxGrid.GetCell(10, i, &cell);
			sow = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&hg, 11);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 11);
			m_wxGrid.GetCell(11, i, &cell);
			hg = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&wg, 12);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 12);
			m_wxGrid.GetCell(12, i, &cell);
			wg = cell.GetNumber();//pNumCell->GetNumber();
			//wxData.GetItem(&seas, 13);
			//pNumCell = (CGridCellNumeric *)m_wxGrid.GetCell(i + 1, 13);
			m_wxGrid.GetCell(13, i, &cell);
			seas = cell.GetNumber();//pNumCell->GetNumber();
			m_NFDRS.iCalcMoist(0, (short)t, (short)r, (short)tMax, (short)rMax, (short)tMin, (short)rMin, (short)pDur, pcp, (short)sow, -1000.0, 
				DayOfYear(day), day.GetYear(), 0, 0, (short)hg, (short)wg, (short)seas,
				&f1, &f10, &f100, &f1000, &fHerb, &fWood, &iStage, &fGren, &fX1000, &iRainEvent, &iKBDI); 
			m_NFDRS.iCalcIndexes((short)w, staSet.m_SlopeCls, &fROS, &iSC, &fERC, &fFL, &iFIL, &iBI);
			m_NFDRS.iCalcIC((short)t, (short)sow, f1, iSC, &iIC);
			canadian.CalcIndices(DayOfYear(day), day.GetMonth(), day.GetYear(),(double)t, (double)r, (double)w * 1.15, pcp,
				&ffmc, &dmc, &dc, &bui, &isi, &fwi, &dsr);
			int row = m_varGrid.GetNumberRows();//
			m_varGrid.AppendRow();//.InsertRow("");
			//CDTData rowData;
			//rowData.CreateArray(20);
			//rowData.PutItem(day, 0);
			//pDateCell = (CGridCellDateTime *)m_varGrid.GetCell(row, 0);
			//pDateCell->SetTime(*ctime);
			m_varGrid.GetCell(0, row, &cell);
			cell.SetTime(second, minute, hour, cDay, month, year);
			m_varGrid.SetCell(0, row, &cell);
			//rowData.PutItem((long)iSC, 1);
			m_varGrid.GetCell(1, row, &cell);
			cell.SetNumber(iSC);
			m_varGrid.SetCell(1, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 1);
			//iSC = pNumCell->GetNumber();
			//rowData.PutItem(Round(fERC), 2);
			m_varGrid.GetCell(2, row, &cell);
			cell.SetNumber(fERC);
			m_varGrid.SetCell(2, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 2);
			//fERC = pNumCell->GetNumber();
			//rowData.PutItem((long)iBI, 3);
			m_varGrid.GetCell(3, row, &cell);
			cell.SetNumber(iBI);
			m_varGrid.SetCell(3, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 3);
			//iBI = pNumCell->GetNumber();
			//rowData.PutItem((long)iKBDI, 4);
			m_varGrid.GetCell(4, row, &cell);
			cell.SetNumber(iKBDI);
			m_varGrid.SetCell(4, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 4);
			//iKBDI = pNumCell->GetNumber();
			//rowData.PutItem((long)iIC, 5);
			m_varGrid.GetCell(5, row, &cell);
			cell.SetNumber(iIC);
			m_varGrid.SetCell(5, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 5);
			//iIC = pNumCell->GetNumber();
			//rowData.PutItem(f1, 6);
			m_varGrid.GetCell(6, row, &cell);
			cell.SetNumber(f1);
			m_varGrid.SetCell(6, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 6);
			//f1 = pNumCell->GetNumber();
			//rowData.PutItem(f10, 7);
			m_varGrid.GetCell(7, row, &cell);
			cell.SetNumber(f10);
			m_varGrid.SetCell(7, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 7);
			//f10 = pNumCell->GetNumber();
			//rowData.PutItem(f100, 8);
			m_varGrid.GetCell(8, row, &cell);
			cell.SetNumber(f100);
			m_varGrid.SetCell(8, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 8);
			//f100 = pNumCell->GetNumber();
			//rowData.PutItem(f1000, 9);
			m_varGrid.GetCell(9, row, &cell);
			cell.SetNumber(f1000);
			m_varGrid.SetCell(9, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 9);
			//f1000 = pNumCell->GetNumber();
			//rowData.PutItem(fHerb, 10);
			m_varGrid.GetCell(10, row, &cell);
			cell.SetNumber(fHerb);
			m_varGrid.SetCell(10, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 10);
			//fHerb = pNumCell->GetNumber();
			///rowData.PutItem(fWood, 11);
			m_varGrid.GetCell(11, row, &cell);
			cell.SetNumber(fWood);
			m_varGrid.SetCell(11, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 11);
			//fWood = pNumCell->GetNumber();
			//rowData.PutItem(fX1000, 12);
			m_varGrid.GetCell(12, row, &cell);
			cell.SetNumber(fX1000);
			m_varGrid.SetCell(12, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 12);
			//fX1000 = pNumCell->GetNumber();
			//rowData.PutItem(ffmc, 13);
			m_varGrid.GetCell(13, row, &cell);
			cell.SetNumber(ffmc);
			m_varGrid.SetCell(13, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 13);
			//ffmc = pNumCell->GetNumber();
			//rowData.PutItem(dmc, 14);
			m_varGrid.GetCell(14, row, &cell);
			cell.SetNumber(dmc);
			m_varGrid.SetCell(14, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 14);
			//dmc = pNumCell->GetNumber();
			//rowData.PutItem(dc, 15);
			m_varGrid.GetCell(15, row, &cell);
			cell.SetNumber(dc);
			m_varGrid.SetCell(15, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 15);
			//dc = pNumCell->GetNumber();
			//rowData.PutItem(isi, 16);
			m_varGrid.GetCell(16, row, &cell);
			cell.SetNumber(isi);
			m_varGrid.SetCell(16, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 16);
			//isi = pNumCell->GetNumber();
			//rowData.PutItem(bui, 17);
			m_varGrid.GetCell(17, row, &cell);
			cell.SetNumber(bui);
			m_varGrid.SetCell(17, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 17);
			//bui = pNumCell->GetNumber();
			//rowData.PutItem(fwi, 18);
			m_varGrid.GetCell(18, row, &cell);
			cell.SetNumber(fwi);
			m_varGrid.SetCell(18, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 18);
			//fwi = pNumCell->GetNumber();
			//rowData.PutItem(dsr, 19);
			m_varGrid.GetCell(19, row, &cell);
			cell.SetNumber(dsr);
			m_varGrid.SetCell(19, row, &cell);
			//pNumCell = (CGridCellNumeric *)m_varGrid.GetCell(row, 19);
			//dsr = pNumCell->GetNumber();
			//varTable.GetRowSet().Add(dtAfterLast, rowData);
		}
	}
	staSet.Close();
	//varTable.Refresh();
	m_varGrid.RedrawAll();
	//if(varTable.GetRowSet().GetCount() > 0)
	if(m_varGrid.GetNumberRows() > 0)
		graphButton.EnableWindow(TRUE);
}



void CForecastDialog::OnCancel() 
{
	CString find, del;
	find.Format("%s\\*.nfd", theApp.dbDir);
	CFileFind ff;
	BOOL bWorking = ff.FindFile(find);
	while(bWorking)
	{
		bWorking = ff.FindNextFile();
		del = ff.GetFilePath();
		remove(del);
	}

	DestroyWindow();
}

void CForecastDialog::Remove()
{
	OnCancel();
}

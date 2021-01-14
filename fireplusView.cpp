// fireplusView.cpp : implementation of the CFireplusView class
//

#include "stdafx.h"
#include "fireplus.h"
//#include "pvnumeric.h"
 
#include "fireplusSet.h"
#include "fireplusDoc.h"
#include "fireplusView.h"
#include "climateSet.h"
#include "wxSet.h"
#include "SIGStationSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "FireSumSet.h"
#include "ClimAnalysis.h"
//#include "climdialog.h"
#include "climdoc.h"
#include "climateview.h"
//#include "DTOCX.H"
//#include "pvdtrowset1.H"
//#include "pvdtcolumnset1.H"
//#include "pvdtcolumn1.H"
//#include "pvdtrow1.H"
//#include "pvdtcurcell1.H"
#include "seasonbinset.H"
#include "PAnalysisDialog.h"
#include "SeverityDialog.h"
#include "PProbDialog.h"
#include "sigset.h"
#include "sigsdialog.h"
#include "stationInSIGSet.h"
//#include "StationsUsed.h"
#include "PocketCardSet.h"
#include "PocketCardDialog.h"
#include "ListingDialog.h"
#include "Stationdialog.h"
#include "FireAssocSet.h"
#include "FireAgencySet.h"
#include "FireRegionSet.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"
#include "FireQueryPage.h"
#include "YearsPage.h"
#include "FireQuerySheet.h"
#include "FireSet.h"
#include "ImportDialog.h"
#include "FireSummary.h"
#include "FireSumDoc.h"
#include "FireSumView.h"
#include "FireSumWnd.h"
#include "FireAnalysisDialog.h"
#include "AgencyDialog.h"
#include "rerapVarDialog.h"
#include "rerapDialog.h"
#include <afxrich.h>
#include "richdoc.h"
#include "MainFrm.h"
#include "EventLocatorDialog.h"
#include "ForecastDialog.h"
#include <process.h>
#include "DiurnalDialog.h"
#include "FARSITEDialog.h"
#include "UserBrowseDialog.h"
//#include "GridCellCombo.h"
//#include "GridCellCheck.h"
//#include "GridCellNumeric.h"
//#include "GridCellDateTime.h"
//#include "Periodic.h"
#include "ReportOptionsSet.h"
#include "RunSave.h"
// added for batch2
#include "RunRestoreDialog.h"
#include "BatchCreateDialog.h"
#include "BatchManageDialog.h"
#include "BatchOptionsSet.h"
#include "BatchSavedRunMaintDialog.h"
#include "ODBCRecordset.h"
#include "WindRoseDialog.h"
#include "QuickBatchDialog.h"
#include "PocketCardDoc.h"
#include "PocketCardFrame.h"
#include "PocketCardView.h"
#include "NelsonDFM.h"
#include "FriskExportDialog.h"
#include "WxStationSet.h"
#include "CustomInitsDialog.h"
#include "HourlyEventLocatorDlg.h"
#include "FFPOptsSet.h"
//#include "LFIDialog.h"
//#include "CandidatesDlg.h"
#include "TermsDialog.h"
#include "WeatherDoc.h"
#include "WeatherView.h"
#include "WeatherFrame.h"
#include "CandidatesDoc.h"
#include "CandidatesView.h"
#include "CandidatesFrame.h"
#include "SowThresholdsDlg.h"
#include "SowWetFlagOptionsDlg.h"
#include "StatCauseSet.h"
#include "StatCauseDialog.h"
#include "DOICauseSet.h"
#include "DOICauseDialog.h"
#include "LFIPropertySheet.h"
#include "NFDRS2016WxSet.h"
#include "NFDRS2016WxDoc.h"
#include "NFDRS2016WxFrame.h"
#include "NFDRS2016WxView.h"
#include "WorkingSetDialog.h"

// end added for batch2
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CFireplusApp theApp;

CString generalAssociation = "######";
extern int daysInMonth[12];
extern int curr_runID;

const CString fireErrorStrings[] =
{
	"WARNING: No fires filter specified.\n FireFamily Plus will not process fires\nwithout Fire Associations set.\n",
	"Error: No fires found for specified associations.\nCheck your working set data years and annual filter.\n\n",
};

const CString c_strFModels = "A - Western Annual Grasses\nB - California Chaparral\nC - Pine-Grass Savanna\n"
			"D - Southern Rough\nE - Hardwood Litter (Winter)\nF - Intermediate Brush\n"
			"G - Short-Needle (Heavy Dead)\nH - Short-Needle (Normal Dead)\nI - Heavy Slash\n"
			"J - Intermediate Slash\nK - Light Slash\nL - Western Perennial Grasses\nN - Sawgrass\n"
			"O - High Pocosin\nP - Southern Pine Plantation\nQ - Alaskan Black Spruce\n"
			"R - Hardwood Litter (Summer)\nS - Tundra\nT - Sagebrush-Grass\nU - Western Pines\n"
			"V - Grass\nW - Grass-Shrub\nX - Brush\nY - Timber\nZ - Slash/Blowdown\n"
	;
const CString c_strSlopeClass = "1\n2\n3\n4\n5\n";
const CString c_strSlopePos = "L\nM\nU\n";
const CString c_strClimClass = "1\n2\n3\n4\n";
const CString c_strAspect = "0\n1\n2\n3\n4\n5\n6\n7\n8\n";

void SaveNFDRS2016MetaData(CDatabase *pDB, FFPViewCUG *grid, int row)
{
	char fm[64], station[8], temp[64];
	CString tmp;
	tmp = grid->QuickGetText(2, row);
	strcpy(fm, tmp);
	if (strlen(fm) > 0 && isNFDRS2016(fm[0]))
	{
		tmp = grid->QuickGetText(0, row);
		strcpy(station, tmp);
		sprintf(temp, "[StationID] = '%6.6s'", station);
		CSIGStationSet ssSet(pDB);
		ssSet.m_strFilter = _T(temp);
		ssSet.Open();
		CUGCell cell;
		if (!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
		{
			ssSet.Edit();
			grid->GetCell(21, row, &cell);
			int iVal = cell.GetNumber();//pCellCheck->GetCheck();
			grid->GetCell(22, row, &cell);
			BOOL bVal = cell.GetNumber();
			switch (fm[0])
			{
			case 'v':
			case 'V':
				ssSet.m_SCM_V = iVal;
				ssSet.m_MX_Humid_V = bVal;
				break;
			case 'w':
			case 'W':
				ssSet.m_SCM_W = iVal;
				ssSet.m_MX_Humid_W = bVal;
				break;
			case 'x':
			case 'X':
				ssSet.m_SCM_X = iVal;
				ssSet.m_MX_Humid_X = bVal;
				break;
			case 'y':
			case 'Y':
				ssSet.m_SCM_Y = iVal;
				ssSet.m_MX_Humid_Y = bVal;
				break;
			case 'z':
			case 'Z':
				ssSet.m_SCM_Z = iVal;
				ssSet.m_MX_Humid_Z = bVal;
				break;
			}
			ssSet.Update();
		}
		ssSet.Close();
	}
}

void LoadNFDRS2016MetaData(CDatabase *pDB, FFPViewCUG *grid, int row)
{
	char fm[64], station[8], temp[64];
	CString tmp;
	tmp = grid->QuickGetText(2, row);
	strcpy(fm, tmp);
	if (strlen(fm) > 0 && isNFDRS2016(fm[0]))
	{
		tmp = grid->QuickGetText(0, row);
		strcpy(station, tmp);
		sprintf(temp, "[StationID] = '%6.6s'", station);
		CSIGStationSet ssSet(pDB);
		ssSet.m_strFilter = _T(temp);
		ssSet.Open();
		CUGCell cell;
		if (!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
		{
			int iVal;
			BOOL bVal;
			switch (fm[0])
			{
			case 'v':
			case 'V':
				iVal = ssSet.m_SCM_V;
				bVal = ssSet.m_MX_Humid_V;
				break;
			case 'w':
			case 'W':
				iVal = ssSet.m_SCM_W;
				bVal = ssSet.m_MX_Humid_W;
				break;
			case 'x':
			case 'X':
				iVal = ssSet.m_SCM_X;
				bVal = ssSet.m_MX_Humid_X;
				break;
			case 'y':
			case 'Y':
				iVal = ssSet.m_SCM_Y;
				bVal = ssSet.m_MX_Humid_Y;
				break;
			case 'z':
			case 'Z':
				iVal = ssSet.m_SCM_Z;
				bVal = ssSet.m_MX_Humid_Z;
				break;
			}
			grid->QuickSetNumber(21, row, iVal);
			grid->QuickSetNumber(22, row, bVal);
		}
		ssSet.Close();
	}

}



FFPViewCUG::FFPViewCUG()
{
	m_pDB = NULL;
}

FFPViewCUG::~FFPViewCUG()
{
}

void FFPViewCUG::OnSetup()
{
	CUGCell cell;
	m_iCheckBoxIndex = AddCellType( &m_checkBoxCT );
	m_iDTPickerIndex = AddCellType( &m_dtPicker );
	SetCurrentCellMode(2);
	SetNumberCols(23);
	CRect rect;
	GetClientRect(&rect);
	int wid = rect.Width() / 30;
	QuickSetText(0, -1, "StationID");
	QuickSetText(1, -1, "Name");
	QuickSetText(2, -1, "NFDRS Fuel Model");
	QuickSetText(3, -1, "Use 88 Model");
	QuickSetText(4, -1, "Slope Class");
	QuickSetText(5, -1, "Climate Class");
	QuickSetText(6, -1, "Greenup DOY");
	QuickSetText(7, -1, "Freeze DOY");
	QuickSetText(8, -1, "Start KBDI");
	QuickSetText(9, -1, "Start FM 1000");
	QuickSetText(10, -1, "Avg Precip");
	QuickSetText(11, -1, "FM1 = FM10");
	QuickSetText(12, -1, "Herb Annual");
	QuickSetText(13, -1, "Deciduous");
	QuickSetText(14, -1, "Aspect");
	QuickSetText(15, -1, "Slope Position");
	QuickSetText(16, -1, "Elevation");
	QuickSetText(17, -1, "Latitude");

	GetCell(18,-1,&cell);
	cell.SetCellType(UGCT_NORMAL);
	cell.SetText("Brush Dormant DOY");
	SetCell(18,-1,&cell);

	GetCell(19,-1,&cell);
	cell.SetCellType(UGCT_NORMAL);
	cell.SetText("Use Brush Dormant DOY");
	SetCell(19,-1,&cell);

	GetCell(20,-1,&cell);
	cell.SetCellType(UGCT_NORMAL);
	cell.SetText("Use weighted 10-h sticks");
	SetCell(20,-1,&cell);

	//nfdrs2016 additions
	QuickSetText(21, -1, "Max SC");
	QuickSetText(22, -1, "Humid");

	SetColWidth(0, wid * 3);
	SetColWidth(1, wid * 5);
	SetColWidth(2, wid * 8);
	SetColWidth(3, wid * 4);
	SetColWidth(4, wid * 4);
	SetColWidth(5, wid * 4);
	SetColWidth(6, wid * 5);
	SetColWidth(7, wid * 4);
	SetColWidth(8, wid * 4);
	SetColWidth(9, wid * 5);
	SetColWidth(10, wid * 4);
	SetColWidth(11, wid * 4);
	SetColWidth(12, wid * 4);
	SetColWidth(13, wid * 4);
	SetColWidth(14, wid * 4);
	SetColWidth(15, wid * 5);
	SetColWidth(16, wid * 4);
	SetColWidth(17, wid * 4);
	SetColWidth(18, wid * 7);
	SetColWidth(19, wid * 8);
	SetColWidth(20, wid * 8);
	SetColWidth(21, wid * 4);
	SetColWidth(22, wid * 4);
	GetColDefault( 0, &cell );
	for (int i = 0; i < 23; i++)
		colWidths[i] = GetColWidth(i);
	cell.SetReadOnly(TRUE);
	SetColDefault(0, &cell);
	GetColDefault( 1, &cell );
	cell.SetReadOnly(FALSE);
	SetColDefault(1, &cell);
	//number cells
	GetColDefault( 8, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetNumberDecimals(0);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault(8, &cell);
	GetColDefault( 9, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetNumberDecimals(2);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault(9, &cell);
	GetColDefault( 10, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetNumberDecimals(2);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault(10, &cell);
	GetColDefault( 16, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetNumberDecimals(0);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault(16, &cell);
	GetColDefault( 17, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetNumberDecimals(2);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault(17, &cell);

	//dropdown lists
	GetColDefault( 2, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetCellType(UGCT_DROPLIST);
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText(c_strFModels);
	SetColDefault(2, &cell);
	GetColDefault( 4, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetCellType(UGCT_DROPLIST);
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText(c_strSlopeClass);
	SetColDefault(4, &cell);
	GetColDefault( 5, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetCellType(UGCT_DROPLIST);
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText(c_strClimClass);
	SetColDefault(5, &cell);
	GetColDefault( 14, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetCellType(UGCT_DROPLIST);
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText(c_strAspect);
	SetColDefault(14, &cell);
	GetColDefault( 15, &cell );
	cell.SetReadOnly(FALSE);
	cell.SetCellType(UGCT_DROPLIST);
	cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	cell.SetLabelText(c_strSlopePos);
	SetColDefault(15, &cell);
	//checkboxes
	GetColDefault( 3, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 3, &cell );
	GetColDefault( 11, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 11, &cell );
	GetColDefault( 12, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 12, &cell );
	GetColDefault( 13, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 13, &cell );
//date fields
	GetColDefault( 6, &cell );
	cell.SetDataType(UGCELLDATA_TIME);
	cell.SetCellType( m_iDTPickerIndex );
	cell.SetCellTypeEx( UGCT_DROPLISTHIDEBUTTON );
	//cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 6, &cell );
	GetColDefault( 7, &cell );
	cell.SetDataType(UGCELLDATA_TIME);
	cell.SetCellType( m_iDTPickerIndex );
	cell.SetCellTypeEx( UGCT_DROPLISTHIDEBUTTON );
	//cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 7, &cell );

	GetColDefault( 18, &cell );
	cell.SetDataType(UGCELLDATA_TIME);
	cell.SetCellType( m_iDTPickerIndex );
	cell.SetCellTypeEx( UGCT_DROPLISTHIDEBUTTON );
	//cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 18, &cell );

	GetColDefault( 19, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 19, &cell );

	GetColDefault( 20, &cell );
	cell.SetCellType( m_iCheckBoxIndex );
	cell.SetCellTypeEx( UGCT_CHECKBOXUSEALIGN );
	cell.SetAlignment( UG_ALIGNCENTER|UG_ALIGNVCENTER );
	SetColDefault( 20, &cell );

	GetColDefault(21, &cell);
	cell.SetReadOnly(FALSE);
	cell.SetNumberDecimals(0);
	cell.SetParam(USE_COXNUMBER);
	SetColDefault(21, &cell);

	GetColDefault(22, &cell);
	cell.SetCellType(m_iCheckBoxIndex);
	cell.SetCellTypeEx(UGCT_CHECKBOXUSEALIGN);
	cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
	SetColDefault(22, &cell);

	((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );
	LockColumns(1);

	//create the metadata table
	/*m_Grid.SetVirtualMode(FALSE);
	m_Grid.SetHeaderSort(FALSE);
	m_Grid.SetListMode(FALSE);
	m_Grid.SetFrameFocusCell(FALSE);
	m_Grid.SetTrackFocusCell(FALSE);
	m_Grid.SetColumnCount(18);
	m_Grid.SetRowCount(1);
	m_Grid.SetFixedRowCount(1);
	m_Grid.SetFixedColumnCount(1);
	m_Grid.SetDefCellHeight(22);
	//headers
	m_Grid.SetItemText(0, 0, "StationID");
	m_Grid.SetItemText(0, 1, "Name");
	m_Grid.SetItemText(0, 2, "NFDRS Fuel Model");
	m_Grid.SetItemText(0, 3, "Use 88 Model");
	m_Grid.SetItemText(0, 4, "Slope Class");
	m_Grid.SetItemText(0, 5, "Climate Class");
	m_Grid.SetItemText(0, 6, "Greenup DOY");
	m_Grid.SetItemText(0, 7, "Freeze DOY");
	m_Grid.SetItemText(0, 8, "Start KBDI");
	m_Grid.SetItemText(0, 9, "Start FM 1000");
	m_Grid.SetItemText(0, 10, "Avg Precip");
	m_Grid.SetItemText(0, 11, "FM1 = FM10");
	m_Grid.SetItemText(0, 12, "Herb Annual");
	m_Grid.SetItemText(0, 13, "Deciduous");
	m_Grid.SetItemText(0, 14, "Aspect");
	m_Grid.SetItemText(0, 15, "Slope Position");
	m_Grid.SetItemText(0, 16, "Elevation");
	m_Grid.SetItemText(0, 17, "Latitude");
	m_Grid.EnableTitleTips(FALSE);
	m_Grid.EnableToolTips(FALSE);
	m_Grid.AutoSizeColumns();
	m_Grid.SetGridLines(GVL_BOTH);*/
}

int FFPViewCUG::OnEditStart(int col, long row,CWnd **edit)
{
	CUGCell cell;
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
		case 21:
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
	}
	return TRUE;
}

void FFPViewCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void FFPViewCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey );
}

void FFPViewCUG::OnKeyDown(UINT *vcKey,BOOL processed)
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

int FFPViewCUG::OnCellTypeNotify(long ID,int col,long row,long msg,long param)
{
	if(ID == m_iDTPickerIndex)
	{
		if(msg == UGCT_CONVERTDATE)
		{
			UGCTCONVERTDATE* cd = (UGCTCONVERTDATE*)param;
			cd->string = cd->date.Format("%m/%d");
		}
	}
	if (col == 2 && msg == UGCT_DROPLISTPOSTSELECT)
	{
		//may change visible columns

		CString fm = QuickGetText(col, row);
		if (fm.GetLength() > 0 && isNFDRS2016(fm[0]))
		{
			//((CFireplusView *)GetParent())->LoadNFDRS2016MetaData(row);
			LoadNFDRS2016MetaData(m_pDB, this, row);
		}
		CWnd *pView = GetParent();
		CRuntimeClass *pClass = pView->GetRuntimeClass();
		if (strcmp(pClass->m_lpszClassName, "CFireplusView") == 0)
		{
			((CFireplusView *)pView)->SetYears(fm);
			((CFireplusView *)pView)->ConfigCheckBoxes();
		}
		else if (strcmp(pClass->m_lpszClassName, "CWorkingSetDialog") == 0)
		{
			((CWorkingSetDialog *)pView)->SetYears(fm);
			((CWorkingSetDialog *)pView)->ConfigCheckBoxes();
		}
		//else if()
		SetVisibleColumns();
		RedrawAll();
	}
	else if (col == 2 && msg == UGCT_DROPLISTSTART)
	{
		//may have to save some other fields if NFDR2016 fuel
		CString fm = QuickGetText(col, row);
		if (fm.GetLength() > 0 && isNFDRS2016(fm[0]))
		{
			//((CFireplusView *)GetParent())->SaveNFDRS2016MetaData(row);
			SaveNFDRS2016MetaData(m_pDB, this, row);
		}
	}
	return TRUE;
}

COLORREF FFPViewCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_WINDOW);
	else
		return GetSysColor(COLOR_BTNFACE);
}

void FFPViewCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

void FFPViewCUG::SetVisibleColumns()
{
	//first, determine if any rows using old NFDRS fuel models
	//if so, display all columns
	//if not, display only columns pertaining to NFDRS2016
	bool allNFDRS2016 = true;
	CString str;
	for (int r = 0; r < this->GetNumberRows(); r++)
	{
		str = QuickGetText(2, r);
		if (!isNFDRS2016(str[0]))
		{
			allNFDRS2016 = false;
			break;
		}
	}
	SetColWidth(3, allNFDRS2016 ? 0 : colWidths[3]);
	SetColWidth(5, allNFDRS2016 ? 0 : colWidths[5]);
	SetColWidth(6, allNFDRS2016 ? 0 : colWidths[6]);
	SetColWidth(7, allNFDRS2016 ? 0 : colWidths[7]);
	SetColWidth(8, allNFDRS2016 ? 0 : colWidths[8]);
	SetColWidth(9, allNFDRS2016 ? 0 : colWidths[9]);
	SetColWidth(11, allNFDRS2016 ? 0 : colWidths[11]);
	//SetColWidth(12, allNFDRS2016 ? 0 : colWidths[12]);
	SetColWidth(13, allNFDRS2016 ? 0 : colWidths[13]);
	SetColWidth(14, allNFDRS2016 ? 0 : colWidths[14]);
	SetColWidth(15, allNFDRS2016 ? 0 : colWidths[15]);
	SetColWidth(16, allNFDRS2016 ? 0 : colWidths[16]);
	SetColWidth(18, allNFDRS2016 ? 0 : colWidths[18]);
	SetColWidth(19, allNFDRS2016 ? 0 : colWidths[19]);
	SetColWidth(20, allNFDRS2016 ? 0 : colWidths[20]);

	SetColWidth(21, allNFDRS2016 ? colWidths[21] : 0);
	SetColWidth(22, allNFDRS2016 ? colWidths[22] : 0);
	if (!allNFDRS2016)
	{
		theApp.m_UseDailyExtremes = FALSE;
		theApp.m_ForceNFDRS2016Recompute = FALSE;
		//m_chkUSeDailyExtremes.SetCheck(FALSE);
		//m_chkForceRecompute.SetCheck(FALSE);
	}
	//added 10/7/2019 to force scroll bar resizing
	OnSize(1,1,1);
}


/////////////////////////////////////////////////////////////////////////////
// CFireplusView

IMPLEMENT_DYNCREATE(CFireplusView, CRecordView)

BEGIN_MESSAGE_MAP(CFireplusView, CRecordView)
	//{{AFX_MSG_MAP(CFireplusView)
	ON_COMMAND(ID_CLIMATOLOGY, OnClimatology)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_DATA_IMPORT, OnDataImport)
	ON_CBN_SELCHANGE(IDC_SIGSTATION, OnSelchangeSigstation)
	ON_COMMAND(ID_SIGS, OnSigs)
	ON_COMMAND(ID_RERAP, OnRerap)
	ON_COMMAND(ID_WEATHER_POCKETCARD, OnWeatherPocketcard)
	ON_COMMAND(ID_WEATHER_SEASONREPORTS_PERSISTENCEPROBABILITY, OnWeatherSeasonreportsPersistenceprobability)
	ON_COMMAND(ID_WEATHER_SEASONREPORTS_PROBABILITYANALYSIS, OnWeatherSeasonreportsProbabilityanalysis)
	ON_COMMAND(ID_WEATHER_SEASONREPORTS_SEVERITYLIST, OnWeatherSeasonreportsSeveritylist)
	ON_COMMAND(ID_WEATHER_SEASONREPORTS_SEVERITYSUMMARY, OnWeatherSeasonreportsSeveritysummary)
	ON_COMMAND(ID_FIRES_FIREBUSINESS, OnFiresFirebusiness)
	ON_COMMAND(ID_WEATHER_SEASONREPORTS_DAILYLISTING, OnWeatherSeasonreportsDailylisting)
	ON_COMMAND(ID_DATA_STATIONS, OnDataStations)
	ON_COMMAND(ID_FIRES_ASSOCIATIONS, OnFiresAssociations)
	ON_BN_CLICKED(IDC_FIRESBUTTON, OnFiresbutton)
	ON_COMMAND(ID_FIRES_FIREANALYSIS, OnFiresFireanalysis)
	ON_COMMAND(ID_FIRES_SUMMARY_WORKINGSET, OnFiresSummaryWorkingset)
	ON_COMMAND(ID_DATA_AGENCIES, OnDataAgencies)
	ON_COMMAND(ID_WEATHER_WINDS, OnWeatherWindRose)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_WEATHER_EVENTLOCATOR, OnWeatherEventlocator)
	ON_COMMAND(ID_WEATHER_FORECASTING, OnWeatherForecasting)
	ON_COMMAND(ID_WEATHER_NFDRSCALCULATOR, OnWeatherNfdrscalculator)
	ON_COMMAND(ID_WEATHER_VIEWOBSERVATIONS_ALL, OnWeatherViewobservationsAll)
	ON_COMMAND(ID_WEATHER_VIEWOBSERVATIONS_DAILY, OnWeatherViewobservationsDaily)
	ON_COMMAND(ID_WEATHER_DIURNAL_GRAPH, OnWeatherDiurnalGraph)
	ON_UPDATE_COMMAND_UI(ID_WEATHER_DIURNAL_GRAPH, OnUpdateWeatherDiurnalGraph)
	ON_COMMAND(ID_WEATHER_DIURNAL_HOURLYLISTING, OnWeatherDiurnalHourlylisting)
	ON_UPDATE_COMMAND_UI(ID_WEATHER_DIURNAL_HOURLYLISTING, OnUpdateWeatherDiurnalHourlylisting)
	ON_COMMAND(ID_WEATHER_HOURLYDATAANALYSIS_FARSITEEXPORTS, OnFARSITEExports)
	ON_COMMAND(ID_CALCULATEDSOW_THRESHOLDS, OnWeatherHourlydataanalysisSOWThresholds)
	ON_COMMAND(ID_CALCULATEDSOW_OPTIONS, OnWeatherHourlydataanalysisSOWOptions)
	ON_COMMAND(ID_CALCULATEDSOW_RECALCULATESR, OnWeatherHourlydataanalysisSOWReCalc)
	ON_COMMAND(ID_OPTIONS_CAUSECODES , OnOptionsCauseCodes)
	ON_COMMAND(ID_OPTIONS_CAUSECODES2 , OnOptionsCauseCodes2)
	

	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_WEATHER_HOURLYDATAANALYSIS_FARSITEEXPORTS, OnUpdateWeatherHourlydataanalysisFARSITEexports)
	ON_UPDATE_COMMAND_UI(ID_CALCULATEDSOW_THRESHOLDS, OnUpdateWeatherHourlydataanalysisSOWThresholds)
	ON_UPDATE_COMMAND_UI(ID_CALCULATEDSOW_OPTIONS, OnUpdateWeatherHourlydataanalysisSOWOptions)
	ON_UPDATE_COMMAND_UI(ID_CALCULATEDSOW_RECALCULATESR, OnUpdateWeatherHourlydataanalysisSOWReCalc)

	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeCombo1)
	ON_COMMAND(ID_DATA_USERVARIABLES_VIEWOBSERVATIONS, OnDataUservariablesViewobservations)
	// added for batch2
	ON_COMMAND(ID_OPTIONS_RUN_RESTORE, OnRunRestore)
	ON_COMMAND(ID_BATCH_CREATENEWBATCH, OnBatchCreateNew)
	ON_COMMAND(ID_BATCH_MANAGEBATCHLIST,OnBatchManage)
	ON_COMMAND(ID_BATCH_SAVEDRUNMAINTENANCE,OnBatchSavedRunMaint)
	ON_COMMAND(ID_BATCH_INTERACTIVE,OnBatchInteractive)
	// end added for batch2
	//}}AFX_MSG_MAP
	// Standard printing commands
	//ON_COMMAND(ID_FILE_PRINT, CDaoRecordView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_DIRECT, CDaoRecordView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW, CDaoRecordView::OnFilePrintPreview)
	ON_COMMAND(ID_WEATHER_FLAMMAPEXPORTS, &CFireplusView::OnWeatherFlammapexports)
	ON_COMMAND(ID_NELSON, OnNelsonDFM)
	//ON_COMMAND(ID_NELSON2, OnNelsonDFM2)
	ON_COMMAND(ID_CUSTOMYEARLYINITS,OnCustomYearlyInits)
	ON_COMMAND(ID_WEATHER_HOURLY_EVENTLOCATOR,OnHourlyEventLocator)
	ON_UPDATE_COMMAND_UI(ID_WEATHER_HOURLY_EVENTLOCATOR,OnUpdateHourlyEventLocator)
	//ON_COMMAND(ID_OPTIONS_LFISETTINGS, &CFireplusView::OnOptionsLfisettings)
	ON_COMMAND(ID_WEATHER_FIREBUSINESSCANDIDATES, &CFireplusView::OnWeatherFirebusinesscandidates)
	//ON_COMMAND(ID_WEATHER_TERM, &CFireplusView::OnWeatherTerm)
	ON_COMMAND(ID_TERM_GENERAL, &CFireplusView::OnTermGeneral)
	ON_COMMAND(ID_TERM_WORKINGSET, &CFireplusView::OnTermWorkingset)
	//ON_COMMAND(ID_NELSONDEADFUELMOISTURES_FIRESANALYSIS, &CFireplusView::OnNelsondeadfuelmoisturesFiresanalysis)
	ON_UPDATE_COMMAND_UI(ID_NELSONDEADFUELMOISTURES_FIRESANALYSIS, &CFireplusView::OnUpdateNelsondeadfuelmoisturesFiresanalysis)
	ON_COMMAND(ID_FIRESANALYSIS_ALLNELSONMOISTURES, &CFireplusView::OnFiresanalysisAllnelsonmoistures)
	ON_COMMAND(ID_FIRESANALYSIS_1AND10HOURMOISTURES, &CFireplusView::OnFiresanalysis1and10hourmoistures)
	ON_UPDATE_COMMAND_UI(ID_FIRESANALYSIS_ALLNELSONMOISTURES, &CFireplusView::OnUpdateFiresanalysisAllnelsonmoistures)
	ON_UPDATE_COMMAND_UI(ID_FIRESANALYSIS_1AND10HOURMOISTURES, &CFireplusView::OnUpdateFiresanalysis1and10hourmoistures)
	ON_COMMAND(ID_COMPAREWITHDAILYNFDRS_1AND10HOURMOISTURES, &CFireplusView::OnComparewithdailynfdrs1and10hourmoistures)
	ON_COMMAND(ID_COMPAREWITHDAILYNFDRS_ALLNELSONMOISTURES, &CFireplusView::OnComparewithdailynfdrsAllnelsonmoistures)
	ON_UPDATE_COMMAND_UI(ID_COMPAREWITHDAILYNFDRS_1AND10HOURMOISTURES, &CFireplusView::OnUpdateComparewithdailynfdrs1and10hourmoistures)
	ON_UPDATE_COMMAND_UI(ID_COMPAREWITHDAILYNFDRS_ALLNELSONMOISTURES, &CFireplusView::OnUpdateComparewithdailynfdrsAllnelsonmoistures)
	ON_COMMAND(ID_LFIOPTIONS_CURRENTSIG, &CFireplusView::OnLfioptionsCurrentsig)
	ON_COMMAND(ID_LFIOPTIONS_DEFAULTS, &CFireplusView::OnLfioptionsDefaults)
	ON_EN_CHANGE(IDC_SDAY, &CFireplusView::OnEnChangeSday)
	ON_EN_CHANGE(IDC_EDAY, &CFireplusView::OnEnChangeEday)
	ON_EN_CHANGE(IDC_EYEAR, &CFireplusView::OnEnChangeEyear)
	ON_EN_CHANGE(IDC_SYEAR, &CFireplusView::OnEnChangeSyear)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireplusView construction/destruction

CFireplusView::CFireplusView()
	: CRecordView(IDD_FIREPLUS_FORM)
{
	//{{AFX_DATA_INIT(CFireplusView)
	m_pSet = NULL;
	m_dbName = _T("");
	//}}AFX_DATA_INIT
	hasHourly = FALSE;
	UpdateOnDeactivate = TRUE;
	tableBottomMin = 0;
	tableRightMin = 0;
	forecastDialog = NULL;
	ignoreMetaTable = false;
}

CFireplusView::~CFireplusView()
{
}

void CFireplusView::DoDataExchange(CDataExchange* pDX)
{
	CRecordView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFireplusView)
	DDX_Control(pDX, IDC_DATAYEARS, dataYearsBtn);
	DDX_Control(pDX, IDC_SYEAR, m_StartYear);
	DDX_Control(pDX, IDC_SMONTH, m_StartMonth);
	DDX_Control(pDX, IDC_SDAY, m_StartDay);
	DDX_Control(pDX, IDC_EYEAR, m_EndYear);
	DDX_Control(pDX, IDC_EDAY, m_EndDay);
	DDX_Control(pDX, IDC_EMONTH, m_EndMonth);
	DDX_Control(pDX, IDC_COMBO1, m_PeriodLenControl);
	DDX_Control(pDX, IDC_SIGSTATION, m_ctlSIG_Station);
	DDX_Control(pDX, IDC_SPIN4, m_spnEday);
	DDX_Control(pDX, IDC_SPIN3, m_spnSday);
	DDX_Control(pDX, IDC_SPIN2, m_spnEyear);
	DDX_Control(pDX, IDC_SPIN1, m_spnSyear);
	DDX_Text(pDX, IDC_DBNAME, m_dbName);
	//	DDX_Control(pDX, IDC_DATATBLCTRL1, m_metaTable);
	DDX_FieldText(pDX, IDC_SYEAR, m_pSet->m_StartYear, m_pSet);
	DDX_FieldText(pDX, IDC_EYEAR, m_pSet->m_EndYear, m_pSet);
	DDX_FieldText(pDX, IDC_EDAY, m_pSet->m_EndDay, m_pSet);
	DDX_FieldText(pDX, IDC_SDAY, m_pSet->m_StartDay, m_pSet);
	DDX_FieldText(pDX, IDC_DESCRIPTION, m_pSet->m_Description, m_pSet);
	DDV_MaxChars(pDX, m_pSet->m_Description, 100);
	DDX_FieldCBString(pDX, IDC_SMONTH, m_pSet->m_StartMonth, m_pSet);
	DDX_FieldCBString(pDX, IDC_EMONTH, m_pSet->m_EndMonth, m_pSet);
	DDX_FieldCBString(pDX, IDC_COMBO1, m_pSet->m_PeriodLength, m_pSet);
	DDX_FieldCheck(pDX, IDC_ENABLEALTERNATE, m_pSet->m_Use78, m_pSet);
	DDX_Check(pDX, IDC_CHECK_FORCE_RECOMPUTE, theApp.m_ForceNFDRS2016Recompute);
	DDX_Check(pDX, IDC_CHECK_DAILY_EXTREMES, theApp.m_UseDailyExtremes);
	//DDX_FieldText(pDX, IDC_SDAY, m_iStartDay);
	/*if(pDX->m_bSaveAndValidate)
	{
	DDX_Text(pDX, IDC_SDAY, m_iStartDay);
	DDX_Text(pDX, IDC_EDAY, m_iEndDay);
	}
	DDV_MinMaxInt(pDX, m_iStartDay, 1, 31);
	DDV_MinMaxInt(pDX, m_iEndDay, 1, 31);*/
	//DDV_MinMaxInt(pDX, m_StartDay, 1, 31);
	//}}AFX_DATA_MAP
	//DDX_Control(pDX, IDC_GRID, m_Grid);             // associate the grid window with a C++ object
	DDX_Control(pDX, IDC_CHECK_DAILY_EXTREMES, m_chkUSeDailyExtremes);
	DDX_Control(pDX, IDC_CHECK_FORCE_RECOMPUTE, m_chkForceRecompute);
}

BOOL CFireplusView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CRecordView::PreCreateWindow(cs);
}

void CFireplusView::OnInitialUpdate()
{
	m_pSet = GetDocument()->m_fireplusSet;
	m_dbName = GetDocument()->m_pDB->GetDatabaseName();
	GetDocument()->m_view = this;
	CRecordView::OnInitialUpdate();
	m_grid.AttachGrid(this, IDC_GRID);
	m_grid.m_pDB = this->GetDocument()->m_pDB;
	m_StartYear.SetWindowText("");
	m_EndYear.SetWindowText("");
	m_spnSyear.SetRange(1900, 2099);
	m_spnEyear.SetRange(1900, 2099);
	m_spnSday.SetRange(1, 31);
	m_spnEday.SetRange(1, 31);
	ConfigureStationList();
	//m_iStartDay = m_spnSday.GetPos();
	//m_iEndDay = m_spnEday.GetPos();
	//update metadata table
	//m_metaTable.GetRowSet().Reset();
	// OnSelchangeSigstation();
	// changed 2012
	InitSigstation();

	CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
	GetDocument()->SetTitle(GetDocument()->GetTitle() + " - Working Set");
	ResizeParentToFit(false);
	CRect tRect;
	m_grid.GetClientRect(&tRect);
	tableBottomMin = tRect.bottom;
	tableRightMin = tRect.right;

	fxParamChange = false;
}

/////////////////////////////////////////////////////////////////////////////
// CFireplusView printing

BOOL CFireplusView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return 0;//DoPreparePrinting(pInfo);
}

void CFireplusView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CFireplusView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CFireplusView diagnostics

#ifdef _DEBUG
void CFireplusView::AssertValid() const
{
	CRecordView::AssertValid();
}

void CFireplusView::Dump(CDumpContext& dc) const
{
	CRecordView::Dump(dc);
}

CFireplusDoc* CFireplusView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFireplusDoc)));
	return (CFireplusDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFireplusView database support
CRecordset* CFireplusView::OnGetRecordset()
{
	return m_pSet;
}

/////////////////////////////////////////////////////////////////////////////
// CFireplusView message handlers

void CFireplusView::OnClimatology()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	//see if climatology window exists. If so, make it active
	//CWnd *next;
	CWnd *next = ((CMDIFrameWnd *)GetParentOwner())->MDIGetActive();
	while(next)
	{
		CRuntimeClass* prt = next->GetRuntimeClass();
		if(strcmp( prt->m_lpszClassName, "CMDIChildWnd" )  == 0)
		{
			next->BringWindowToTop();
			return;
		}
		next = next->GetNextWindow();
	}

	//didn't exist, so create it
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate =
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("ClimSet"))
		{
			CClimDoc *pDoc = new CClimDoc();
			pDoc->m_pSet = GetDocument()->m_fireplusSet;
			CFrameWnd* pFrame = curTemplate->CreateNewFrame(pDoc, NULL);
			curTemplate->InitialUpdateFrame( pFrame, pDoc);
			return;
		}
	}
	//CClimDialog cd(this, m_pSet->m_pDatabase);
	//cd.DoModal();
}

void CFireplusView::OnFileSave()
{
	SaveMetaTable();
	CString oldEmonth = m_pSet->m_EndMonth,
		oldSmonth = m_pSet->m_StartMonth;
	short oldEday = m_pSet->m_EndDay, oldSday = m_pSet->m_StartDay,
		oldEyear = m_pSet->m_EndYear, oldSyear = m_pSet->m_StartYear;
	try
	{
	m_pSet->Edit();
	if(!UpdateData(TRUE))
		return;
	m_pSet->Update();
		}
		catch (CDBException* e)
		{
			DisplayDBException(e);
			e->Delete();
			return;
		}
	if(m_pSet->m_EndDay != oldEday || m_pSet->m_EndYear != oldEyear
		|| m_pSet->m_StartDay != oldSday || m_pSet->m_StartYear != oldSyear
		 || m_pSet->m_EndMonth != oldEmonth
		|| m_pSet->m_StartMonth != oldSmonth)
		fxParamChange = true;

	//save the active SIG/Station
	if(m_ctlSIG_Station.GetCount() <= 0 || m_ctlSIG_Station.GetCurSel() == CB_ERR)
		return;
	CString str;
	m_pSet->Edit();
	int loc = m_ctlSIG_Station.GetCurSel();
	m_ctlSIG_Station.GetLBText(loc, str);
	if(str[0] == 'S' && str[1] == 'I' && str[2] == 'G')//SIG
	{
		//char sig[32];
		//strcpy(sig, str);
		m_pSet->m_SIG_Station = str;
	}
	else //single station, only save stationID
	{
		char sta[16];
		strncpy(sta, str, 6);
		sta[6] = 0;
		m_pSet->m_SIG_Station = sta;
	}
	m_pSet->Update();
	//do adjustments to days of month if necessary....
	if(m_pSet->m_StartDay > 28)
	//if(m_pSet->m_StartDay > 29)
	{
		switch(MonthChtoInt(m_pSet->m_StartMonth))
		{
		case 4:
		case 6:
		case 9:
		case 11:
			if(m_pSet->m_StartDay == 31)
			{
				m_pSet->Edit();
				m_pSet->m_StartDay = 30;
				m_pSet->Update();
			}
			break;
		case 2:
			{
				m_pSet->Edit();
				m_pSet->m_StartDay = 28;
				m_pSet->Update();
			}
			break;
		default://up to 31 is OK
			break;
		}
	}
	if(m_pSet->m_EndDay > 28)
	//if(m_pSet->m_EndDay > 29)
	{
		switch(MonthChtoInt(m_pSet->m_EndMonth))
		{
		case 4:
		case 6:
		case 9:
		case 11:
			if(m_pSet->m_EndDay == 31)
			{
				m_pSet->Edit();
				m_pSet->m_EndDay = 30;
				m_pSet->Update();
			}
			break;
		case 2:
			{
				m_pSet->Edit();
				m_pSet->m_EndDay = 28;
				//m_pSet->m_EndDay = 29;
				m_pSet->Update();
			}
			break;
		default://up to 31 is OK
			break;
		}
	}
}

void CFireplusView::OnDataImport()
{
	// TODO: Add your command handler code here
	OnFileSave();
	CImportDialog dlg(this, m_pSet->m_pDatabase);
	dlg.DoModal();
	ConfigureStationList();
	ignoreMetaTable = true;
	OnSelchangeSigstation();
}

void CFireplusView::OnSelchangeSigstation()
{
	// TODO: Add your control notification handler code here
	//must reset date frames to valid years based on new selection
	char temp[64], station[64];
	temp[0] = 0;
	if(!ignoreMetaTable)
		SaveMetaTable();
	ignoreMetaTable = false;
	int itemLoc = m_ctlSIG_Station.GetCurSel(), yr1 = 2030, yr2 = 0;
	CUGCell cell;
	if(itemLoc != CB_ERR)
	{
		CWaitCursor wait;
		hasHourly = FALSE;

		m_ctlSIG_Station.GetLBText(itemLoc, station);
		CSIGStationSet ssSet(GetDocument()->m_pDB);
		ssSet.Open();
		m_grid.SetNumberRows(0);
		//set up string arrays for combobox columns...

		if(strncmp(station, "SIG", 3) == 0)//is a SIG
		{
			CStationInSIGSet sSet(GetDocument()->m_pDB);
			sprintf(temp, "[SIG] = '%-20.20s'", &station[6]);
			sSet.m_strFilter = _T(temp);
			sSet.Open();
			//wxSet.Open();
			int row = 0;
			while(!sSet.IsEOF())
			{
				sprintf(temp, "[StationID] = '%6.6s'", sSet.m_StationID);
				ssSet.m_strFilter = _T(temp);
				ssSet.Requery();
				if(!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
				{
					if(ssSet.m_GreenJulian.m_status != COleDateTime::valid)
					{
						ssSet.Edit();
						ssSet.m_GreenJulian.SetDateTime(2008, 1, 1, 13, 0, 0);
						ssSet.Update();
					}
					if(ssSet.m_FreezeJulian.m_status != COleDateTime::valid)
					{
						ssSet.Edit();
						ssSet.m_FreezeJulian.SetDateTime(2008, 12, 31, 13, 0, 0);
						ssSet.Update();
					}
					if(ssSet.m_DormantJulian.m_status != COleDateTime::valid)
					{
						ssSet.Edit();
						ssSet.m_DormantJulian.SetDateTime(2008, 9, 15, 13, 0, 0);
						ssSet.Update();
					}
					m_grid.AppendRow();
					m_grid.QuickSetText(0, row, ssSet.m_StationID);
					m_grid.QuickSetText(1, row, ssSet.m_Name);
					m_grid.GetCell(2, row, &cell);
					if(ssSet.IsFieldNull(&ssSet.m_NFDRSFM))
					{
						strcpy(temp, " ");
					}
					else
						sprintf(temp, "%c - ", ssSet.m_NFDRSFM[0]);
					if(temp[0] != ' ')
					{
						CString list = c_strFModels;//cell.GetLabelText();//m_metaTable.GetColumnSet().GetItem(2).GetList();
						char *pStr = (char *)strstr(list, temp);
						int l = 0;
						while(pStr[l] != '\t' && pStr[l] != '\n' && pStr[l] != 0)
							temp[l] = pStr[l++];
						temp[l] = 0;
					}
					cell.SetText(temp);
					m_grid.SetCell(2, row, &cell);
					m_grid.GetCell(3, row, &cell);
					cell.SetNumber(ssSet.m_Use88);
					m_grid.SetCell(3, row, &cell);
					CString tmp;
					tmp.Format("%d", ssSet.m_SlopeCls);
					m_grid.QuickSetText(4, row, tmp);
					tmp.Format("%d", ssSet.m_ClimateCls);
					m_grid.QuickSetText(5, row, tmp);
					m_grid.GetCell(6, row, &cell);
					tmp = ssSet.m_GreenJulian.Format("%m/%d");
					cell.SetText(tmp);
					m_grid.SetCell(6, row, &cell);
					m_grid.GetCell(7, row, &cell);
					tmp = ssSet.m_FreezeJulian.Format("%m/%d");
					cell.SetText(tmp);
					m_grid.SetCell(7, row, &cell);
					m_grid.QuickSetNumber(8, row, ssSet.m_StartKBDI);
					m_grid.QuickSetNumber(9, row, ssSet.m_Start1000);
					m_grid.QuickSetNumber(10, row, ssSet.m_AvgPrecip);
					m_grid.QuickSetNumber(11, row, ssSet.m_FM1Eq10);
					m_grid.QuickSetNumber(12, row, ssSet.m_HerbAnnual);
					m_grid.QuickSetNumber(13, row, ssSet.m_Deciduous);
					tmp = ssSet.m_Aspect;
					m_grid.QuickSetText(14, row, tmp);
					tmp = ssSet.m_PsnOnSlope;
					m_grid.QuickSetText(15, row, tmp);
					m_grid.QuickSetNumber(16, row, ssSet.m_Elevation);
					m_grid.QuickSetNumber(17, row, ssSet.m_LatDegrees);

					m_grid.GetCell(18, row, &cell);
					tmp = ssSet.m_DormantJulian.Format("%m/%d");
					cell.SetText(tmp);
					m_grid.SetCell(18, row, &cell);

					m_grid.GetCell(19, row, &cell);
					cell.SetNumber(ssSet.m_UseDormant);
					m_grid.SetCell(19, row, &cell);

					m_grid.GetCell(20, row, &cell);
					cell.SetNumber(ssSet.m_UseStick);
					m_grid.SetCell(20, row, &cell);

					//if it's an NFDR2016 Fuel set SCM and MX_Humid
					if (isNFDRS2016(ssSet.m_NFDRSFM[0]))
					{
						int scmVal, mxVal;
						switch (ssSet.m_NFDRSFM[0])
						{
						case 'v':
						case 'V':
							scmVal = ssSet.m_SCM_V;
							mxVal = ssSet.m_MX_Humid_V;
							break;
						case 'w':
						case 'W':
							scmVal = ssSet.m_SCM_W;
							mxVal = ssSet.m_MX_Humid_W;
							break;
						case 'x':
						case 'X':
							scmVal = ssSet.m_SCM_X;
							mxVal = ssSet.m_MX_Humid_X;
							break;
						case 'y':
						case 'Y':
							scmVal = ssSet.m_SCM_Y;
							mxVal = ssSet.m_MX_Humid_Y;
							break;
						case 'z':
						case 'Z':
							scmVal = ssSet.m_SCM_Z;
							mxVal = ssSet.m_MX_Humid_Z;
							break;
						}
						m_grid.QuickSetNumber(21, row, scmVal);
						m_grid.QuickSetNumber(22, row, mxVal);
					}
				}
				//check for hourly data
				sSet.MoveNext();
				row++;
			}
			sSet.Close();
		}
		else
		{	//a single station
			CString sta;
			
			sta.Format("%6.6s",station);
			// WRCC id fix
			if (sta.Mid(5,1)== _T("-"))
			   sta.Format("%4.4s",station);

			sprintf(temp, "[StationID] = '%6.6s'", sta);
			

			ssSet.m_strFilter = _T(temp);
			ssSet.Requery();
			if(!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
			{
				if(ssSet.m_GreenJulian.m_status != COleDateTime::valid)
				{
					ssSet.Edit();
					ssSet.m_GreenJulian.SetDateTime(2008, 1, 1, 13, 0, 0);
					ssSet.Update();
				}
				if(ssSet.m_DormantJulian.m_status != COleDateTime::valid)
				{
					ssSet.Edit();
					ssSet.m_DormantJulian.SetDateTime(2008, 9, 15, 13, 0, 0);
					ssSet.Update();
				}
				if(ssSet.m_FreezeJulian.m_status != COleDateTime::valid)
				{
					ssSet.Edit();
					ssSet.m_FreezeJulian.SetDateTime(2008, 12, 31, 13, 0, 0);
					ssSet.Update();
				}
				int row = 0;
				m_grid.AppendRow();
				m_grid.QuickSetText(0, row, ssSet.m_StationID);
				m_grid.QuickSetText(1, row, ssSet.m_Name);
				m_grid.GetCell(2, row, &cell);
				if(ssSet.IsFieldNull(&ssSet.m_NFDRSFM))
					strcpy(temp, " ");
				else
					sprintf(temp, "%c - ", ssSet.m_NFDRSFM[0]);
				if(temp[0] != ' ')
				{
					CString list = c_strFModels;
					char *pStr = (char *)strstr(list, temp);
					int l = 0;
					while(pStr[l] != '\t' && pStr[l] != '\n' && pStr[l] != 0)
						temp[l] = pStr[l++];
					temp[l] = 0;
				}
				cell.SetText(temp);
				m_grid.SetCell(2, row, &cell);
				m_grid.GetCell(3, row, &cell);
				cell.SetNumber(ssSet.m_Use88);
				m_grid.SetCell(3, row, &cell);
				CString tmp;
				tmp.Format("%d", ssSet.m_SlopeCls);
				m_grid.QuickSetText(4, row, tmp);
				tmp.Format("%d", ssSet.m_ClimateCls);
				m_grid.QuickSetText(5, row, tmp);
				m_grid.GetCell(6, row, &cell);
				tmp = ssSet.m_GreenJulian.Format("%m/%d");
				cell.SetText(tmp);
				m_grid.SetCell(6, row, &cell);
				m_grid.GetCell(7, row, &cell);
				tmp = ssSet.m_FreezeJulian.Format("%m/%d");
				cell.SetText(tmp);
				m_grid.SetCell(7, row, &cell);
				m_grid.QuickSetNumber(8, row, ssSet.m_StartKBDI);
				m_grid.QuickSetNumber(9, row, ssSet.m_Start1000);
				m_grid.QuickSetNumber(10, row, ssSet.m_AvgPrecip);
				m_grid.QuickSetNumber(11, row, ssSet.m_FM1Eq10);
				m_grid.QuickSetNumber(12, row, ssSet.m_HerbAnnual);
				m_grid.QuickSetNumber(13, row, ssSet.m_Deciduous);
				tmp = ssSet.m_Aspect;
				m_grid.QuickSetText(14, row, tmp);
				tmp = ssSet.m_PsnOnSlope;
				m_grid.QuickSetText(15, row, tmp);
				m_grid.QuickSetNumber(16, row, ssSet.m_Elevation);
				m_grid.QuickSetNumber(17, row, ssSet.m_LatDegrees);

				m_grid.GetCell(18, row, &cell);
				tmp = ssSet.m_DormantJulian.Format("%m/%d");
				cell.SetText(tmp);
				m_grid.SetCell(18, row, &cell);

				m_grid.GetCell(19, row, &cell);
				cell.SetNumber(ssSet.m_UseDormant);
				m_grid.SetCell(19, row, &cell);

				m_grid.GetCell(20, row, &cell);
				cell.SetNumber(ssSet.m_UseStick);
				m_grid.SetCell(20, row, &cell);
				//if it's an NFDR2016 Fuel set SCM and MX_Humid
				if (isNFDRS2016(ssSet.m_NFDRSFM[0]))
				{
					int scmVal, mxVal;
					switch (ssSet.m_NFDRSFM[0])
					{
					case 'v':
					case 'V':
						scmVal = ssSet.m_SCM_V;
						mxVal = ssSet.m_MX_Humid_V;
						break;
					case 'w':
					case 'W':
						scmVal = ssSet.m_SCM_W;
						mxVal = ssSet.m_MX_Humid_W;
						break;
					case 'x':
					case 'X':
						scmVal = ssSet.m_SCM_X;
						mxVal = ssSet.m_MX_Humid_X;
						break;
					case 'y':
					case 'Y':
						scmVal = ssSet.m_SCM_Y;
						mxVal = ssSet.m_MX_Humid_Y;
						break;
					case 'z':
					case 'Z':
						scmVal = ssSet.m_SCM_Z;
						mxVal = ssSet.m_MX_Humid_Z;
						break;
					}
					m_grid.QuickSetNumber(21, row, scmVal);
					m_grid.QuickSetNumber(22, row, mxVal);
				}
			}
		}
		ssSet.Close();
	}
	m_grid.SetVisibleColumns();
	m_grid.RedrawAll();
	ConfigCheckBoxes();
	SetYears();
}

/* function added 2012  FFP4.1
   same basic functionality as onSelchangeSigstation()

   but this is meant to run on initialization, so the saved
   working set  start/end years are retained

*/
void CFireplusView::InitSigstation()
{
	// TODO: Add your control notification handler code here
	//must reset date frames to valid years based on new selection
	char temp[64], station[64];
	temp[0] = 0;
	if(!ignoreMetaTable)
		SaveMetaTable();
	ignoreMetaTable = false;
	int itemLoc = m_ctlSIG_Station.GetCurSel();// , yr1 = 2030, yr2 = 0;

	CUGCell cell;
	if(itemLoc != CB_ERR)
	{
		CWaitCursor wait;
		hasHourly = FALSE;

		m_ctlSIG_Station.GetLBText(itemLoc, station);
		CSIGStationSet ssSet(GetDocument()->m_pDB);
		ssSet.Open();
		m_grid.SetNumberRows(0);
		//set up string arrays for combobox columns...

		if(strncmp(station, "SIG", 3) == 0)//is a SIG
		{
			CStationInSIGSet sSet(GetDocument()->m_pDB);
			sprintf(temp, "[SIG] = '%-20.20s'", &station[6]);
			sSet.m_strFilter = _T(temp);
			sSet.Open();
			int row = 0;
			while(!sSet.IsEOF())
			{
				sprintf(temp, "[StationID] = '%6.6s'", sSet.m_StationID);
				ssSet.m_strFilter = _T(temp);
				ssSet.Requery();
				if(!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
				{
					if(ssSet.m_GreenJulian.m_status != COleDateTime::valid)
					{
						ssSet.Edit();
						ssSet.m_GreenJulian.SetDateTime(2008, 1, 1, 13, 0, 0);
						ssSet.Update();
					}
					if(ssSet.m_DormantJulian.m_status != COleDateTime::valid)
					{
						ssSet.Edit();
						ssSet.m_DormantJulian.SetDateTime(2008, 9, 15, 13, 0, 0);
						ssSet.Update();
					}
					if(ssSet.m_FreezeJulian.m_status != COleDateTime::valid)
					{
						ssSet.Edit();
						ssSet.m_FreezeJulian.SetDateTime(2008, 12, 31, 13, 0, 0);
						ssSet.Update();
					}
					m_grid.AppendRow();
					m_grid.QuickSetText(0, row, ssSet.m_StationID);
					m_grid.QuickSetText(1, row, ssSet.m_Name);
					m_grid.GetCell(2, row, &cell);
					if(ssSet.IsFieldNull(&ssSet.m_NFDRSFM))
					{
						strcpy(temp, " ");
					}
					else
						sprintf(temp, "%c - ", ssSet.m_NFDRSFM[0]);
					if(temp[0] != ' ')
					{
						CString list = c_strFModels;//cell.GetLabelText();//m_metaTable.GetColumnSet().GetItem(2).GetList();
						char *pStr = (char *)strstr(list, temp);
						int l = 0;
						while(pStr[l] != '\t' && pStr[l] != '\n' && pStr[l] != 0)
							temp[l] = pStr[l++];
						temp[l] = 0;
					}
					cell.SetText(temp);
					m_grid.SetCell(2, row, &cell);
					m_grid.GetCell(3, row, &cell);
					cell.SetNumber(ssSet.m_Use88);
					m_grid.SetCell(3, row, &cell);
					CString tmp;
					tmp.Format("%d", ssSet.m_SlopeCls);
					m_grid.QuickSetText(4, row, tmp);
					tmp.Format("%d", ssSet.m_ClimateCls);
					m_grid.QuickSetText(5, row, tmp);
					m_grid.GetCell(6, row, &cell);
					tmp = ssSet.m_GreenJulian.Format("%m/%d");
					cell.SetText(tmp);
					m_grid.SetCell(6, row, &cell);
					m_grid.GetCell(7, row, &cell);
					tmp = ssSet.m_FreezeJulian.Format("%m/%d");
					cell.SetText(tmp);
					m_grid.SetCell(7, row, &cell);
					m_grid.QuickSetNumber(8, row, ssSet.m_StartKBDI);
					m_grid.QuickSetNumber(9, row, ssSet.m_Start1000);
					m_grid.QuickSetNumber(10, row, ssSet.m_AvgPrecip);
					m_grid.QuickSetNumber(11, row, ssSet.m_FM1Eq10);
					m_grid.QuickSetNumber(12, row, ssSet.m_HerbAnnual);
					m_grid.QuickSetNumber(13, row, ssSet.m_Deciduous);
					tmp = ssSet.m_Aspect;
					m_grid.QuickSetText(14, row, tmp);
					tmp = ssSet.m_PsnOnSlope;
					m_grid.QuickSetText(15, row, tmp);
					m_grid.QuickSetNumber(16, row, ssSet.m_Elevation);
					m_grid.QuickSetNumber(17, row, ssSet.m_LatDegrees);

					m_grid.GetCell(18, row, &cell);
					tmp = ssSet.m_DormantJulian.Format("%m/%d");
					cell.SetText(tmp);
					m_grid.SetCell(18, row, &cell);

					m_grid.GetCell(19, row, &cell);
					cell.SetNumber(ssSet.m_UseDormant);
					m_grid.SetCell(19, row, &cell);

					m_grid.GetCell(20, row, &cell);
					cell.SetNumber(ssSet.m_UseStick);
					m_grid.SetCell(20, row, &cell);

					//if it's an NFDR2016 Fuel set SCM and MX_Humid
					if (isNFDRS2016(ssSet.m_NFDRSFM[0]))
					{
						int scmVal, mxVal;
						switch (ssSet.m_NFDRSFM[0])
						{
						case 'v':
						case 'V':
							scmVal = ssSet.m_SCM_V;
							mxVal = ssSet.m_MX_Humid_V;
							break;
						case 'w':
						case 'W':
							scmVal = ssSet.m_SCM_W;
							mxVal = ssSet.m_MX_Humid_W;
							break;
						case 'x':
						case 'X':
							scmVal = ssSet.m_SCM_X;
							mxVal = ssSet.m_MX_Humid_X;
							break;
						case 'y':
						case 'Y':
							scmVal = ssSet.m_SCM_Y;
							mxVal = ssSet.m_MX_Humid_Y;
							break;
						case 'z':
						case 'Z':
							scmVal = ssSet.m_SCM_Z;
							mxVal = ssSet.m_MX_Humid_Z;
							break;
						}
						m_grid.QuickSetNumber(21, row, scmVal);
						m_grid.QuickSetNumber(22, row, mxVal);
					}
				}
				//check for hourly data
				sSet.MoveNext();
				row++;
			}
			sSet.Close();



		}
		else
		{	//a single station
			sprintf(temp, "[StationID] = '%6.6s'", station);
			ssSet.m_strFilter = _T(temp);
			ssSet.Requery();

			if(!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
			{
				if(ssSet.m_GreenJulian.m_status != COleDateTime::valid)
				{
					ssSet.Edit();
					ssSet.m_GreenJulian.SetDateTime(2008, 1, 1, 13, 0, 0);
					ssSet.Update();
				}
				if(ssSet.m_DormantJulian.m_status != COleDateTime::valid)
				{
					ssSet.Edit();
					ssSet.m_DormantJulian.SetDateTime(2008, 9, 15, 13, 0, 0);
					ssSet.Update();
				}
				if(ssSet.m_FreezeJulian.m_status != COleDateTime::valid)
				{
					ssSet.Edit();
					ssSet.m_FreezeJulian.SetDateTime(2008, 12, 31, 13, 0, 0);
					ssSet.Update();
				}
				int row = 0;
				m_grid.AppendRow();
				m_grid.QuickSetText(0, row, ssSet.m_StationID);
				m_grid.QuickSetText(1, row, ssSet.m_Name);
				m_grid.GetCell(2, row, &cell);
				if(ssSet.IsFieldNull(&ssSet.m_NFDRSFM))
					strcpy(temp, " ");
				else
					sprintf(temp, "%c - ", ssSet.m_NFDRSFM[0]);
				if(temp[0] != ' ')
				{
					CString list = c_strFModels;//cell.GetLabelText();//m_metaTable.GetColumnSet().GetItem(2).GetList();
					char *pStr = (char *)strstr(list, temp);
					int l = 0;
					if (pStr != NULL)
					{
						while (pStr[l] != '\t' && pStr[l] != '\n' && pStr[l] != 0)
							temp[l] = pStr[l++];
					}
					temp[l] = 0;
				}
				cell.SetText(temp);
				m_grid.SetCell(2, row, &cell);
				m_grid.GetCell(3, row, &cell);
				cell.SetNumber(ssSet.m_Use88);
				m_grid.SetCell(3, row, &cell);
				CString tmp;
				tmp.Format("%d", ssSet.m_SlopeCls);
				m_grid.QuickSetText(4, row, tmp);
				tmp.Format("%d", ssSet.m_ClimateCls);
				m_grid.QuickSetText(5, row, tmp);
				m_grid.GetCell(6, row, &cell);
				tmp = ssSet.m_GreenJulian.Format("%m/%d");
				cell.SetText(tmp);
				m_grid.SetCell(6, row, &cell);
				m_grid.GetCell(7, row, &cell);
				tmp = ssSet.m_FreezeJulian.Format("%m/%d");
				cell.SetText(tmp);
				m_grid.SetCell(7, row, &cell);
				m_grid.QuickSetNumber(8, row, ssSet.m_StartKBDI);
				m_grid.QuickSetNumber(9, row, ssSet.m_Start1000);
				m_grid.QuickSetNumber(10, row, ssSet.m_AvgPrecip);
				m_grid.QuickSetNumber(11, row, ssSet.m_FM1Eq10);
				m_grid.QuickSetNumber(12, row, ssSet.m_HerbAnnual);
				m_grid.QuickSetNumber(13, row, ssSet.m_Deciduous);
				tmp = ssSet.m_Aspect;
				m_grid.QuickSetText(14, row, tmp);
				tmp = ssSet.m_PsnOnSlope;
				m_grid.QuickSetText(15, row, tmp);
				m_grid.QuickSetNumber(16, row, ssSet.m_Elevation);
				m_grid.QuickSetNumber(17, row, ssSet.m_LatDegrees);
				m_grid.GetCell(18, row, &cell);
				tmp = ssSet.m_DormantJulian.Format("%m/%d");
				cell.SetText(tmp);
				m_grid.SetCell(18, row, &cell);

				m_grid.GetCell(19, row, &cell);
				cell.SetNumber(ssSet.m_UseDormant);
				m_grid.SetCell(19, row, &cell);

				m_grid.GetCell(20, row, &cell);
				cell.SetNumber(ssSet.m_UseStick);
				m_grid.SetCell(20, row, &cell);
				//if it's an NFDR2016 Fuel set SCM and MX_Humid
				if (isNFDRS2016(ssSet.m_NFDRSFM[0]))
				{
					int scmVal, mxVal;
					switch (ssSet.m_NFDRSFM[0])
					{
					case 'v':
					case 'V':
						scmVal = ssSet.m_SCM_V;
						mxVal = ssSet.m_MX_Humid_V;
						break;
					case 'w':
					case 'W':
						scmVal = ssSet.m_SCM_W;
						mxVal = ssSet.m_MX_Humid_W;
						break;
					case 'x':
					case 'X':
						scmVal = ssSet.m_SCM_X;
						mxVal = ssSet.m_MX_Humid_X;
						break;
					case 'y':
					case 'Y':
						scmVal = ssSet.m_SCM_Y;
						mxVal = ssSet.m_MX_Humid_Y;
						break;
					case 'z':
					case 'Z':
						scmVal = ssSet.m_SCM_Z;
						mxVal = ssSet.m_MX_Humid_Z;
						break;
					}
					m_grid.QuickSetNumber(21, row, scmVal);
					m_grid.QuickSetNumber(22, row, mxVal);
				}
			}
		}
		ssSet.Close();
	}
	m_grid.SetVisibleColumns();
	m_grid.RedrawAll();
	ConfigCheckBoxes();
	SetYears();
}

void CFireplusView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	if (UpdateOnDeactivate)
		if(!bActivate)
			OnFileSave();
	theApp.dbDir = GetDocument()->docDir;
	CRecordView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CFireplusView::OnSigs()
{
	OnFileSave();
	CSIGSet sigSet(GetDocument()->m_pDB);
	sigSet.Open();
	CSIGSDialog sigsDialog(this, &sigSet, m_pSet);
	sigsDialog.DoModal();
	sigSet.Close();
	//need to update SIG/Station combo box
	ConfigureStationList();
	OnSelchangeSigstation();
}

void CFireplusView::SaveMetaTable()
{
	//write metadata fields to wxStation
	CWaitCursor wait;
	char temp[64], station[8];
	//m_Grid.EndEditing();
	//m_metaTable.GetCurCell().EndEdit();
	CSIGStationSet ssSet(GetDocument()->m_pDB);
	ssSet.Open();
	short iVal;
	float fVal;
	double dVal;

	CString str;
	CUGCell cell;
	if(ssSet.IsOpen())
	{
		try
		{    // requery recordset
			//for(int rc = 0; rc < m_metaTable.GetRowSet().GetCount(); rc++)

			for(int rc = 0; rc < m_grid.GetNumberRows(); rc++)
			{
				CString tmp;
				tmp = m_grid.QuickGetText(0, rc);
				//CDTData RowData = m_metaTable.GetRowSet().GetItem(rc).GetValue();
				//RowData.GetItem(station, 0);
				strcpy(station, tmp);
				sprintf(temp, "[StationID] = '%6.6s'", station);
				ssSet.m_strFilter = _T(temp);
				ssSet.Requery();
				if(!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
				{
					ssSet.Edit();
					ssSet.m_Name = m_grid.QuickGetText(1, rc);//m_Grid.GetCell(rc, 1)->GetText();
					//ssSet.m_Name = tmp
					//RowData.GetItem(ssSet.m_Name, 1);
					tmp = m_grid.QuickGetText(2, rc);//m_Grid.GetCell(rc, 2)->GetText();
					strcpy(temp, tmp);
					//RowData.GetItem(temp, 2);
					temp[1] = 0;
					if(strncmp(ssSet.m_NFDRSFM, temp, 1) != 0)
						fxParamChange = true;
					ssSet.m_NFDRSFM = temp;
					//RowData.GetItem(&iVal, 3);
					//CGridCellCheck *pCellCheck = (CGridCellCheck*) m_Grid.GetCell(rc, 3);
					m_grid.GetCell(3, rc, &cell);
					iVal = cell.GetNumber();//pCellCheck->GetCheck();
					if(ssSet.m_Use88 != iVal)
						fxParamChange = true;
					ssSet.m_Use88 = iVal;
					//iVal = atoi(m_Grid.GetCell(rc, 4)->GetText());
					iVal = atoi(m_grid.QuickGetText(4, rc));
					//RowData.GetItem(&iVal, 4);
					if(ssSet.m_SlopeCls != iVal)
						fxParamChange = true;
					ssSet.m_SlopeCls = iVal;
					iVal = atoi(m_grid.QuickGetText(5, rc));
					//RowData.GetItem(&iVal, 5);
					if(ssSet.m_ClimateCls != iVal)
						fxParamChange = true;
					ssSet.m_ClimateCls = iVal;
					//CTime tTime;
					int second, minute, hour, day, month, year;
					m_grid.GetCell(6, rc, &cell);
					cell.GetTime(&second, &minute, &hour, &day, &month, &year);
					COleDateTime tTime(year, month, day, hour, minute, second);
					//CTime *cTime;
					//CGridCellDateTime* pDateCell = (CGridCellDateTime*) m_Grid.GetCell(rc,6);
					//RowData.GetItem(tTime, 6);
					//cTime = pDateCell->GetTime();
					//tTime.SetDateTime(cTime->GetYear(), cTime->GetMonth(), cTime->GetDay(),
						//cTime->GetHour(), cTime->GetMinute(), cTime->GetSecond());
					if(ssSet.m_GreenJulian != tTime)
						fxParamChange = true;
					ssSet.m_GreenJulian = tTime;
					//pDateCell = (CGridCellDateTime*) m_Grid.GetCell(rc,7);
					//cTime = pDateCell->GetTime();
					//tTime.SetDateTime(cTime->GetYear(), cTime->GetMonth(), cTime->GetDay(),
					//	cTime->GetHour(), cTime->GetMinute(), cTime->GetSecond());
					//RowData.GetItem(tTime, 7);
					m_grid.GetCell(7, rc, &cell);
					cell.GetTime(&second, &minute, &hour, &day, &month, &year);
					COleDateTime tTime2(year, month, day, hour, minute, second);
					//tTime.SetDateTime(year, month, day, hour, minute, second);
					if(ssSet.m_FreezeJulian != tTime2)
						fxParamChange = true;
					ssSet.m_FreezeJulian = tTime2;
					//CGridCellNumeric *pNumCell = (CGridCellNumeric *)m_Grid.GetCell(rc, 8);
					m_grid.GetCell(8, rc, &cell);
					iVal = cell.GetNumber();
					//RowData.GetItem(&iVal, 8);
					//iVal = (short)pNumCell->GetNumber();
					if(ssSet.m_StartKBDI != iVal)
						fxParamChange = true;
					ssSet.m_StartKBDI = iVal;
					//pNumCell = (CGridCellNumeric *)m_Grid.GetCell(rc, 9);
					m_grid.GetCell(9, rc, &cell);
					fVal = cell.GetNumber();
					//fVal = pNumCell->GetNumber();
					//RowData.GetItem(&fVal, 9);
					if(ssSet.m_Start1000 != fVal)
						fxParamChange = true;
					ssSet.m_Start1000 = fVal;
					//pNumCell = (CGridCellNumeric *)m_Grid.GetCell(rc, 10);
					m_grid.GetCell(10, rc, &cell);
					fVal = cell.GetNumber();
					//fVal = pNumCell->GetNumber();
					//RowData.GetItem(&fVal, 10);
					if(ssSet.m_AvgPrecip != fVal)
						fxParamChange = true;
					ssSet.m_AvgPrecip = fVal;
					//pCellCheck = (CGridCellCheck*) m_Grid.GetCell(rc, 11);
					m_grid.GetCell(11, rc, &cell);
					iVal = cell.GetNumber();
					//iVal = pCellCheck->GetCheck();
					//RowData.GetItem(&iVal, 11);
					if(ssSet.m_FM1Eq10 != iVal)
						fxParamChange = true;
					ssSet.m_FM1Eq10 = iVal;
					//pCellCheck = (CGridCellCheck*) m_Grid.GetCell(rc, 12);
					m_grid.GetCell(12, rc, &cell);
					iVal = cell.GetNumber();
					//iVal = pCellCheck->GetCheck();
					//RowData.GetItem(&iVal, 12);
					if(ssSet.m_HerbAnnual != iVal)
						fxParamChange = true;
					ssSet.m_HerbAnnual = iVal;
					//RowData.GetItem(&iVal, 13);
					//pCellCheck = (CGridCellCheck*) m_Grid.GetCell(rc, 13);
					m_grid.GetCell(13, rc, &cell);
					iVal = cell.GetNumber();
					//iVal = pCellCheck->GetCheck();
					if(ssSet.m_Deciduous != iVal)
						fxParamChange = true;
					ssSet.m_Deciduous = iVal;
					str = m_grid.QuickGetText(14, rc);//m_Grid.GetCell(rc, 14)->GetText();
					//RowData.GetItem(str, 14);
					if(str.Compare(ssSet.m_Aspect) != 0)
						fxParamChange = true;
					ssSet.m_Aspect = str;
					str = m_grid.QuickGetText(15, rc);//m_Grid.GetCell(rc, 15)->GetText();
					//RowData.GetItem(str, 15);
					if(str.Compare(ssSet.m_PsnOnSlope) != 0)
						fxParamChange = true;
					//pNumCell = (CGridCellNumeric *)m_Grid.GetCell(rc, 16);
					m_grid.GetCell(16, rc, &cell);
					iVal = cell.GetNumber();
					//iVal = pNumCell->GetNumber();
					//RowData.GetItem(&iVal, 16);
					if(ssSet.m_Elevation != iVal)
						fxParamChange = true;
					ssSet.m_Elevation = iVal;
					m_grid.GetCell(17, rc, &cell);
					dVal = cell.GetNumber();
					//pNumCell = (CGridCellNumeric *)m_Grid.GetCell(rc, 17);
					//iVal = pNumCell->GetNumber();
					//RowData.GetItem(&iVal, 17);
					if(ssSet.m_LatDegrees != dVal)
						fxParamChange = true;
					ssSet.m_LatDegrees = dVal;

					m_grid.GetCell(18, rc, &cell);
					cell.GetTime(&second, &minute, &hour, &day, &month, &year);
					COleDateTime dTime(year, month, day, hour, minute, second);

					if(ssSet.m_DormantJulian != dTime)
						fxParamChange = true;
					ssSet.m_DormantJulian = dTime;

					m_grid.GetCell(19, rc, &cell);
					iVal = cell.GetNumber();//pCellCheck->GetCheck();
					if(ssSet.m_UseDormant != iVal)
						fxParamChange = true;
					ssSet.m_UseDormant = iVal;

					m_grid.GetCell(20, rc, &cell);
					iVal = cell.GetNumber();//pCellCheck->GetCheck();
					if(ssSet.m_UseStick != iVal)
						fxParamChange = true;
					ssSet.m_UseStick = iVal;

					//if NFDRS2016 fuel model, need to save SCM_? and MX_Humid_? for station's selected fuel model
					BOOL bVal;
					if (isNFDRS2016(ssSet.m_NFDRSFM[0]))
					{
						m_grid.GetCell(21, rc, &cell);
						iVal = cell.GetNumber();//pCellCheck->GetCheck();
						m_grid.GetCell(22, rc, &cell);
						bVal = cell.GetNumber();

						switch (ssSet.m_NFDRSFM[0])
						{
						case 'v':
						case 'V':
							ssSet.m_SCM_V = iVal;
							ssSet.m_MX_Humid_V = bVal;
							break;
						case 'w':
						case 'W':
							ssSet.m_SCM_W = iVal;
							ssSet.m_MX_Humid_W = bVal;
							break;
						case 'x':
						case 'X':
							ssSet.m_SCM_X = iVal;
							ssSet.m_MX_Humid_X = bVal;
							break;
						case 'y':
						case 'Y':
							ssSet.m_MX_Humid_Y = bVal;
							ssSet.m_SCM_Y = iVal;
							break;
						case 'z':
						case 'Z':
							ssSet.m_SCM_Z = iVal;
							ssSet.m_MX_Humid_Z = bVal;
							break;
						}
					}
					ssSet.Update();
				}
			}
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
	ssSet.Close();
}
void  CFireplusView::ConfigCheckBoxes()
{
	bool allNFDRS2016 = true;
	CString str;
	for (int r = 0; r < m_grid.GetNumberRows(); r++)
	{
		str = m_grid.QuickGetText(2, r);
		if (!isNFDRS2016(str[0]))
		{
			allNFDRS2016 = false;
			break;
		}
	}
	m_chkForceRecompute.EnableWindow(allNFDRS2016);
	m_chkUSeDailyExtremes.EnableWindow(allNFDRS2016);
}

/*void CFireplusView::SaveNFDRS2016MetaData(int row)
{
	char fm[64], station[8], temp[64];
	CString tmp;
	tmp = m_grid.QuickGetText(2, row);
	strcpy(fm, tmp);
	if (strlen(fm) > 0 && isNFDRS2016(fm[0]))
	{
		tmp = m_grid.QuickGetText(0, row);
		strcpy(station, tmp);
		sprintf(temp, "[StationID] = '%s'", station);
		CSIGStationSet ssSet(GetDocument()->m_pDB);
		ssSet.m_strFilter = _T(temp);
		ssSet.Open();
		CUGCell cell;
		if (!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
		{
			ssSet.Edit();
			m_grid.GetCell(21, row, &cell);
			int iVal = cell.GetNumber();//pCellCheck->GetCheck();
			m_grid.GetCell(22, row, &cell);
			BOOL bVal = cell.GetNumber();
			switch (fm[0])
			{
			case 'v':
			case 'V':
				ssSet.m_SCM_V = iVal;
				ssSet.m_MX_Humid_V = bVal;
				break;
			case 'w':
			case 'W':
				ssSet.m_SCM_W = iVal;
				ssSet.m_MX_Humid_W = bVal;
				break;
			case 'x':
			case 'X':
				ssSet.m_SCM_X = iVal;
				ssSet.m_MX_Humid_X = bVal;
				break;
			case 'y':
			case 'Y':
				ssSet.m_SCM_Y = iVal;
				ssSet.m_MX_Humid_Y = bVal;
				break;
			case 'z':
			case 'Z':
				ssSet.m_SCM_Z = iVal;
				ssSet.m_MX_Humid_Z = bVal;
				break;
			}
			ssSet.Update();
		}
		ssSet.Close();
	}
}

void CFireplusView::LoadNFDRS2016MetaData(int row)
{
	char fm[64], station[8], temp[64];
	CString tmp;
	tmp = m_grid.QuickGetText(2, row);
	strcpy(fm, tmp);
	if (strlen(fm) > 0 && isNFDRS2016(fm[0]))
	{
		tmp = m_grid.QuickGetText(0, row);
		strcpy(station, tmp);
		sprintf(temp, "[StationID] = '%s'", station);
		CSIGStationSet ssSet(GetDocument()->m_pDB);
		ssSet.m_strFilter = _T(temp);
		ssSet.Open();
		CUGCell cell;
		if (!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
		{
			int iVal;
			BOOL bVal;
			switch (fm[0])
			{
			case 'v':
			case 'V':
				iVal = ssSet.m_SCM_V;
				bVal = ssSet.m_MX_Humid_V;
				break;
			case 'w':
			case 'W':
				iVal = ssSet.m_SCM_W;
				bVal = ssSet.m_MX_Humid_W;
				break;
			case 'x':
			case 'X':
				iVal = ssSet.m_SCM_X;
				bVal = ssSet.m_MX_Humid_X;
				break;
			case 'y':
			case 'Y':
				iVal = ssSet.m_SCM_Y;
				bVal = ssSet.m_MX_Humid_Y;
				break;
			case 'z':
			case 'Z':
				iVal = ssSet.m_SCM_Z;
				bVal = ssSet.m_MX_Humid_Z;
				break;
			}
			m_grid.QuickSetNumber(21, row, iVal);
			m_grid.QuickSetNumber(22, row, bVal);
		}
		ssSet.Close();
	}

}*/

void CFireplusView::OnRerap()
{
	// TODO: Add your command handler code here
	if(!FieldsOK())
		return;
	OnFileSave();
	if(m_pSet->m_SIG_Station.GetLength() > 6)
	{
		AfxMessageBox("ERROR: Rerap analysis can not be done with SIGs.\n");
		return;
	}
	CRerapVarDialog rvd(this);
	if(rvd.DoModal() == IDOK)
	{
		long varID = rvd.varID;
		AfxGetMainWnd()->EnableWindow(FALSE);

		// added for batch
		// single run mode
        CString runDesc;
		runDesc.Format("Percentile Weather: ");

        CClimateSet cSet(m_pSet->m_pDatabase);
	    cSet.m_strFilter.Format("[VarID]=%d",varID);

		cSet.Open();
		if (cSet.IsOpen() && !cSet.IsEOF())
		{
				runDesc.Append(cSet.m_ShortName);
		}
		cSet.Close();

		CRunSave saveRun(m_pSet);
		saveRun.SaveGeneral(0, runDesc , m_pSet);
		saveRun.SaveClimateOptions(curr_runID, m_pSet);
		//saveRun.SaveWxStation(curr_runID, m_pSet);
		saveRun.SaveFireOptions(curr_runID, m_pSet);

		CString value1;

		// var
		value1.Format("%2d", varID);
		saveRun.SaveRunOption(curr_runID,  REPORT_WEATHER_PERCENTILE_WEATHER, 0, "varID", value1, m_pSet);

		// end single run mode
		// end added for batch

		CRerapDialog rd(this, m_pSet, varID);
		BOOL ret = rd.ReadData();
		AfxGetMainWnd()->EnableWindow(TRUE);
		if(ret)
			rd.DoModal();
	}}

void CFireplusView::OnWeatherPocketcard()
{
	// TODO: Add your command handler code here
	if(!FieldsOK())
		return;
	OnFileSave();
	CPocketCardSet pocketSet(m_pSet->m_pDatabase);
	pocketSet.OpenEnsureSIGStationRec(m_pSet->m_SIG_Station);
	CPocketCardDialog dlg(this, m_pSet, &pocketSet);
	if(IDOK == dlg.DoModal())
	{
		m_pSet->Edit();
		m_pSet->m_PeriodLength = "1";
		m_pSet->Update();
		//analyze the data
		CClimAnalysis *analysis = new CClimAnalysis(m_pSet->m_pDatabase);
		analysis->fpSet = m_pSet;
		int *varIDs = new int[1];
		varIDs[0] = dlg.m_varID;//dlg.m_IndexBox.GetCurSel();
		//EnableWindow(FALSE);
		int ret = analysis->RunAnalyze(varIDs, 1, m_pSet, false);
			//analysis->Analyze(this, varIDs, 1, m_pSet, false);
		//EnableWindow(TRUE);
		if(ret != IDOK)
			return;
		//clean up fires
		analysis->KillFires();
		Period ***ytdPeriods = NULL;
		analysis->LoadPocketOptions(&pocketSet, dlg.m_varID);//dlg.m_IndexBox.GetCurSel());
		if(dlg.yearToDate)
		{
			COleDateTime today;
			today = COleDateTime::GetCurrentTime();
			int curYear = today.GetYear(), oldSyear = m_pSet->m_StartYear, oldEyear = m_pSet->m_EndYear;
			m_pSet->Edit();
			m_pSet->m_StartYear = m_pSet->m_EndYear = curYear;
			m_pSet->Update();
			CClimAnalysis tanalysis(m_pSet->m_pDatabase);
			int *tvarIDs = new int[1];
			tvarIDs[0] = dlg.m_varID;//dlg.m_IndexBox.GetCurSel();
			tanalysis.checkFires = false;
			//EnableWindow(FALSE);
			ret = tanalysis.RunAnalyze(tvarIDs, 1, m_pSet, false);
				//Analyze(this, tvarIDs, 1, m_pSet, false);
			//EnableWindow(TRUE);
			if(ret != IDOK)
				return;
			//clean up fires
			tanalysis.KillFires();
			tanalysis.killPeriods = false;
			ytdPeriods = tanalysis.periods;
			m_pSet->Edit();
			m_pSet->m_StartYear = oldSyear;
			m_pSet->m_EndYear = oldEyear;
			m_pSet->Update();
			analysis->m_pocketOpts.ytdPeriods = ytdPeriods;
		}
		//now display the pocket card in a child window
		POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate =
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("PocketCard"))
			{
				CPocketCardDoc *pDoc = (CPocketCardDoc *)curTemplate->CreateNewDocument();
				//set PocketCardDoc members
				pDoc->m_pAnalysis = analysis;
				CPocketCardFrame* pFrame = (CPocketCardFrame*)curTemplate->CreateNewFrame(pDoc, NULL);
				CPocketCardView *pView = (CPocketCardView *)pFrame->GetWindow(GW_CHILD);
				pView->m_pAnalysis = pDoc->m_pAnalysis;
				curTemplate->InitialUpdateFrame(pFrame, pDoc);
				CString tTitle;
				tTitle.Format("%s Pocket Card", pocketSet.m_FireDangerArea);
				pDoc->SetTitle(tTitle);
				break;
			}
		}
	}
	pocketSet.Close();
}

void CFireplusView::OnWeatherSeasonreportsPersistenceprobability()
{
	// TODO: Add your command handler code here
	if(!FieldsOK())
		return;
	OnFileSave();
	CPProbDialog cpd(this, m_pSet);
	cpd.DoModal();
}

void CFireplusView::OnWeatherSeasonreportsProbabilityanalysis()
{
	// TODO: Add your command handler code here
	if(!FieldsOK())
		return;
	OnFileSave();
	CPAnalysisDialog cpd(this, m_pSet);
	cpd.DoModal();
}

void CFireplusView::OnWeatherSeasonreportsSeveritylist()
{
	// TODO: Add your command handler code here
	if(!FieldsOK())
		return;
	OnFileSave();
	CSeverityDialog csd(this, m_pSet, 0);
	csd.DoModal();
}

void CFireplusView::OnWeatherSeasonreportsSeveritysummary()
{
	// TODO: Add your command handler code here
	if(!FieldsOK())
		return;
	OnFileSave();
	CSeverityDialog csd(this, m_pSet, 1);
	csd.DoModal();
}

void CFireplusView::OnFiresSummaryWorkingset()
{
	// TODO: Add your command handler code here
	if(!FieldsOK())
		return;
	OnFileSave();
	// added for batch
	// single run mode
	CRunSave saveRun(m_pSet);
	saveRun.SaveGeneral(0, "Fires Summary" , m_pSet);
	saveRun.SaveClimateOptions(curr_runID, m_pSet);
	//saveRun.SaveWxStation(curr_runID, m_pSet);
	saveRun.SaveFireOptions(curr_runID, m_pSet);
	// end added for batch
	CString tTitle, staStr;
	staStr = m_pSet->m_SIG_Station;
	staStr.TrimRight();
	if(staStr.GetLength() > 6)
		tTitle.Format("Select Fires for Summary for %s", staStr);
	else
		tTitle.Format("Select Fires for Summary for Station %s", staStr);
	CFireQuerySheet sheet(tTitle, this, 0, m_pSet->m_pDatabase, m_pSet->m_SIG_Station,
		NULL, NULL, m_pSet,m_pSet);
	if(sheet.DoModal() == IDOK)
	{
		//AfxGetMainWnd()->EnableWindow(FALSE);
		CFireSummary *fSum = new CFireSummary();
		int firesOK = fSum->AddFires(m_pSet->m_pDatabase, m_pSet);
		//AfxGetMainWnd()->EnableWindow(TRUE);
		if(firesOK == 0)
		{//create and add a FireSumDoc
			POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

			while(curTemplatePos != NULL)
			{
				CDocTemplate* curTemplate =
					theApp.GetNextDocTemplate(curTemplatePos);
				CString str;
				curTemplate->GetDocString(str, CDocTemplate::docName);
				if(str == _T("FireSummary"))
				{
					CFireSumDoc *fDoc = (CFireSumDoc *)curTemplate->CreateNewDocument();
					fDoc->summary = fSum;
					//CString tTitle;
					tTitle.Format("Fire Summary for %s", staStr);
					fDoc->SetTitle(tTitle);
					CFireSumWnd* pFrame = (CFireSumWnd*)curTemplate->CreateNewFrame(fDoc, NULL);
					POSITION pos = fDoc->GetFirstViewPosition();
					CFireSumView *pNewView = (CFireSumView *)fDoc->GetNextView(pos);//->ShowWindow(SW_SHOW);//A->Invalidate();
					pNewView->m_pFiresSummary = fSum;
					curTemplate->InitialUpdateFrame(pFrame, fDoc);
					return;
				}
			}
		}
		else if(firesOK > 0)
		{
			CString strMsg;
			strMsg.Format(_T("%s"), fireErrorStrings[firesOK - 1]);
			AfxMessageBox(strMsg);
		}
		else
			AfxMessageBox("Fire Summary cancelled.\n");
		delete fSum;
	}
}

void CFireplusView::OnFiresFirebusiness()
{
	// TODO: Add your command handler code here
	//str.Format("Select Fires for Summary");
	COleDateTime today;
	today = COleDateTime::GetCurrentTime();
	int start = 1900, end = today.GetYear();
	// added for batch
	// single run mode
	CRunSave saveRun(m_pSet);
	saveRun.SaveGeneral(0, "Fires Summary" , m_pSet);
	saveRun.SaveClimateOptions(curr_runID, m_pSet);
	//saveRun.SaveWxStation(curr_runID, m_pSet);
	saveRun.SaveFireOptions(curr_runID, m_pSet);
	// end added for batch
	CFireQuerySheet sheet("Select Fires and Years for General Summary", this, 0, m_pSet->m_pDatabase, generalAssociation, &start, &end,NULL,m_pSet);
	if(sheet.DoModal() == IDOK)
	{
		if(start > end)
		{
			int swp = start;
			start = end;
			end = swp;
		}
		//AfxGetMainWnd()->EnableWindow(FALSE);
		CFireSummary *fSum = new CFireSummary();
		int firesOK = fSum->AddFires(m_pSet->m_pDatabase, generalAssociation, start, end);
		//AfxGetMainWnd()->EnableWindow(TRUE);
		if(firesOK == 0)
		{//create and add a FireSumDoc
			POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

			while(curTemplatePos != NULL)
			{
				CDocTemplate* curTemplate =
					theApp.GetNextDocTemplate(curTemplatePos);
				CString str;
				curTemplate->GetDocString(str, CDocTemplate::docName);
				if(str == _T("FireSummary"))
				{
					CFireSumDoc *fDoc = (CFireSumDoc *)curTemplate->CreateNewDocument();
					fDoc->summary = fSum;
					CString tTitle;
					tTitle.Format("Fire Summary");
					fDoc->SetTitle(tTitle);
					CFireSumWnd* pFrame = (CFireSumWnd*)curTemplate->CreateNewFrame(fDoc, NULL);
					POSITION pos = fDoc->GetFirstViewPosition();
					CFireSumView *pNewView = (CFireSumView *)fDoc->GetNextView(pos);//->ShowWindow(SW_SHOW);//A->Invalidate();
					pNewView->m_pFiresSummary = fSum;
					curTemplate->InitialUpdateFrame(pFrame, fDoc);
					return;
				}
			}
		}
		else if(firesOK > 0)
		{
			CString strMsg;
			strMsg.Format(_T("%s"), fireErrorStrings[firesOK - 1]);
			AfxMessageBox(strMsg);
		}
		else
			AfxMessageBox("Fire Summary cancelled.\n");
		delete fSum;
	}
}

bool CFireplusView::FieldsOK()
{
	int itemLoc = m_ctlSIG_Station.GetCurSel();
	if(itemLoc == CB_ERR)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: No SIG or Station selected for Active Working Set.\n\n"));
		AfxMessageBox(strMsg);
		return false;
	}
	CString txt;
	int sYear, eYear, sDay, eDay;
	m_StartYear.GetWindowText(txt);
	sYear = atoi(txt);
	if(txt.GetLength() == 0 || sYear == 0)
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: Start year can not zero or blank.\n\n");
		AfxMessageBox(strMsg);
		return false;
	}
	m_EndYear.GetWindowText(txt);
	eYear = atoi(txt);
	if(txt.GetLength() == 0 || eYear == 0)
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: End year can not zero or blank.\n\n");
		AfxMessageBox(strMsg);
		return false;
	}
	if(sYear > eYear)
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: End year can not be less then Start Year.\n\n");
		AfxMessageBox(strMsg);
		return false;
	}
	int sMin, sMax;
	m_spnEyear.GetRange(sMin, sMax);
	if(sYear < sMin || sYear > sMax)
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: Start year must be from %d - %d\n\n", sMin, sMax);
		AfxMessageBox(strMsg);
		return false;
	}
	if(eYear < sMin || eYear > sMax)
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: End year must be from %d - %d\n\n", sMin, sMax);
		AfxMessageBox(strMsg);
		return false;
	}
	//OnFileSave();
	//check other fields for blanks
	itemLoc = m_PeriodLenControl.GetCurSel();
	if(itemLoc == CB_ERR)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Analysis period length can not be blank.\n\n"));
		AfxMessageBox(strMsg);
		return false;
	}
	itemLoc = m_StartMonth.GetCurSel();
	if(itemLoc == CB_ERR)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Start month can not be blank.\n\n"));
		AfxMessageBox(strMsg);
		return false;
	}
	itemLoc = m_EndMonth.GetCurSel();
	if(itemLoc == CB_ERR)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: End month can not be blank.\n\n"));
		AfxMessageBox(strMsg);
		return false;
	}
	m_StartDay.GetWindowText(txt);
	sDay = atoi(txt);
	if(txt.GetLength() == 0 || sDay == 0)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Start day can not be zero or blank.\n\n"));
		AfxMessageBox(strMsg);
		return false;
	}
	else if(sDay > 31)
	{
		AfxMessageBox(_T("ERROR: Start day can not be greater than 31.\n\n"));
		return false;
	}
	m_EndDay.GetWindowText(txt);
	eDay = atoi(txt);
	if(txt.GetLength() == 0 || eDay == 0)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: End day can not be zero or blank.\n\n"));
		AfxMessageBox(strMsg);
		return false;
	}
	else if(eDay > 31)
	{
		AfxMessageBox(_T("ERROR: End day can not be greater than 31.\n\n"));
		return false;
	}
	//now check metedata in table
	CUGCell cell;
	for(int rc = 0; rc < m_grid.GetNumberRows(); rc++)
	{
		CString str, msg, sta;
		char temp[64];
		short iVal;
		//CDTData RowData = m_metaTable.GetRowSet().GetItem(rc).GetValue();
		sta = m_grid.QuickGetText(0, rc);//m_Grid.GetCell(rc, 0)->GetText();
		//RowData.GetItem(sta, 0);
		//RowData.GetItem(str, 1);
		str = m_grid.QuickGetText(1, rc);//m_Grid.GetCell(rc, 1)->GetText();
		str.TrimLeft();
		str.TrimRight();
		if(str.GetLength() <= 0)
		{
			msg.Format("ERROR: No name for Station %s!", sta);
			AfxMessageBox(msg);
			return false;
		}
		str = m_grid.QuickGetText(2, rc);//m_Grid.GetCell(rc, 2)->GetText();
		//RowData.GetItem(temp, 2);
		strcpy(temp, str);
		temp[1] = 0;
		if(temp[0] < 'A' || temp[0] > 'Z')
		{
			msg.Format("ERROR: No NFDRS fuel model for Station %s!", sta);
			AfxMessageBox(msg);
			return false;
		}
		//RowData.GetItem(&iVal, 4);
		//m_grid.GetCell(4, rc, &cell);
		//iVal = cell.GetNumber();
		iVal = atoi(m_grid.QuickGetText(4, rc));
		if(iVal <= 0 || iVal >= 6)
		{
			msg.Format("ERROR: No slope class for Station %s!", sta);
			AfxMessageBox(msg);
			return false;
		}
		//m_grid.GetCell(4, rc, &cell);
		//iVal = cell.GetNumber();
		iVal = atoi(m_grid.QuickGetText(5, rc));
		//RowData.GetItem(&iVal, 5);
		if(iVal < 1 || iVal > 4)
		{
			msg.Format("ERROR: No climate class for Station %s!", sta);
			AfxMessageBox(msg);
			return false;
		}
	}
	return true;
}

void CFireplusView::ConfigureStationList()
{
	m_ctlSIG_Station.ResetContent();
	//fill SIG/Station combobox...
	//int loc = 0;
//	CQueryDef wxStations(GetDocument()->m_pDB);
//	wxStations.Open("DistinctStation");
//	CString sqlStr = wxStations.GetSQL();
//	AfxMessageBox(sqlStr);
	//StationsUsed stnsUsed(GetDocument()->m_pDB);
	//stnsUsed.Open(CRecordset::snapshot, "{call DistinctStation}");
	//CSIGStationSet ssSet(GetDocument()->m_pDB);
	//ssSet.Open();
	CDatabase pDB;
	CSIGStationSet staSet(m_pSet->m_pDatabase);
	staSet.Open();
	CWxStationSet usedStations(m_pSet->m_pDatabase);
	usedStations.Open(CRecordset::dynaset, "SELECT DISTINCT WXObs.StationID FROM WXObs   ORDER BY WXObs.StationID;");

	if (usedStations.IsOpen())
	{
		while (!usedStations.IsEOF())
		{
			CString str = usedStations.m_StationID;
			str.TrimRight();
			CString sta;
			
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", str);
			staSet.Requery();
			str += " - " + staSet.m_Name;
			int loc = m_ctlSIG_Station.AddString(str);
			m_ctlSIG_Station.SetItemData(loc, 0);
			if(usedStations.m_StationID == m_pSet->m_SIG_Station)
				m_ctlSIG_Station.SetCurSel(loc);
			usedStations.MoveNext();
			//loc++;
		}
	}
	usedStations.Close();
//	wxStations.Close();
	if(HasUserTables(GetDocument()->m_pDB))
	{
		CRecordset userStations(GetDocument()->m_pDB);
		userStations.Open(CRecordset::dynaset, "SELECT DISTINCT ffpUserVals.StationID FROM ffpUserVals ORDER BY ffpUserVals.StationID;");
		while(!userStations.IsEOF())
		{
			CString str;
			CDBVariant var;
			userStations.GetFieldValue("StationID", var);
			//str = var.pcVal;
			str = *var.m_pstring;
			str.TrimRight();
			if(m_ctlSIG_Station.FindString(-1, str) == CB_ERR)
			{
				staSet.m_strFilter.Format("[StationID] = '%6.6s'", str);
				staSet.Requery();
				CString str2 = str +" - " + staSet.m_Name;
				//str += " - " + staSet.m_Name;
				int loc = m_ctlSIG_Station.AddString(str2);
				m_ctlSIG_Station.SetItemData(loc, 0);
				if(str == m_pSet->m_SIG_Station)
					m_ctlSIG_Station.SetCurSel(loc);
			}
			userStations.MoveNext();
		}
		userStations.Close();
	}
	staSet.Close();
	//add SIGS
	CSIGSet sigSet(GetDocument()->m_pDB);
	sigSet.Open();
	if (sigSet.IsOpen())
	{
		while (!sigSet.IsEOF())
		{
			CString str = "SIG - " + sigSet.m_SIG;
			int loc = m_ctlSIG_Station.AddString(str);
			m_ctlSIG_Station.SetItemData(loc, 1);
			if(str == m_pSet->m_SIG_Station)
				m_ctlSIG_Station.SetCurSel(loc);
			sigSet.MoveNext();
		}
	}
	sigSet.Close();
}

void CFireplusView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: Add your specialized code here and/or call the base class
	//do nothing....
	///CDaoRecordView::OnPrint(pDC, pInfo);
}

void CFireplusView::OnWeatherSeasonreportsDailylisting()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CListingDialog cld(this, m_pSet, TRUE);
	if(cld.DoModal() == IDOK)
	{
		int nVars = 0, *varIDs;
		CClimateSet climSet(m_pSet->m_pDatabase);
		climSet.m_strSort = "[Daily List]";
		climSet.m_strFilter = "[OptionType] >= 0 AND [Daily List] > 0";
		climSet.Open();
		while(!climSet.IsEOF())
		{
			nVars++;
			climSet.MoveNext();
		}
		if(nVars > 0)
		{
			varIDs = new int[nVars];
			climSet.MoveFirst();
			int loc = 0;
			while(!climSet.IsEOF())
			{
				varIDs[loc] = climSet.m_VarID - 1;
				loc++;
				climSet.MoveNext();
			}
			climSet.Close();
			CString oldLen = m_pSet->m_PeriodLength;
			m_pSet->m_PeriodLength = "1";
			CClimAnalysis analysis(m_pSet->m_pDatabase);
			analysis.checkFires = false;
			int ret = analysis.RunAnalyze(varIDs, nVars, m_pSet, false);
				//.Analyze(this, varIDs, nVars, m_pSet, false);
			bool createTable = false;
			// check to see if we need to create the DL_Ouput table
			CFFPOptsSet optsSet1(m_pSet->m_pDatabase);
			optsSet1.Open();
			if(!optsSet1.IsEOF())
			{			
				if (optsSet1.m_DLExport >= 1)
					createTable = true;			
			}
			optsSet1.Close();
			if(ret == IDOK)
				analysis.DailyListing(&theApp.lo,true,createTable);
			m_pSet->m_PeriodLength = oldLen;
		}
		else
		{
			climSet.Close();
		}
	}
}
// LJS 8-15-2003
// The following function is for the FPAAnalysis
// FIXME: put this structure def in appropriate .h
struct fpa_weather {
		int datestamp, month, day;
		double ERC;
		double windspeed;
		double moisture_1h,
			   moisture_10h,
			   moisture_100h,
			   moisture_1000h,
			   moisture_herb,
			   moisture_woody;
		int fires;
	};

// LJS 8-15-2003
// The following function is for the FPAAnalysis
// FIXME: put in appropriate place
int windspeed_compare( const void *arg1, const void *arg2 )
{
    fpa_weather *a1 = (fpa_weather *) arg1;
    fpa_weather *a2 = (fpa_weather *) arg2;

	if(a1->windspeed > a2->windspeed)
		return 1;
	else if(a1->windspeed < a2->windspeed)
		return -1;
	return 0;
}

// LJS 8-15-2003
// The following function is for the FPAAnalysis
// FIXME: put in appropriate place
int ERC_compare( const void *arg1, const void *arg2 )
{
    fpa_weather *a1 = (fpa_weather *) arg1;
    fpa_weather *a2 = (fpa_weather *) arg2;

	if(a1->ERC > a2->ERC)
		return 1;
	else if(a1->ERC < a2->ERC)
		return -1;
	return 0;
}

// LJS 8-20-2003
// The following function is for the FPAAnalysis
// FIXME: put in appropriate place
int datestamp_compare( const void *arg1, const void *arg2 )
{
    fpa_weather *a1 = (fpa_weather *) arg1;
    fpa_weather *a2 = (fpa_weather *) arg2;

	if(a1->datestamp > a2->datestamp)
		return 1;
	else if(a1->datestamp < a2->datestamp)
		return -1;
	return 0;
}

CString AddDateStamp()
{
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	CString dStr = dateTime.Format("%m/%d/%Y-%H:%M");
	CString ret;
	//ret.Format("FF+%s %s", verStr, dStr);
	ret.Format("FF+%s %s %s", verStr, theApp.m_buildDateStrPacked, dStr);
	return ret;
}

void CFireplusView::OnDataWeatherobservations(int obsType)
{
	if(!FieldsOK())
		return;
	OnFileSave();

	CWxSet *pWx = new CWxSet(m_pSet->m_pDatabase);
	pWx->m_strSort =  _T("[ObsDate]");
	CString query, temp, temp2;
	m_pSet->BuildBaseQuery(query, "ObsDate", false);
	if(obsType > 0)
	{
		if(query.GetLength() > 0)
			query += "AND ";
		if(obsType == 1)
		{
			CSIGStationSet staSet(this->m_pSet->m_pDatabase);
			staSet.m_strFilter.Format("[StationID]='%6.6s'", m_pSet->m_SIG_Station);
			staSet.Open();
			//temp2.Format("([ObsType] = 'O' OR ([ObsType] = 'R' AND [Hour] = %d ))", staSet.m_RegSchdObs);
			temp2.Format("([DailyObs] = 1)", staSet.m_RegSchdObs);
			//query += temp2;//"([ObsType] = 'O' OR ([ObsType] = 'R' AND [Hour] = 13 AND [SOW] IS NOT NULL))";
			query += temp2;//"([DailyObs] = 1 OR ([DailyObs] = 0 AND [Hour] = 13 AND [SOW] IS NOT NULL))";
		}
		//else
			//query += "[ObsType] = 'R'";
			//query += "[DailyObs] = 0";
	}
	if(m_pSet->m_SIG_Station.GetLength() <= 6)//single station
		temp.Format(" and [StationID] = '%6.6s'", m_pSet->m_SIG_Station);
	else
	{
		CString tmp;
		char sig[128];
		strcpy(sig, m_pSet->m_SIG_Station);
		CStationInSIGSet sSet(m_pSet->m_pDatabase);
		temp.Format("[SIG] = '%-20.20s'", &sig[6]);
		sSet.m_strFilter = _T(temp);
		sSet.Open();
		if(sSet.IsBOF() && sSet.IsEOF())
		{//make sure SIG is valid...
			sSet.Close();
			CString strMsg;
			strMsg.Format(
				_T("ERROR: No stations in SIG.\n Redefine Working Set Query or SIG. \n\n"));
			AfxMessageBox(strMsg);
			delete pWx;
			return;
		}
		int stations = 0;
		temp.Format(" and (");
		while(!sSet.IsEOF())
		{
			if(stations > 0)
				tmp.Format(" or [StationID] = '%6.6s'", sSet.m_StationID);
			else
				tmp.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			temp += tmp;
			stations++;
			sSet.MoveNext();
		}
		temp += ")";
		sSet.Close();
	}
	pWx->m_strFilter = _T(query + temp);
	pWx->Open();
	//make sure some data!
	if(pWx->IsBOF() || pWx->IsEOF())
	{
		AfxMessageBox("No matching records to display.");
		delete pWx;
	}
	else //had some data)
	{
		//create a document to view the results....
		POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate =
				theApp.GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T("WeatherView"))
			{
				CWeatherDoc *pDoc = (CWeatherDoc *)curTemplate->CreateNewDocument();
				pDoc->records = pWx;
				CWeatherFrame* pFrame = (CWeatherFrame*)curTemplate->CreateNewFrame(pDoc, NULL);
				CWeatherView *pView;// = (CWeatherView *)pFrame->GetWindow(GW_CHILD)->GetNextWindow();
				//pView->records = pDoc->records;
				curTemplate->InitialUpdateFrame(pFrame, pDoc);
				POSITION pos = pDoc->GetFirstViewPosition();
				pView = (CWeatherView *)pDoc->GetNextView(pos);
				pView->SetRecords(pDoc->records);
				CString tTitle;
				tTitle = "Observations";
				pDoc->SetTitle(tTitle);
			}
		}
	}

/*	// TODO: Add your command handler code here
	if(!FieldsOK())
		return;
	OnFileSave();
	CWxSet wxSet(m_pSet->m_pDatabase);
	wxSet.m_strSort =  _T("[ObsDate]");
	CString query, temp;
	m_pSet->BuildBaseQuery(query, "ObsDate", false);
	if(obsType > 0)
	{
		if(query.GetLength() > 0)
			query += "AND ";
		if(obsType == 1)
			query += "[ObsType] = 'O'";
		else
			query += "[ObsType] = 'R'";
	}
	if(m_pSet->m_SIG_Station.GetLength() <= 6)//single station
		temp.Format(" and [StationID] = '%6.6s'", m_pSet->m_SIG_Station);
	else
	{
		CString tmp;
		char sig[128];
		strcpy(sig, m_pSet->m_SIG_Station);
		CStationInSIGSet sSet(m_pSet->m_pDatabase);
		temp.Format("[SIG] = '%-20.20s'", &sig[6]);
		sSet.m_strFilter = _T(temp);
		sSet.Open();
		if(sSet.IsBOF() && sSet.IsEOF())
		{//make sure SIG is valid...
			sSet.Close();
			CString strMsg;
			strMsg.Format(
				_T("ERROR: No stations in SIG.\n Redefine Working Set Query or SIG. \n\n"));
			AfxMessageBox(strMsg);
			return;
		}
		int stations = 0;
		temp.Format(" and (");
		while(!sSet.IsEOF())
		{
			if(stations > 0)
				tmp.Format(" or [StationID] = '%s'", sSet.m_StationID);
			else
				tmp.Format("[StationID] = '%s'", sSet.m_StationID);
			temp += tmp;
			stations++;
			sSet.MoveNext();
		}
		temp += ")";
		sSet.Close();
	}
	wxSet.m_strFilter = _T(query + temp);
	wxSet.Open();
	//make sure some data!
	if(wxSet.IsBOF() || wxSet.IsEOF())
		AfxMessageBox("No matching records to display.");
	else
	{
		CWaitCursor wait;
		CBrowseDialog cbd(this, &wxSet);
		cbd.DoModal();
	}
	wxSet.Close();*/
}

void CFireplusView::OnOptionsCauseCodes()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CStatCauseSet scSet(m_pSet->m_pDatabase);
	
	scSet.Open();
	CStatCauseDialog ctd(this, &scSet, m_pSet);
	ctd.DoModal();
	scSet.Close();
}


void CFireplusView::OnOptionsCauseCodes2()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CDOICauseSet scSet(m_pSet->m_pDatabase);
	scSet.m_strSort = "[GeneralCause],[SpecificCause]";
	scSet.Open();
	CDOICauseDialog ctd(this, &scSet, m_pSet);
	ctd.DoModal();
	scSet.Close();
}


void CFireplusView::OnDataStations()
{
	OnFileSave();
	// TODO: Add your command handler code here
	CSIGStationSet stations(m_pSet->m_pDatabase);
	stations.m_strSort = "[StationID]";
	stations.Open();
	CStationdialog sd(NULL, &stations,m_pSet);
	sd.DoModal();
	stations.Close();
	//need to update SIG/Station combo box and metadatatable
	//m_metaTable.GetRowSet().Reset();
	ConfigureStationList();
	ignoreMetaTable = true;
	OnSelchangeSigstation();
}

void CFireplusView::OnFiresAssociations()
{
	// TODO: Add your command handler code here
	if(!FieldsOK())
		return;
	OnFileSave();
	int itemLoc = m_ctlSIG_Station.GetCurSel();
	if(itemLoc == CB_ERR)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: No SIG or Station selected for Active Working Set.\n\n"));
		AfxMessageBox(strMsg);
		return;
	}
	CString str, staStr;
	//m_ctlSIG_Station.GetLBText(itemLoc, staStr);
	staStr = m_pSet->m_SIG_Station;
	staStr.TrimRight();
	str.Format("Set Fire Associations for %s", staStr);
	CFireQuerySheet sheet(str, this, 0, m_pSet->m_pDatabase, staStr,
		NULL, NULL, m_pSet,m_pSet);
	sheet.DoModal();
}

void CFireplusView::OnFiresbutton()
{
	// TODO: Add your control notification handler code here
	OnFiresAssociations();
}

void CFireplusView::OnFiresFireanalysis()
{
	// TODO: Add your command handler code here
	if(!FieldsOK())
		return;
	OnFileSave();
	CFireAnalysisDialog fad(this, m_pSet);
	fad.DoModal();
}

void CFireplusView::OnDataAgencies()
{
	// TODO: Add your command handler code here
	CAgencyDialog ad(this, m_pSet->m_pDatabase);
	ad.DoModal();
}

void CFireplusView::OnWeatherWindRose()
{
    if(!FieldsOK())
		return;
	OnFileSave();
	CWaitCursor wait;
	CWindRoseDialog wrd(this, m_pSet);
	wrd.DoModal();
}

void CFireplusView::OnFileSaveAs()
{
	//tell the mainframe to do it
	CMainFrame *mf = (CMainFrame *)GetParentFrame()->GetParentFrame();
	ASSERT(mf->IsKindOf(RUNTIME_CLASS(CMainFrame)));
	mf->SaveItAs();
}

void CFireplusView::OnWeatherEventlocator()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CWaitCursor wait;
	CEventLocatorDialog eld(this, m_pSet);
	eld.DoModal();
}

void CFireplusView::OnWeatherForecasting()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	if(forecastDialog && fxParamChange)
		forecastDialog->Remove();
	if(!forecastDialog)
	{
		// added for batch
		//
		// save run metadata
		// single run mode
		CRunSave saveRun(m_pSet);
		saveRun.SaveGeneral(0, "Fire Danger Projections", m_pSet);
		saveRun.SaveClimateOptions(curr_runID, m_pSet);
		//saveRun.SaveWxStation(curr_runID, m_pSet);
		saveRun.SaveFireOptions(curr_runID, m_pSet);

		// end single run mode
		// end added for batch
		int rows = 11;
		int *varIDs = new int[rows];
		varIDs[0] = 0;
		varIDs[1] = 2;
		varIDs[2] = 3;
		varIDs[3] = 4;
		varIDs[4] = 6;
		varIDs[5] = 7;
		varIDs[6] = 8;
		varIDs[7] = 9;
		varIDs[8] = 10;
		varIDs[9] = 19;
		varIDs[10] = 30;
		CString oldPlen = m_pSet->m_PeriodLength;
		m_pSet->Edit();
		m_pSet->m_PeriodLength = "1";
		m_pSet->Update();
		AfxGetMainWnd()->EnableWindow(FALSE);
		CClimAnalysis *analysis = new CClimAnalysis(m_pSet->m_pDatabase);
		analysis->checkFires = false;
		if(analysis->RunAnalyze(varIDs, rows, m_pSet, false) != IDOK)
			//Analyze(AfxGetMainWnd(), varIDs, rows, m_pSet, false) != 0)
		{
			m_pSet->Edit();
			m_pSet->m_PeriodLength = oldPlen;
			m_pSet->Update();
			delete analysis;
			AfxGetMainWnd()->EnableWindow(TRUE);
			return;
		}
		m_pSet->Edit();
		m_pSet->m_PeriodLength = oldPlen;
		m_pSet->Update();
		AfxGetMainWnd()->EnableWindow(TRUE);
		forecastDialog  = new CForecastDialog(this, m_pSet, analysis);
		forecastDialog->Create();
	}
	else
	{
		forecastDialog->ShowWindow(SW_SHOWNORMAL);
		forecastDialog->SetActiveWindow();
	}
	fxParamChange = false;
}

void CFireplusView::OnWeatherNfdrscalculator()
{
	CString calcStr;
	calcStr.Format("%s\\NFDRSCalculator2.exe", theApp.exeDir);
	int ret = _spawnl(_P_NOWAIT, calcStr, calcStr, NULL);
	if(ret < 0)
	{
		CString retMsg;
		retMsg.Format("Error launching the NFDRS Calculator.");
		AfxMessageBox(retMsg);
	}
}

void CFireplusView::OnWeatherViewobservationsAll()
{
	OnDataWeatherobservations(0);
}

void CFireplusView::OnWeatherViewobservationsDaily()
{
	OnDataWeatherobservations(1);
}

void CFireplusView::OnWeatherDiurnalGraph()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CWaitCursor wait;
	CDiurnalDialog dd(this, m_pSet);
	dd.DoModal();
}

void CFireplusView::OnUpdateWeatherDiurnalGraph(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnWeatherDiurnalHourlylisting()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CListingDialog cld(this, m_pSet, FALSE);
	cld.DoModal();
}

void CFireplusView::OnWeatherHourlydataanalysisSOWThresholds()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CSowThresholdsSet sowSet(m_pSet->m_pDatabase);
	sowSet.m_strFilter.Format("[SIG_Station] = '%s'", m_pSet->m_SIG_Station);//station->m_StationID);
	sowSet.Open();
	if(sowSet.IsBOF() || sowSet.IsEOF())
	{
		sowSet.AddDefaultForStation(m_pSet->m_SIG_Station);
	}
	CSowThresholdsDlg std(this, &sowSet);
	std.DoModal();
	sowSet.Close();
}
void CFireplusView::OnWeatherHourlydataanalysisSOWOptions()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CSowOptionsSet sowSet(m_pSet->m_pDatabase);
	sowSet.m_strFilter.Format("[SIG_Station] = '%s'", m_pSet->m_SIG_Station);
	sowSet.Open();
	if(sowSet.IsBOF() || sowSet.IsEOF())
	{
		sowSet.AddNew();
		sowSet.m_SIG_Station = m_pSet->m_SIG_Station;
		sowSet.m_SOW_Usage = 0;
		sowSet.m_WetFlag_Usage = 0;
		sowSet.Update();
		sowSet.Requery();
	}
	CSowWetFlagOptionsDlg std(this, &sowSet);
	std.DoModal();
	sowSet.Close();
}
void CFireplusView::OnWeatherHourlydataanalysisSOWReCalc()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CWxSet wxSet(m_pSet->m_pDatabase);
	//wxSet.m_strFilter.Format("[StationID] = '%6.6s' and [ObsType] = 'R'", m_pSet->m_SIG_Station);
	wxSet.m_strFilter.Format("[StationID] = '%6.6s' and [DailyObs] = 0", m_pSet->m_SIG_Station);
	wxSet.Open();
	if(wxSet.IsBOF() && wxSet.IsEOF())//no RAWS records
	{
		AfxMessageBox("Station needs hourly weather to calculate SR_SOW and SR_WetFlag\nSolar Radiation field should also be populated");
		return;
	}
	wxSet.m_strFilter.Format("[StationID] = '%6.6s'", m_pSet->m_SIG_Station);
	wxSet.Requery();
	long nUpdates = wxSet.CalcSOW();
	CString msg;
	msg.Format("SR_SOW/SR_WetFlag calculation complete. Updated %ld records.", nUpdates);
	AfxMessageBox(msg);
}

void CFireplusView::OnUpdateWeatherDiurnalHourlylisting(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnFARSITEExports()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CFARSITEDialog fd(this, m_pSet);
	fd.DoModal();
}

void CFireplusView::OnNelsonDFM()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CNelsonDFM nelson(this);
	nelson.fpSet = m_pSet;
    nelson.RunReport();
}

/*void CFireplusView::OnNelsonDFM2()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CNelsonDFM nelson(this);
	nelson.fpSet = m_pSet;
    nelson.RunReport2();
}*/

void CFireplusView::OnUpdateHourlyEventLocator(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnHourlyEventLocator()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CWaitCursor wait;
	CHourlyEventLocatorDlg eld(this, m_pSet);
	eld.DoModal();
}

void CFireplusView::OnCustomYearlyInits()
{
	if(!FieldsOK())
		return;

	OnFileSave();

	if (strlen(m_pSet->m_SIG_Station) > 6 || atoi(m_pSet->m_SIG_Station) < 1){
       AfxMessageBox("You cannot create custom inits for a SIG. To create custom inits for an individual station within a SIG, first select that station in the 'Working Set' window.");
	   return;
	}
	CCustomInitSet cinitSet(m_pSet->m_pDatabase);
    cinitSet.m_strFilter.Format("[SIG_Station]='%s'", m_pSet->m_SIG_Station);
	cinitSet.m_strSort.Format("[Year]");
	cinitSet.Open();

    CCustomInitsDialog cid(this,m_pSet,&cinitSet);
    cid.DoModal();
	cinitSet.Close();
}

void CFireplusView::OnSize(UINT nType, int cx, int cy)
{
	CRecordView::OnSize(nType, cx, cy);

	if(tableBottomMin && tableRightMin)
	{
		CRect tRect, wRect, rect;
		GetWindowRect(&wRect);
		//m_metaTable.GetWindowRect(&tRect);
		m_grid.GetWindowRect(&tRect);
		rect.left = 7;
		rect.top = tRect.top - wRect.top - 2;
		rect.right = max(tableRightMin, wRect.Width() - 7);
		//rect.bottom = rect.top + max(tableBottomMin, tRect.Height() - 7);
		rect.bottom = max(rect.top + tableBottomMin, wRect.Height() - 7);
		m_grid.MoveWindow(&rect);
	}
}

void CFireplusView::OnUpdateWeatherHourlydataanalysisFARSITEexports(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnUpdateWeatherHourlydataanalysisSOWThresholds(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnUpdateWeatherHourlydataanalysisSOWOptions(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnUpdateWeatherHourlydataanalysisSOWReCalc(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(1);
}

void CFireplusView::OnDestroy()
{
	if(forecastDialog)
	{
		forecastDialog->Remove();
		forecastDialog = NULL;
	}
	CRecordView::OnDestroy();
}

void CFireplusView::OnSelchangeCombo1()
{
	fxParamChange = true;
}

void CFireplusView::OnDataUservariablesViewobservations()
{
 	bool hasUserVars = false, hasUserVals = false;
	//CODBCTableDefInfo tInfo;
	try
	{
		CRecordset trecs(m_pSet->m_pDatabase);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpUserVars"))
		{
			hasUserVars = true;
			trecs.Close();
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	try
	{
		CRecordset trecs(m_pSet->m_pDatabase);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM  ffpUserVals"))
		{
			hasUserVals = true;
			trecs.Close();
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	/*for(int i = 0; i < m_pSet->m_pDatabase->GetODGetTableDefCount(); i++)
	{
		m_pSet->m_pDatabase->GetTableDefInfo(i, tInfo);
		if(tInfo.m_strName.CompareNoCase("ffpUserVars") == 0)
			hasUserVars = true;
		if(tInfo.m_strName.CompareNoCase("ffpUserVals") == 0)
			hasUserVals = true;
	}*/
	if(!hasUserVars || !hasUserVals)
	{
		AfxMessageBox("No User Variables Defined.\nSelect \"Data - User Variables - Manage\"");
		return;
	}
	if(!FieldsOK())
		return;
	OnFileSave();
	CODBCRecordset records(m_pSet->m_pDatabase);
	records.m_strSort =  _T("[ObsDate]");
	CString query, temp;
	m_pSet->BuildBaseQuery(query, "ObsDate", false);
	if(m_pSet->m_SIG_Station.GetLength() <= 6)//single station
		temp.Format(" and [StationID] = '%6.6s'", m_pSet->m_SIG_Station);
	else
	{
		CString tmp;
		char sig[128];
		strcpy(sig, m_pSet->m_SIG_Station);
		CStationInSIGSet sSet(m_pSet->m_pDatabase);
		temp.Format("[SIG] = '%-20.20s'", &sig[6]);
		sSet.m_strFilter = _T(temp);
		sSet.Open();
		if(sSet.IsBOF() && sSet.IsEOF())
		{//make sure SIG is valid...
			sSet.Close();
			CString strMsg;
			strMsg.Format(
				_T("ERROR: No stations in SIG.\n Redefine Working Set Query or SIG. \n\n"));
			AfxMessageBox(strMsg);
			return;
		}
		int stations = 0;
		temp.Format(" and (");
		while(!sSet.IsEOF())
		{
			if(stations > 0)
				tmp.Format(" or [StationID] = '%6.6s'", sSet.m_StationID);
			else
				tmp.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			temp += tmp;
			stations++;
			sSet.MoveNext();
		}
		temp += ")";
		sSet.Close();
	}
	//records.m_strFilter = _T(query + temp);
	CString strSQL = "SELECT * FROM ffpUserVals WHERE " + query + temp;
	records.Open(strSQL, CRecordset::dynaset);
	//make sure some data!
	if(records.IsBOF() || records.IsEOF())
		AfxMessageBox("No matching records to display.");
	else
	{
		CUserBrowseDialog ubd(this, &records);
		ubd.DoModal();
	}
	records.Close();
}

// added for batch2
void CFireplusView::OnRunRestore()
{
	if(!FieldsOK())
		return;

	CRunSaveViewSet records(m_pSet->m_pDatabase);
	//records.m_strSort =  _T("[ffpRuns.runID]");

	CString query, temp;

	//records.m_strFilter = _T(query + temp);
	//CString strSQL = "[ffpRuns],[ffpRunDescription]";
   // records.m_strFilter = _T("ffpRuns.runID = ffpRunDescription.runID");
    CString strSQL = "SELECT *  FROM ffpRuns, ffpRunDescription WHERE ffpRuns.runID = ffpRunDescription.runID ORDER BY ffpRuns.runID";

	records.Open(CRecordset::dynaset, strSQL);
	//records.Open();
	// turn off automatic update of dialog data
	UpdateOnDeactivate = FALSE;
	if(records.IsBOF() || records.IsEOF())
		AfxMessageBox("No matching records to display.");
	else
	{
		CRunRestoreDialog rrd(this, m_pSet, &records);

		if (rrd.DoModal())
		{
			ignoreMetaTable = true;
			UpdateData(FALSE);
			ConfigureStationList();

			OnSelchangeSigstation();
			ignoreMetaTable = false;
		}
	}
	records.Close();
	UpdateOnDeactivate = TRUE;
}

void CFireplusView::OnBatchCreateNew()
{
	if(!FieldsOK())
		return;

	CRunSaveViewSet records(m_pSet->m_pDatabase);
	//records.m_strSort =  _T("[ffpRuns.runID]");

	CString query, temp;

	CRunsSet records2(m_pSet->m_pDatabase);

	//records.m_strFilter = _T(query + temp);
	//CString strSQL = "[ffpRuns],[ffpRunDescription]";
   // records.m_strFilter = _T("ffpRuns.runID = ffpRunDescription.runID");
    CString strSQL = "SELECT *  FROM ffpRuns, ffpRunDescription WHERE ffpRuns.runID = ffpRunDescription.runID ORDER BY ffpRuns.runID";

	records.Open(CRecordset::dynaset, strSQL);
	records2.Open();

	// turn off automatic update of dialog data
	UpdateOnDeactivate = FALSE;
	if(records.IsBOF() || records.IsEOF())
		AfxMessageBox("No runs found. You must run at least one report before creating a new batch.");
	else
	{
		CBatchCreateDialog bcd(this, m_pSet, &records,&records2);

		if (bcd.DoModal()){
		}
	}
	records.Close();

	if (records2.IsOpen())
		 records2.Close();

	UpdateOnDeactivate = TRUE;
}

void CFireplusView::OnBatchManage()
{
	if(!FieldsOK())
		return;

	CBatchSet batches(m_pSet->m_pDatabase);
	batches.m_strSort =  _T("[batchID]");

	CString query, temp;

	//records.m_strFilter = _T(query + temp);
	CString strSQL = "SELECT * FROM ffpBatch";

	batches.Open(CRecordset::dynaset, strSQL);

	// turn off automatic update of dialog data
	UpdateOnDeactivate = FALSE;
	if(batches.IsBOF() || batches.IsEOF())
		AfxMessageBox("No batches have been created.");
	else
	{
		CBatchManageDialog bmd(this, m_pSet, &batches);

		if (bmd.DoModal()){
		//	UpdateData(FALSE);
		//	ConfigureStationList();

		//	OnSelchangeSigstation();
		}
	}
	batches.Close();
	UpdateOnDeactivate = TRUE;
}
// end added for batch2

void CFireplusView::OnBatchSavedRunMaint()
{
	if(!FieldsOK())
		return;

	/* CBatchOptionsSet ba(m_pSet->m_pDatabase);

	CString strSQL = "SELECT * FROM ffpBatchOptions";

	ba.Open(dbOpenDynaset, strSQL);
	*/

	// turn off automatic update of dialog data
	UpdateOnDeactivate = FALSE;
	/* if(ba.IsBOF() || ba.IsEOF()){
		AfxMessageBox("Error: table ffpBatchOptions is missing or corrupt.");
		ba.Close();
	} else
	{
		ba.Close(); */
		CFFPOptsSet optsSet1(m_pSet->m_pDatabase);
		optsSet1.Open();
		if(!optsSet1.IsEOF())
		{
			optsSet1.Edit();
			optsSet1.m_RunSaveOn = GetDocument()->doRunSaving;
			optsSet1.Update();
		}
		optsSet1.Close();

		CBatchSavedRunMaintDialog bmd(this,m_pSet);

		if (IDOK == bmd.DoModal())
		{
			CFFPOptsSet optsSet(m_pSet->m_pDatabase);
			optsSet.Open();
			if(!optsSet.IsEOF())
			{
				GetDocument()->doRunSaving = optsSet.m_RunSaveOn;
			}
			optsSet.Close();
		}
	/* }
	ba.Close();
	*/

	UpdateOnDeactivate = TRUE;
}

void CFireplusView::OnWeatherFlammapexports()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CFriskExportDialog fed(this);
	fed.m_fpSet = m_pSet;
	if(fed.DoModal() != IDOK)
		return;
	CClimAnalysis ca(m_pSet->m_pDatabase);
	ca.checkFires = false;
	int rows = 8;
	int *varIDs = new int[rows];
	//varIDs[0] = 10;
	varIDs[0] = 12;
	varIDs[1] = 16;
	varIDs[2] = 17;
	varIDs[3] = 18;
	varIDs[4] = 19;
	varIDs[5] = 20;
	varIDs[6] = 21;
	varIDs[7] = 22;
	//varIDs[9] = 23;
	//varIDs[10] = 30;
	CString oldPlen = m_pSet->m_PeriodLength;
	m_pSet->Edit();
	m_pSet->m_PeriodLength = "1";
	m_pSet->Update();
	//AfxGetMainWnd()->EnableWindow(FALSE);
	if(ca.RunAnalyze(varIDs, rows, m_pSet, false) != IDOK)
		//Analyze(AfxGetMainWnd(), varIDs, rows, m_pSet, false) != 0)
	{
		m_pSet->Edit();
		m_pSet->m_PeriodLength = oldPlen;
		m_pSet->Update();
		//AfxGetMainWnd()->EnableWindow(TRUE);
		return;
	}
	m_pSet->Edit();
	m_pSet->m_PeriodLength = oldPlen;
	m_pSet->Update();
	int syear = fed.m_sYear, eyear = fed.m_eYear,
		smonth = fed.m_startMonthDay.GetMonth(), emonth = fed.m_endMonthDay.GetMonth(),
		sday = fed.m_startMonthDay.GetDay(), eday = fed.m_endMonthDay.GetDay();
	ca.FlamMapExport(true, fed.m_calmVal, syear, smonth, sday, fed.m_startTime.GetHour(), fed.m_startTime.GetMinute(),
		eyear, emonth, eday, fed.m_endTime.GetHour(), fed.m_endTime.GetMinute(), fed.windType, fed.m_monthlyWinds, fed.m_generateWindrose,
		fed.m_iBinSize, fed.m_iMaxBin, fed.m_bUseWeibull);
	//AfxGetMainWnd()->EnableWindow(TRUE);
}

void CFireplusView::OnBatchInteractive()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CQuickBatchDialog qb(this, m_pSet->m_pDatabase, m_pSet);
	qb.DoModal();
}
/*void CFireplusView::OnOptionsLfisettings()
{
	CLFIDialog lfiDlg(this, m_pSet->m_pDatabase);
	lfiDlg.DoModal();
}*/

void CFireplusView::OnWeatherFirebusinesscandidates()
{
	theApp.UpdateCandidatesFrame(m_pSet->m_pDatabase);
	/*CCandidateSet *candSet = new CCandidateSet(m_pSet->m_pDatabase);
	candSet->Open();
	//create a document to view the results....
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate =
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("CandidatesView"))
		{
			CCandidatesDoc *pCandDoc = (CCandidatesDoc *)curTemplate->CreateNewDocument();
			pCandDoc->records = candSet;
			CCandidatesFrame* pCandFrame = (CCandidatesFrame*)curTemplate->CreateNewFrame(pCandDoc, NULL);
			curTemplate->InitialUpdateFrame(pCandFrame, pCandDoc);
			POSITION pos = pCandDoc->GetFirstViewPosition();
			CCandidatesView *pView = (CCandidatesView *)pCandDoc->GetNextView(pos);
			pView->SetRecords(pCandDoc->records);
			CString tTitle;
			tTitle = "Fire Business Candidates";
			pCandDoc->SetTitle(tTitle);
		}
	}*/
}

/*void CFireplusView::OnWeatherTerm()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CTermsSet termsSet(m_pSet->m_pDatabase);
	termsSet.m_strFilter.Format("[SIG_Station] = '%s'", m_pSet->m_SIG_Station);
	termsSet.Open();
	CTermsDialog ctd(this, &termsSet, m_pSet);
	ctd.DoModal();
	termsSet.Close();
}
*/
void CFireplusView::OnTermGeneral()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CTermsSet termsSet(m_pSet->m_pDatabase);
	//termsSet.m_strFilter.Format("[SIG_Station] = '%s'", m_pSet->m_SIG_Station);
	termsSet.Open();
	CTermsDialog ctd(this, &termsSet, "", true, m_pSet);
	ctd.DoModal();
	termsSet.Close();
}

void CFireplusView::OnTermWorkingset()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CTermsSet termsSet(m_pSet->m_pDatabase);
	termsSet.m_strFilter.Format("[SIG_Station] = '%s'", m_pSet->m_SIG_Station);
	termsSet.Open();
	CTermsDialog ctd(this, &termsSet, m_pSet->m_SIG_Station, false, m_pSet);
	ctd.DoModal();
	termsSet.Close();
}

/*void CFireplusView::OnNelsondeadfuelmoisturesFiresanalysis()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CFireAnalysisDialog fad(this, m_pSet, true);
	fad.DoModal();
}*/

void CFireplusView::OnUpdateNelsondeadfuelmoisturesFiresanalysis(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnFiresanalysisAllnelsonmoistures()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CFireAnalysisDialog fad(this, m_pSet, true, false);
	fad.DoModal();
}

void CFireplusView::OnFiresanalysis1and10hourmoistures()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CFireAnalysisDialog fad(this, m_pSet, true, true);
	fad.DoModal();
}

void CFireplusView::OnUpdateFiresanalysisAllnelsonmoistures(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnUpdateFiresanalysis1and10hourmoistures(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnComparewithdailynfdrs1and10hourmoistures()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CNelsonDFM nelson(this);
	nelson.fpSet = m_pSet;
    nelson.RunReport2(true);
}

void CFireplusView::OnComparewithdailynfdrsAllnelsonmoistures()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CNelsonDFM nelson(this);
	nelson.fpSet = m_pSet;
    nelson.RunReport2(false);
}

void CFireplusView::OnUpdateComparewithdailynfdrs1and10hourmoistures(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnUpdateComparewithdailynfdrsAllnelsonmoistures(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(hasHourly);
}

void CFireplusView::OnLfioptionsCurrentsig()
{
	if(!FieldsOK())
		return;
	OnFileSave();
	CLFIPropertySheet lfiSheet("GSI Options", this, 0, m_pSet->m_pDatabase, m_pSet->m_SIG_Station);
	lfiSheet.DoModal();
}

void CFireplusView::OnLfioptionsDefaults()
{
	CLFIPropertySheet lfiSheet("GSI Options", this, 0, m_pSet->m_pDatabase);
	lfiSheet.DoModal();
}

void CFireplusView::OnEnChangeSday()
{
	if(!m_StartDay.m_hWnd)
		return;
	char buf[64];
	GetDlgItem(IDC_SDAY)->GetWindowText(buf, 63);
	buf[63] = 0;
	if(atoi(buf) < 1 || atoi(buf) > 31)
	{
		AfxMessageBox("Error: Start Day must be from 1 to 31");
	}
}

void CFireplusView::OnEnChangeEday()
{
	if(!m_EndDay.m_hWnd)
		return;
	char buf[64];
	GetDlgItem(IDC_EDAY)->GetWindowText(buf, 63);
	buf[63] = 0;
	if(atoi(buf) < 1 || atoi(buf) > 31)
	{
		AfxMessageBox("Error: End Day must be from 1 to 31");
	}
}

void CFireplusView::OnEnChangeEyear()
{
	if(!m_EndYear.m_hWnd)
		return;
	char buf[64];
	GetDlgItem(IDC_EYEAR)->GetWindowText(buf, 63);
	buf[63] = 0;
	int sMin, sMax;
	m_spnEyear.GetRange(sMin, sMax);
	if(strlen(buf) > 3 && sMin < sMax && (atoi(buf) < sMin  || atoi(buf) > sMax))
	{
		CString msg;
		msg.Format("Error: End Year must be from %d to %d", sMin, sMax);
		AfxMessageBox(msg);
	}
}

void CFireplusView::OnEnChangeSyear()
{
	if(!m_StartYear.m_hWnd)
		return;
	char buf[64];
	GetDlgItem(IDC_SYEAR)->GetWindowText(buf, 63);
	buf[63] = 0;
	int sMin, sMax;
	m_spnSyear.GetRange(sMin, sMax);
	if(strlen(buf) > 3 && sMin < sMax && (atoi(buf) < sMin  || atoi(buf) > sMax))
	{
		CString msg;
		msg.Format("Error: Start Year must be from %d to %d", sMin, sMax);
		AfxMessageBox(msg);
	}
}

bool CFireplusView::SetYears(CString fmStr)
{
	bool ret = false;
	int itemLoc = m_ctlSIG_Station.GetCurSel(), yr1 = 2030, yr2 = 0;
	CUGCell cell;
	CWxSet wxSet(GetDocument()->m_pDB);
	wxSet.m_strSort = _T("[ObsDate]");
	char temp[64], station[64];
	bool isValid2016 = true;
	bool using2016 = false;
	if (itemLoc != CB_ERR)
	{
		CWaitCursor wait;
		hasHourly = FALSE;

		m_ctlSIG_Station.GetLBText(itemLoc, station);
		if (strncmp(station, "SIG", 3) == 0)//is a SIG
		{
			CStationInSIGSet sSet(GetDocument()->m_pDB);
			sprintf(temp, "[SIG] = '%-20.20s'", &station[6]);
			sSet.m_strFilter = _T(temp);
			sSet.Open();
			wxSet.Open();
			int row = 0;
			while (!sSet.IsEOF() && (!using2016 || (using2016 && isValid2016)))
			{
				sprintf(temp, "[StationID] = '%6.6s'", sSet.m_StationID);
				wxSet.m_strFilter = _T(temp);
				wxSet.Requery();
				if (!wxSet.IsBOF() && !wxSet.IsEOF())//has records
				{	//set years for spinners
					wxSet.MoveFirst();
					if (yr1 > 0)
						yr1 = min(yr1, wxSet.m_ObsDate.GetYear());
					else
						yr1 = wxSet.m_ObsDate.GetYear();
					wxSet.MoveLast();
					if (yr2 > 0)
						yr2 = max(yr2, wxSet.m_ObsDate.GetYear());
					else
						yr2 = wxSet.m_ObsDate.GetYear();
					if (HasUserTables(GetDocument()->m_pDB))
					{
						CRecordset userVals(GetDocument()->m_pDB);
						CString strSQL;
						strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%6.6s'", sSet.m_StationID);
						userVals.m_strSort = _T("[ObsDate]");
						userVals.Open(CRecordset::dynaset, strSQL);
						if (!userVals.IsBOF() && !userVals.IsEOF())//has records
						{	//set years for spinners
							CDBVariant var;
							userVals.MoveFirst();
							userVals.GetFieldValue("ObsDate", var);
							COleDateTime dt(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
								var.m_pdate->minute, var.m_pdate->second);
							//dt = var.m_boolVal;
							if (yr1 > 0)
								yr1 = min(yr1, dt.GetYear());
							else
								yr1 = dt.GetYear();
							userVals.MoveLast();
							userVals.GetFieldValue("ObsDate", var);
							COleDateTime dt2(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
								var.m_pdate->minute, var.m_pdate->second);
							if (yr2 > 0)
								yr2 = max(yr2, dt2.GetYear());
							else
								yr2 = dt2.GetYear();
						}
						userVals.Close();
					}
					CSIGStationSet ssSet(GetDocument()->m_pDB);
					ssSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
					ssSet.Open();
					if (isNFDRS2016(fmStr.GetLength() > 0 ? fmStr[0] : ssSet.m_NFDRSFM[0]))
					{
						using2016 = true;
						COleDateTime n2Start, n2End;
						bool valid2016 = m_pSet->GetNFDRS2016Range(sSet.m_StationID, &n2Start, &n2End);
						if (valid2016 && isValid2016)
						{
							yr1 = min(yr1, n2Start.GetYear());
							yr2 = max(yr2, n2End.GetYear());
						}
						else
						{
							isValid2016 = false;
							yr1 = yr2 = 0;
						}
					}
					ssSet.Close();
				}
				sSet.MoveNext();
			}
		}
		else
		{
			//single station
			wxSet.Open();
			sprintf(temp, "[StationID] = '%6.6s'", station);
			wxSet.m_strFilter = _T(temp);
			wxSet.Requery();
			if (!wxSet.IsBOF() && !wxSet.IsEOF())//has records
			{	//set years for spinners
				wxSet.MoveFirst();
				if (yr1 > 0)
					yr1 = min(yr1, wxSet.m_ObsDate.GetYear());
				else
					yr1 = wxSet.m_ObsDate.GetYear();
				wxSet.MoveLast();
				if (yr2 > 0)
					yr2 = max(yr2, wxSet.m_ObsDate.GetYear());
				else
					yr2 = wxSet.m_ObsDate.GetYear();
				if (HasUserTables(GetDocument()->m_pDB))
				{
					CRecordset userVals(GetDocument()->m_pDB);
					CString strSQL;
					strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%6.6s'", station);
					userVals.m_strSort = _T("[ObsDate]");
					userVals.Open(CRecordset::dynaset, strSQL);
					if (!userVals.IsBOF() && !userVals.IsEOF())//has records
					{	//set years for spinners
						CDBVariant var;
						userVals.MoveFirst();
						userVals.GetFieldValue("ObsDate", var);
						COleDateTime dt(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
							var.m_pdate->minute, var.m_pdate->second);
						//dt = var.m_boolVal;
						if (yr1 > 0)
							yr1 = min(yr1, dt.GetYear());
						else
							yr1 = dt.GetYear();
						userVals.MoveLast();
						userVals.GetFieldValue("ObsDate", var);
						COleDateTime dt2(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
							var.m_pdate->minute, var.m_pdate->second);
						if (yr2 > 0)
							yr2 = max(yr2, dt2.GetYear());
						else
							yr2 = dt2.GetYear();
					}
					userVals.Close();
				}
				CSIGStationSet ssSet(GetDocument()->m_pDB);
				ssSet.m_strFilter.Format("[StationID] = '%6.6s'", station);
				ssSet.Open();
				if (isNFDRS2016(fmStr.GetLength() > 0 ? fmStr[0] : ssSet.m_NFDRSFM[0]))
				{
					using2016 = true;
					COleDateTime n2Start, n2End;
					bool valid2016 = m_pSet->GetNFDRS2016Range(ssSet.m_StationID, &n2Start, &n2End);
					if (valid2016 && isValid2016)
					{
						yr1 = n2Start.GetYear();// min(yr1, n2Start.GetYear());
						yr2 = n2End.GetYear();// max(yr2, n2End.GetYear());
					}
					else
					{
						isValid2016 = false;
						yr1 = yr2 = 0;
					}
				}
				ssSet.Close();
			}
			wxSet.m_strFilter += " and [DailyObs] = 0";
			wxSet.Requery();
			if (!wxSet.IsBOF() && !wxSet.IsEOF())//has RAWS records
				hasHourly = TRUE;
		}
		CString gStr;
		if (!isValid2016 && using2016)
		{
			gStr.Format("No Suitable Hourly Data");
		}
		else if (yr1 > 0 && yr2 > 0)
		{
			gStr.Format("Data Years (%d - %d)", yr1, yr2);

		}
		else//invalid years
		{
			gStr.Format("No Weather Data");
		}
		dataYearsBtn.SetWindowTextA(gStr);
		m_spnSyear.SetRange(yr1, yr2);
		m_spnEyear.SetRange(yr1, yr2);
		char yrText1[16], yrText2[16];
		m_spnSyear.GetBuddy()->GetWindowText(yrText1, 15);
		m_spnEyear.GetBuddy()->GetWindowText(yrText2, 15);
		int viewYr1, viewYr2;
		if (strlen(yrText1) > 0)
			viewYr1 = atoi(yrText1);
		else
		{
			if (!m_pSet->IsFieldNull(&m_pSet->m_StartYear))
				viewYr1 = m_pSet->m_StartYear;
			else
				viewYr1 = yr1;
		}
		if (strlen(yrText2) > 0)
			viewYr2 = atoi(yrText2);
		else
		{
			if (!m_pSet->IsFieldNull(&m_pSet->m_EndYear))
				viewYr2 = m_pSet->m_EndYear;
			else
				viewYr2 = yr2;
		}
		if (viewYr1 <= yr1 || viewYr1 >= yr2)
		{
			sprintf(temp, "%d", yr1);
			m_spnSyear.GetBuddy()->SetWindowText(temp);
		}
		else
		{
			sprintf(temp, "%d", m_pSet->m_StartYear);
			m_spnSyear.GetBuddy()->SetWindowText(temp);
		}
		if (viewYr2 <= yr1 || viewYr2 >= yr2)
		{
			sprintf(temp, "%d", yr2);
			m_spnEyear.GetBuddy()->SetWindowText(temp);
		}
		else
		{
			sprintf(temp, "%d", m_pSet->m_EndYear);
			m_spnEyear.GetBuddy()->SetWindowText(temp);
		}
	}
	wxSet.Close();
	// changed 2012 FFP4.1 - if saved working set has valid years, use
	/*if (m_pSet && m_pSet->m_StartYear > 1900 && m_pSet->m_StartYear < 2100 && m_pSet->m_EndYear > 1900 && m_pSet->m_EndYear < 2100)
	{
		sprintf(temp, "%d", m_pSet->m_StartYear);
		m_spnSyear.GetBuddy()->SetWindowText(temp);
		sprintf(temp, "%d", m_pSet->m_EndYear);
		m_spnEyear.GetBuddy()->SetWindowText(temp);
	}*/
	if (yr1 > 0 && yr2 > 0)
		return true;
	return false;
}

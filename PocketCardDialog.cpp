// PocketCardDialog.cpp : implementation file
//
 
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
#include "fireplusSet.h"
#include "fireplusView.h"
#include "climateSet.h"
#include "PocketCardSet.h"
#include "wxSet.h"
#include "SIGStationSet.h"
#include "ClimateSet.h"
#include "FireDay.h"
#include "perioddoc.h"
#include "CFiresSet.h"
#include "ClimAnalysis.h"
#include "PocketCardDialog.h"
#include "PcBgDiaolg.h"
#include "FireSelDialog.h"
#include "FireAssocSet.h"
#include "NoWxDialog.h"
#include <direct.h>
#include "StationInSIGSet.h"
#include "RunSave.h"
#include "BatchOutputFrame.h"
#include "BatchOutputDoc.h"
#include "FFPOptsSet.h"
#include "RunPocketCardSet.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int curr_runID;
extern CFireplusApp theApp;

PocketCardCUG::PocketCardCUG()
{
}

PocketCardCUG::~PocketCardCUG()
{
}

/////////////////////////////////////////////////////////////////////////////
//	OnSetup
//		This function is called just after the grid window
//		is created or attached to a dialog item.
//		It can be used to initially setup the grid
void PocketCardCUG::OnSetup()
{
	CRect rect;
	GetClientRect(&rect);
	m_nButtonIndex = AddCellType(&m_button);
	SetCurrentCellMode(2);
	SetNumberCols(4);
	QuickSetText(0, -1, "Fire Name");
	QuickSetText(1, -1, "Fire Date");
	QuickSetText(2, -1, "Find");
	QuickSetText(3, -1, "Clear");
	int wid = rect.Width() / 8;
	SetColWidth(-1, 0);
	SetColWidth(0, wid * 4);
	SetColWidth(1, wid * 2);
	SetColWidth(2, wid);
	SetColWidth(3, wid);
	((CUGEdit* )GetEditClass())->SetAutoSize( FALSE );
	CUGCell cell;
	//GetColDefault( 0, &cell );
	//cell.SetCellType(UGCT_DROPLIST);
	//cell.SetCellTypeEx (UGCT_DROPLISTHIDEBUTTON);
	//cell.SetLabelText(pDlg->m_varsString);
	//SetColDefault( 0, &cell );
	GetColDefault( 2, &cell );
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetText("Find");
	cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
	cell.SetHBackColor(GetSysColor(COLOR_BTNFACE));
	cell.SetHTextColor(cell.GetTextColor());
	SetColDefault( 2, &cell );
	GetColDefault( 3, &cell );
	cell.SetCellType(m_nButtonIndex);
	cell.SetCellTypeEx(UGCT_BUTTONNOFOCUS);
	cell.SetText("Clear");
	cell.SetBackColor(GetSysColor(COLOR_BTNFACE));
	cell.SetHBackColor(GetSysColor(COLOR_BTNFACE));
	cell.SetHTextColor(cell.GetTextColor());
	SetColDefault( 3, &cell );
	SetNumberRows(3);
}

int PocketCardCUG::OnCellTypeNotify(long ID,int col,long row,long msg, LONG_PTR param)
{
	//UNREFERENCED_PARAMETER(ID);
	//UNREFERENCED_PARAMETER(col);
	//UNREFERENCED_PARAMETER(row);
	//UNREFERENCED_PARAMETER(msg);
	UNREFERENCED_PARAMETER(param);
	if(ID == m_nButtonIndex && msg == UGCT_BUTTONUP)
	{
		if(col == 2)//find a fire
		{
			//see if there's fire associations
			CFireplusSet *fpSet = ((CPocketCardDialog *)GetParent())->fpSet;
			CFireAssocSet associations(fpSet->m_pDatabase);
			CString staStr = fpSet->m_SIG_Station;
			if(staStr.GetLength() > 6)//is a SIG
			{
				char temp[64];
				strcpy_s(temp, staStr);
				staStr.Format("%s", &temp[6]);
			}
			associations.m_strFilter.Format("[SIG/Station] = '%s'", staStr);
			associations.Open();
			if(associations.IsBOF() || associations.IsEOF())
			{
				AfxMessageBox("ERROR:No fire associations specified.");
				associations.Close();
				return TRUE;
			}
			//have associations, now build fire edit set and make sure there's fires
			CString query, tQuery, temp, baseQuery;
			int count = 0;
			fpSet->BuildBaseQuery(baseQuery, "Discovery", false);
			while(!associations.IsEOF())
			{
				temp = "";
				if(!associations.IsFieldNull(&associations.m_SubunitID))
					temp.Format("[SubunitID] = %ld", associations.m_SubunitID);
				else if(!associations.IsFieldNull(&associations.m_UnitID))
					temp.Format("[UnitID] = %ld", associations.m_UnitID);
				else if(!associations.IsFieldNull(&associations.m_RegionID))
					temp.Format("[RegionID] = %ld", associations.m_RegionID);
				if(count > 0)
					tQuery += " or ";
				tQuery += temp;
				associations.MoveNext();
				count++;
			}
			if(tQuery.GetLength() > 0)
				query.Format("(%s) and (%s)", tQuery, baseQuery);
			else
				query = baseQuery;
			associations.Close();
			CFiresSet fireEditSet(fpSet->m_pDatabase);
			fireEditSet.m_strFilter = query;
			fireEditSet.m_strSort = "[Discovery]";
			fireEditSet.Open();
			if(fireEditSet.IsEOF())
			{
				AfxMessageBox("Error: No fires found for specified associations.\nCheck your working set data years and annual filter.");
				fireEditSet.Close();
				return TRUE;
			}
			//have fires, display them in a FireSelDialog
			CFireSelDialog fsd(this, &fireEditSet);
			ReleaseCapture();
			if(IDOK == fsd.DoModal())
			{
				//slam record into Row of table
				if(fsd.selRec >= 0)
				{
					fireEditSet.SetAbsolutePosition(fsd.selRec + 1);
					//CDTData val = fireEditSet.m_FireName;
					//CPVDTCell cell = m_FiresTable.GetCellSet().GetItem(Row, 0);
					//cell.SetValue(val);
					QuickSetText(0, row, fireEditSet.m_FireName.Trim());
					QuickSetText(1, row, fireEditSet.m_Discovery.Format(VAR_DATEVALUEONLY));
					//val = fireEditSet.m_Discovery;
					//cell = m_FiresTable.GetCellSet().GetItem(Row, 1);
					//cell.SetValue(val);
					//m_FiresTable.Refresh();
					RedrawRow(row);
				}
			}
			fireEditSet.Close();
		}
		else if(col == 3)//clear fire
		{
			QuickSetText(0, row, "");
			QuickSetText(1, row, "");
			RedrawRow(row);
		}
	}
	return TRUE;
}

void PocketCardCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	//DrawExcelFocusRect(dc,rect);

	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

COLORREF PocketCardCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);
}

void PocketCardCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	//UNREFERENCED_PARAMETER(*vcKey);
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey );
}

void PocketCardCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	//UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	if(col == 0 || col == 1)
		StartEdit();
}

/////////////////////////////////////////////////////////////////////////////
// CPocketCardDialog dialog

CString modelStrings[] =
{
	"A - Western Annual Grasses",
	"B - California Chaparral",
	"C - Pine-Grass Savanna",
	"D - Southern Rough",
	"E - Hardwood Litter (Winter)",
	"F - Intermediate Brush",
	"G - Short-Needle (Heavy Dead)",
	"H - Short-Needle (Normal Dead)",
	"I - Heavy Slash",
	"J - Intermediate Slash",
	"K - Light Slash",
	"L - Western Perennial Grasses",
	"",
	"N - Sawgrass",
	"O - High Pocosin",
	"P - Southern Pine Plantation",
	"Q - Alaskan Black Spruce",
	"R - Hardwood Litter (Summer)",
	"S - Tundra",
	"T - Sagebrush-Grass",
	"U - Western Pines",
	"V - Grass (NFDRS4)",
	"W - Grass-Shrub (NFDRS4)",
	"X - Brush (NFDRS4)",
	"Y - Timber (NFDRS4)",
	"Z - Slash/Blowdown (NFDRS4)"
};

CPocketCardDialog::CPocketCardDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet /*=NULL*/,
	CPocketCardSet *_pocketSet /*= NULL*/)
	: CDialog(CPocketCardDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPocketCardDialog)
	m_Area = _T("");
	m_Bitmap = _T("");
	m_Index = _T("");
	m_PEText = _T("");
	m_Line1 = _T("");
	m_Line2 = _T("");
	m_Line3 = _T("");
	yearToDate = FALSE;
	extra = _T("");
	m_strResponsibleAgency = _T("");
	m_bNWCGStd = FALSE;
	//}}AFX_DATA_INIT
	fpSet = _fpSet;
	pocketSet = _pocketSet;
	//set members to values in pocketSet (if applicable)
	if(!pocketSet->IsEOF() && !pocketSet->IsBOF())
	{
		m_Area = pocketSet->m_FireDangerArea;
		m_Bitmap = pocketSet->m_AreaBitmap;
		m_Index = pocketSet->m_Index;
		m_Line1 = pocketSet->m_Line1;
		m_Line2 = pocketSet->m_Line2;
		m_Line3 = pocketSet->m_Line3;
		m_PEText = pocketSet->m_PastExperience;
		m_bNWCGStd = pocketSet->m_NWCGStd;
		m_strResponsibleAgency = pocketSet->m_RespAgency;
		m_strResponsibleAgency.TrimRight();
		m_strResponsibleAgency.TrimLeft();
	}
	if(pParent->IsKindOf(RUNTIME_CLASS( CFireplusView ) ) )
	{
		CFireplusView *mView = (CFireplusView *)pParent;
		int sMin, sMax;
		mView->m_spnEyear.GetRange(sMin, sMax);
		lastDataYear = sMax;
	}
}

void CPocketCardDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPocketCardDialog)
	DDX_Control(pDX, IDC_NWCGSTANDARD, m_ctlNWCGStd);
	DDX_Control(pDX, IDC_EXTRA, extraBox);
	DDX_Control(pDX, IDC_YTDBUTTON, ytdButton);
	DDX_Control(pDX, IDC_AREA, areaControl);
	DDX_Control(pDX, IDC_INDEX, m_IndexBox);
	DDX_Text(pDX, IDC_AREA, m_Area);
	DDV_MaxChars(pDX, m_Area, 30);
	DDX_Text(pDX, IDC_BITMAP, m_Bitmap);
	DDX_CBString(pDX, IDC_INDEX, m_Index);
	DDV_MaxChars(pDX, m_Index, 50);
	DDX_Text(pDX, IDC_PETEXT, m_PEText);
	DDV_MaxChars(pDX, m_PEText, 720);
	//DDX_Control(pDX, IDC_RH, m_RH);
	//DDX_Control(pDX, IDC_TEMP, m_Temp);
	//DDX_Control(pDX, IDC_WINDSPEED, m_WindSpeed);
	//DDX_Control(pDX, IDC_YEAR1, m_Year1);
	//DDX_Control(pDX, IDC_YEAR2, m_Year2);
	DDX_Text(pDX, IDC_LINE1, m_Line1);
	DDV_MaxChars(pDX, m_Line1, 25);
	DDX_Text(pDX, IDC_LINE2, m_Line2);
	DDV_MaxChars(pDX, m_Line2, 25);
	DDX_Text(pDX, IDC_LINE3, m_Line3);
	DDV_MaxChars(pDX, m_Line3, 25);
	//DDX_Control(pDX, IDC_LIVEFM, m_LiveFM);
	DDX_Check(pDX, IDC_YTDBUTTON, yearToDate);
	DDX_CBString(pDX, IDC_EXTRA, extra);
	//DDX_Control(pDX, IDC_CP, m_cp);
	DDX_Text(pDX, IDC_RESPONSIBLEAGENCY, m_strResponsibleAgency);
	DDV_MaxChars(pDX, m_strResponsibleAgency, 35);
	DDX_Check(pDX, IDC_NWCGSTANDARD, m_bNWCGStd);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_SPINCP, m_spinCP);
	DDX_Control(pDX, IDC_SPINYEAR1, m_spinYear1);
	DDX_Control(pDX, IDC_SPINYEAR2, m_spinYear2);
	DDX_Control(pDX, IDC_SPINWIND, m_spinWind);
	DDX_Control(pDX, IDC_SPINRH, m_spinRH);
	DDX_Control(pDX, IDC_SPINTEMP, m_spinTemp);
	DDX_Control(pDX, IDC_SPINLIVE_FM, m_spinLiveFM);
}

BEGIN_MESSAGE_MAP(CPocketCardDialog, CDialog)
	//{{AFX_MSG_MAP(CPocketCardDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBrowse)
	ON_BN_CLICKED(IDC_BKGRNDBUTTON, OnBkgrndbutton)
	ON_COMMAND(ID_VARSORT, OnVarSort)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPocketCardDialog message handlers

void CPocketCardDialog::OnBrowse()
{
	// TODO: Add your control notification handler code here
	chdir(theApp.userDir);
	CFileDialog	dlg (TRUE, _T("bmp"), NULL, OFN_HIDEREADONLY,
		_T("Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), this);
	if (IDOK == dlg.DoModal())
	{
		m_Bitmap = dlg.GetPathName();
		UpdateData(FALSE);
		theApp.SetUserDir(dlg.GetPathName());
	}
	chdir(theApp.dbDir);
}

BOOL CPocketCardDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);

	//add indices and set selected one
	extraBox.AddString("");
	if(fpSet)
	{
		try
		{    // requery recordset
			int loc = 0;
			CClimateSet climSet(fpSet->m_pDatabase);
			climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
			climSet.setVarSortOrder(theApp.varSortOrder);
			climSet.Open();
			if (climSet.IsOpen())
			{
				while (!climSet.IsEOF())
				{
					int eLoc = extraBox.AddString(climSet.m_Variable_Name);
					extraBox.SetItemData(eLoc, climSet.m_VarID);
					if(strncmp(climSet.m_Variable_Name, pocketSet->m_ExtraVar, strlen(climSet.m_Variable_Name)) == 0)
						extraBox.SetCurSel(eLoc);
					CString str = climSet.m_Variable_Name;
					int iLoc = m_IndexBox.AddString(str);
					m_IndexBox.SetItemData(iLoc,climSet.m_VarID - 1);
					if(str == m_Index)
					{
						//m_IndexBox.SetWindowText(str);
						m_IndexBox.SetCurSel(loc);
					}
					climSet.MoveNext();
					loc++;
				}
			}
			climSet.Close();
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations
			// when opening tables
			e->Delete( );
		}
	}
	//set min and max values for year1 and year2
	m_spinYear1.SetRange(fpSet->m_StartYear, fpSet->m_EndYear);
	m_spinYear2.SetRange(fpSet->m_StartYear, fpSet->m_EndYear);
	//set year controls
	//see if current year data available
	COleDateTime today;
	today = COleDateTime::GetCurrentTime();
	int curYear = today.GetYear();
	if(curYear <= fpSet->m_EndYear || curYear > lastDataYear)
		ytdButton.EnableWindow(FALSE);
	if(pocketSet->m_Year1 >= fpSet->m_StartYear && pocketSet->m_Year1 <= fpSet->m_EndYear)
		m_spinYear1.SetPos(pocketSet->m_Year1);
	if(pocketSet->m_Year2 >= fpSet->m_StartYear && pocketSet->m_Year2 <= fpSet->m_EndYear)
		m_spinYear2.SetPos(pocketSet->m_Year2);
	//set remaining values for remaining controls...
	//local thresholds
	m_spinTemp.SetRange(1, 130);
	//m_spinTemp.SetPos(pocketSet->m_Temp);
	m_spinRH.SetRange(1, 100);
	//m_spinRH.SetPos(pocketSet->m_RH);
	m_spinWind.SetRange(0, 100);
	//m_spinWind.SetPos(pocketSet->m_WindSpeed);
	m_spinLiveFM.SetRange(0, 1000);
	//m_spinLiveFM.SetPos(pocketSet->m_LiveFM);
	m_spinCP.SetRange(1, 99);
	m_spinCP.SetPos(pocketSet->m_CP);
	//fires info
	CString str;
	str = pocketSet->m_Fire1;
	str.Trim();
	if(str.GetLength() > 0)
	{
		m_grid.QuickSetText(0, 0, str);
		m_grid.QuickSetText(1, 0, pocketSet->m_Fire1Date.Format(VAR_DATEVALUEONLY));
	}
	str = pocketSet->m_Fire2;
	str.Trim();
	if(str.GetLength() > 0)
	{
		m_grid.QuickSetText(0, 1, str);
		m_grid.QuickSetText(1, 1, pocketSet->m_Fire2Date.Format(VAR_DATEVALUEONLY));
	}
	str = pocketSet->m_Fire3;
	str.Trim();
	if(str.GetLength() > 0)
	{
		m_grid.QuickSetText(0, 2, str);
		m_grid.QuickSetText(1, 2, pocketSet->m_Fire3Date.Format(VAR_DATEVALUEONLY));
	}
	CRuntimeClass* prt = this->m_pParentWnd->GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, "CBatchOutputFrame" ) == 0)
	{
		m_IndexBox.EnableWindow(FALSE);
		CBatchOutputFrame *pFrame = (CBatchOutputFrame *)m_pParentWnd;
		m_IndexBox.SetCurSel(pFrame->m_pDoc->activeVar);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPocketCardDialog::OnOK()
{
	UpdateData();
	if(m_IndexBox.GetCurSel() == CB_ERR)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: An Index must be selected.\n"));
		AfxMessageBox(strMsg);
		return;
	}

	m_varID = (int) m_IndexBox.GetItemData(m_IndexBox.GetCurSel());
	
	CString str;
	areaControl.GetWindowText(str);
	str.TrimRight();
	if(str == "")
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Area name must be filled in.\n"));
		AfxMessageBox(strMsg);
		return;
	}
	
	m_Bitmap.TrimLeft();
	m_Bitmap.TrimRight();
	if(m_Bitmap.GetLength() > 0)
	{
		CFileFind ff;
		if(ff.FindFile(m_Bitmap, 0) == 0)
		{
			CString msg;
			msg.Format("Error: Can't find file:\n%s\n\nContinue Pocket Card generation?", m_Bitmap);
			if(AfxMessageBox(msg, MB_YESNO) != IDYES)
				return;
		}
	}
	if(m_strResponsibleAgency.GetLength() <= 0)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Responsible Agency can not be blank.\n"));
		AfxMessageBox(strMsg);
		return;
	}
	if(m_strResponsibleAgency.GetLength() > 35)
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: Responsible Agency field is limited to 35 characters.\n"));
		AfxMessageBox(strMsg);
		return;
	}
	// added for batch
	//
    // save run metadata
    // single run mode
	CRuntimeClass* prt = this->m_pParentWnd->GetRuntimeClass();
	if(strcmp( prt->m_lpszClassName, "CBatchOutputFrame" ) != 0)
	{
		CRunSave saveRun(fpSet);
		saveRun.SaveGeneral(0, "Pocket Card", fpSet);
		saveRun.SaveClimateOptions(curr_runID, fpSet);
		// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
		saveRun.SaveFireOptions(curr_runID, fpSet);
		saveRun.SaveRunOption(curr_runID, REPORT_WEATHER_POCKET_CARD, 0, "", "", fpSet);
	}

	// end single run mode
	// end added for batch
	CRunPocketCardSet runPCSet(fpSet->m_pDatabase);
	runPCSet.m_strFilter.Format("[runID] = %d", curr_runID);
	runPCSet.Open();
	if(runPCSet.IsEOF())
		runPCSet.AddNew();
	else
		runPCSet.Edit();
	runPCSet.m_runID = curr_runID;
	runPCSet.m_varID = m_varID;
	pocketSet->Edit();
	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";

    climSet.setVarSortOrder(theApp.varSortOrder);
	climSet.Open();
	if (climSet.IsOpen())
	{
		while (!climSet.IsEOF()) 
		{
			if (climSet.m_VarID == m_varID + 1)
			{
				runPCSet.m_Index = pocketSet->m_Index = climSet.m_Variable_Name;
				break;

			} 
			climSet.MoveNext();
		}
		
	}
	climSet.Close();
	runPCSet.m_CP = pocketSet->m_CP = m_spinCP.GetPos();//m_cp.GetValueInteger();
	extra.TrimRight();
	extra.TrimLeft();
	runPCSet.m_ExtraVar = pocketSet->m_ExtraVar = /*pDoc->m_pAnalysis->m_pocketOpts.extraStr =*/ extra;
	runPCSet.m_FireDangerArea = pocketSet->m_FireDangerArea = m_Area;
	m_Bitmap.Trim();
	runPCSet.m_AreaBitmap = pocketSet->m_AreaBitmap = m_Bitmap;

	runPCSet.m_PastExperience = pocketSet->m_PastExperience = m_PEText;
	runPCSet.m_RH = pocketSet->m_RH = (short)m_spinRH.GetPos();//pDoc->m_pAnalysis->m_pocketOpts.m_RH;
	runPCSet.m_Temp = pocketSet->m_Temp = (short)m_spinTemp.GetPos();//pDoc->m_pAnalysis->m_pocketOpts.m_Temp;
	runPCSet.m_WindSpeed = pocketSet->m_WindSpeed = (short)m_spinWind.GetPos();//pDoc->m_pAnalysis->m_pocketOpts.m_WindSpeed;
	runPCSet.m_LiveFM = pocketSet->m_LiveFM = (short)m_spinLiveFM.GetPos();//pDoc->m_pAnalysis->m_pocketOpts.m_LiveFM;
	runPCSet.m_Year1 = pocketSet->m_Year1 = (short)m_spinYear1.GetPos();//pDoc->m_pAnalysis->m_pocketOpts.m_Year1;
	runPCSet.m_Year2 = pocketSet->m_Year2 = (short)m_spinYear2.GetPos();//pDoc->m_pAnalysis->m_pocketOpts.m_Year2;
	CString tmpStr = m_grid.QuickGetText(0, 0);
	tmpStr.Trim();
	runPCSet.m_Fire1 = pocketSet->m_Fire1 = tmpStr.GetLength() > 0 ? tmpStr : " ";
	COleDateTime tmpDate;
	tmpDate.ParseDateTime(m_grid.QuickGetText(1, 0), VAR_DATEVALUEONLY);
	runPCSet.m_Fire1Date = pocketSet->m_Fire1Date = (tmpDate.GetStatus() == COleDateTime::valid)
		? tmpDate : COleDateTime::GetCurrentTime();
	tmpStr = m_grid.QuickGetText(0, 1);
	tmpStr.Trim();
	runPCSet.m_Fire2 = pocketSet->m_Fire2 = tmpStr.GetLength() > 0 ? tmpStr : " ";
	tmpDate.ParseDateTime(m_grid.QuickGetText(1, 1), VAR_DATEVALUEONLY);
	runPCSet.m_Fire2Date = pocketSet->m_Fire2Date = (tmpDate.GetStatus() == COleDateTime::valid)
		? tmpDate : COleDateTime::GetCurrentTime();
	tmpStr = m_grid.QuickGetText(0, 2);
	tmpStr.Trim();
	runPCSet.m_Fire3 = pocketSet->m_Fire3 = tmpStr.GetLength() > 0 ? tmpStr : " ";
	tmpDate.ParseDateTime(m_grid.QuickGetText(1, 2), VAR_DATEVALUEONLY);
	runPCSet.m_Fire3Date = pocketSet->m_Fire3Date  = (tmpDate.GetStatus() == COleDateTime::valid)
		? tmpDate : COleDateTime::GetCurrentTime();
	runPCSet.m_Line1 = pocketSet->m_Line1 = m_Line1;
	runPCSet.m_Line2 = pocketSet->m_Line2 = m_Line2;
	runPCSet.m_Line3 = pocketSet->m_Line3 = m_Line3;
	runPCSet.m_NWCGStd = pocketSet->m_NWCGStd = m_bNWCGStd;
	runPCSet.m_RespAgency = pocketSet->m_RespAgency = m_strResponsibleAgency;
	CString fValStr = m_grid.QuickGetText(0, 2);
	if(fValStr.GetLength() > 0)
		runPCSet.m_Fire1Value = pocketSet->m_Fire1Value = atoi(fValStr);
	else
		runPCSet.m_Fire1Value = pocketSet->m_Fire1Value = 0;
	fValStr = m_grid.QuickGetText(1, 2);
	if(fValStr.GetLength() > 0)
		runPCSet.m_Fire2Value = pocketSet->m_Fire2Value = atoi(fValStr);
	else
		runPCSet.m_Fire2Value = pocketSet->m_Fire2Value = 0;
	fValStr = m_grid.QuickGetText(2, 2);
	if(fValStr.GetLength() > 0)
		runPCSet.m_Fire3Value = pocketSet->m_Fire3Value = atoi(fValStr);
	else
		runPCSet.m_Fire3Value = pocketSet->m_Fire3Value = 0;
	runPCSet.m_pcRyVal = pocketSet->m_pcRyVal;
	runPCSet.m_pcGyVal = pocketSet->m_pcGyVal;
	runPCSet.m_pcYVal = pocketSet->m_pcYVal;

	pocketSet->Update();
	runPCSet.Update();
	runPCSet.Close();
	CDialog::OnOK();
}

void CPocketCardDialog::OnBkgrndbutton()
{
	CPcBgDiaolg bd(this);
	bd.pSet = pocketSet;
	bd.DoModal();
}

double CPocketCardDialog::GetIndexValue(CClimAnalysis& analysis, COleDateTime iDate)
{
	int p = analysis.GetPeriod(iDate);
	int y = iDate.GetYear() - analysis.periods[m_IndexBox.GetItemData(m_IndexBox.GetCurSel())][0]->start.GetYear();
	if(analysis.periods[m_IndexBox.GetCurSel()][p]->yStats[y].N() > 0)
		return analysis.periods[m_IndexBox.GetItemData(m_IndexBox.GetCurSel())][p]->yStats[y].Mean();
	return -1.0;
}

void CPocketCardDialog::OnVarSort()
{
	// change the app's varSortOrder (and save to ffpOptions)
	// then re-sort and re-display

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;
	theApp.varSortOrder += 1;

	if (theApp.varSortOrder < 0 || theApp.varSortOrder > 2)
		theApp.varSortOrder = 0;

	CFFPOptsSet optsSet1(fpSet->m_pDatabase);
	optsSet1.Open();
	if(!optsSet1.IsEOF())
	{
			optsSet1.Edit();
			optsSet1.m_VarSortOrder = theApp.varSortOrder;
			optsSet1.Update();
	}
	optsSet1.Close();

	CClimateSet climSet(fpSet->m_pDatabase);
	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";

	climSet.setVarSortOrder(theApp.varSortOrder);

	climSet.Open();

	m_IndexBox.ResetContent();
	extraBox.ResetContent();

	int loc = 0;

	while (!climSet.IsEOF())
				{
					int eLoc = extraBox.AddString(climSet.m_Variable_Name);
					extraBox.SetItemData(eLoc, climSet.m_VarID);
					if(strncmp(climSet.m_Variable_Name, pocketSet->m_ExtraVar, strlen(climSet.m_Variable_Name)) == 0)
						extraBox.SetCurSel(eLoc);
					CString str = climSet.m_Variable_Name;
					int iLoc = m_IndexBox.AddString(str);
					m_IndexBox.SetItemData(iLoc,climSet.m_VarID - 1);
					if(str == m_Index)
					{
						//m_IndexBox.SetWindowText(str);
						m_IndexBox.SetCurSel(loc);
					}
					climSet.MoveNext();
					loc++;
				}

	climSet.Close();

	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}
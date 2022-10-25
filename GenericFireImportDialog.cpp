// GenericFireImportDialog.cpp : implementation file
//
#include "stdafx.h"
#include <math.h>
#include <direct.h>
#include <cderr.h>
#include <iostream>
#include <fstream>
#include <sstream>
 

#include "fireplus.h"
#include "Fireplusview.h"
#include "FireplusSet.h"
#include "FireplusDoc.h"
#include "SIGStationSet.h"


#include "FireAgencySet.h"
#include "FireRegionSet.h"
#include "FireUnitSet.h"
#include "FireSubUnitSet.h"
#include "DOIUnitSet.h"

#include "ImportDoneDialog.h"
#include "GenericFireImportDialog.h"

#include "CFiresSet.h"
#include ".\genericfireimportdialog.h"

#include "GenericFireFieldsSet.h"
#include "GenericFireImportSet.h"
using namespace std;   // needed for stream stuff.... std::<whatever> won't work :(

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;


#define FireFields  15
//#define defaultFireFields 0  
long GetFireRegion(CFireRegionSet *regs, long agencyID, char * find)
{
	switch(agencyID)
	{
	case 1://USFS
		regs->m_strFilter.Format("[AgencyID] = %ld and Left([Name], 2) = '%s'", agencyID, find);
		break;
	case 2://BIA
		regs->m_strFilter.Format("[AgencyID] = %ld and Left([Name], 3) = '%s'", agencyID, find);
		break;
	case 3://BLM
		regs->m_strFilter.Format("[AgencyID] = %ld and Left([Name], 2) = '%s'", agencyID, find);
		break;
	case 4://NPS
		//regs->m_strFilter.Format("[AgencyID] = %ld and Left([Name], 4) = '%s'", agencyID, find);
		regs->m_strFilter.Format("[AgencyID] = %ld and Left([Name], 2) = '%2.2s'", agencyID, find);
		break;
	case 5://FWS
		regs->m_strFilter.Format("[AgencyID] = %ld and Left([Name], 5) = '%s'", agencyID, find);
		break;
	default:
		regs->m_strFilter.Format("[AgencyID] = %ld and Left([Name], %d) = '%s'", agencyID, 
			strlen(find), find);
	}
	regs->Requery();
	if(!regs->IsEOF() && !regs->IsBOF())
		return regs->m_RegionID;
	return 0;
}

long GetFireUnit(CFireUnitSet *units, long agencyID, long reg, char * temp)
{
	switch(agencyID)
	{
	case 1://USFS
		units->m_strFilter.Format("[RegionID] = %ld and Left([Name], 2) = '%s'", reg, temp);
		break;
	case 2://BIA
		units->m_strFilter.Format("[RegionID] = %ld and Left([Name], 4) = '%s'", reg, temp);
		break;
	case 3://BLM
		units->m_strFilter.Format("[RegionID] = %ld and Left([Name], 5) = '%s'", reg, temp);
		break;
	case 4://NPS
		units->m_strFilter.Format("[RegionID] = %ld and Left([Name], 4) = '%s'", reg, temp);
		break;
	case 5://FWS
		units->m_strFilter.Format("[RegionID] = %ld and Left([Name], 5) = '%s'", reg, temp);
		break;
	default:
		units->m_strFilter.Format("[RegionID] = %ld and Left([Name], %d) = '%s'", reg, 
			strlen(temp), temp);
	}
	units->Requery();
	if(!units->IsEOF() && !units->IsBOF())
		return units->m_UnitID;
	return 0;
}

long GetFireSubunit(CFireSubunitSet *subs, long agencyID, long unit, char * temp)
{
	switch(agencyID)
	{
	case 1://USFS
		subs->m_strFilter.Format("[UnitID] = %ld and Left([Name], 2) = '%s'", unit, temp);
		break;
	case 2:
	case 3:
	case 4:
	case 5:
		break;
	default:
		subs->m_strFilter.Format("[UnitID] = %ld and Left([Name], %d) = '%s'", unit, 
			strlen(temp), temp);
	}
	subs->Requery();
	if(!subs->IsEOF() && !subs->IsBOF())
		return subs->m_SubunitID;
	return 0;
}

long AddFireUnit(CFireUnitSet *units,long agencyID, long reg, char * temp)
{	//need to add one
	//subs->m_strFilter.Format("[UnitID] = %ld", unit);
	units->m_strFilter = "";//.Format("[UnitID] = %ld",);
	units->m_strSort = "[UnitID]";
	units->Requery();
	long tID = 1;
	if(!units->IsEOF() && !units->IsBOF())
	{
		units->MoveLast();
		tID = units->m_UnitID + 1;
	}
	units->AddNew();
	units->m_UnitID = tID;
	units->m_RegionID = reg;
	//how many chars to format?
	int nChars;
	switch(agencyID)
	{
	case 1://USFS
		nChars = 2;
		break;
	case 2://BIA
		nChars = 4;
		//units->m_strFilter.Format("[RegionID] = %ld and Left([Name], 4) = '%s'", reg, temp);
		break;
	case 3://BLM
		nChars = 5;
		//units->m_strFilter.Format("[RegionID] = %ld and Left([Name], 5) = '%s'", reg, temp);
		break;
	case 4://NPS
		nChars = 4;
		//units->m_strFilter.Format("[RegionID] = %ld and Left([Name], 4) = '%s'", reg, temp);
		break;
	case 5://FWS
		nChars = 5;
		//units->m_strFilter.Format("[RegionID] = %ld and Left([Name], 5) = '%s'", reg, temp);
		break;
	default:
		nChars = strlen(temp);
		//units->m_strFilter.Format("[RegionID] = %ld and Left([Name], %d) = '%s'", reg, 
		//	strlen(temp), temp);
	}
	// is the new unit numeric only?
	bool numericOnly = TRUE;
	for (int i=0; i < strlen(temp); i++)
		if (temp[i] < 48 || temp[i] > 57)
			numericOnly = FALSE;
	if (numericOnly)
	{
		CString tStr;
		tStr.Format("%%%ds Unknown", nChars);
		units->m_Name.Format(tStr, temp);
	} 
	else
	   units->m_Name.Format("%2s", temp);
	
	
	if (units->m_Name.GetLength() > 40)
		units->m_Name.SetAt(40,0);

	units->Update();
	return tID;	
}

long AddFireSubUnit(CFireSubunitSet *subs, long unit, char * temp)
{	//need to add one
	//subs->m_strFilter.Format("[UnitID] = %ld", unit);
	subs->m_strFilter = "";//.Format("[UnitID] = %ld",);
	subs->m_strSort = "[SubunitID]";
	subs->Requery();
	long tID = 1;
	if(!subs->IsEOF() && !subs->IsBOF())
	{
		subs->MoveLast();
		tID = subs->m_SubunitID + 1;
	}
	subs->AddNew();
	// is the new unit numeric only?
	bool numericOnly = TRUE;
	for (int i=0; i < strlen(temp); i++)
		if (temp[i] < 48 || temp[i] > 57)
			numericOnly = FALSE;
	if (numericOnly)
	   subs->m_Name.Format("%2s Unknown", temp);
	else
	   subs->m_Name.Format("%2s", temp);
	if (subs->m_Name.GetLength() > 40)
		subs->m_Name.SetAt(40,0);

	subs->m_UnitID = unit;
	subs->m_SubunitID = tID;
	subs->Update();
	return tID;	
}

char *FireColNames[FireFields] = 
{
	"Region ID *",
	"Unit ID",
	"Subunit ID",
	"Discovery Date *",
	"Fire Number *",
	"Total Acres *",
	"Statistical Cause *",
	"Fire Name",
	"Latitude",
	"Longitude",
	//"State",
	//"County",
	//"Township",
	//"Range",
	//"Section",
	//"Subsection",
	//"Fire Type",
	//"Slope",
	//"Elevation",
	//"Aspect",
	//"Fuel Model",
	"Discovery Time",
	//"First Attack Date",
	//"First Attack Time",
	//"General Cause",
	//"Specific Cause",
	//"Class of People",
	//"Cause Narrative",
	"Containment Date",
	"Containment Time",
	"Strategy Met Date",
	"Strategy Met Time",
	//"Fire Out Date",
	//"Fire Out Time"

};

char *FireFieldNames[FireFields] = 
{
	"RegionID", //4
	"UnitID", //5
	"SubunitID", //6
	"DiscoveryDate", //0
	"FireNumber", //7
	"TotalAcres", //10
	"StatisticalCause", //12
	"FireName", //11
	"Latitude", //21
	"Longitude", //22
	//"State", //8
	//"County", //9
	//"Township", //17
	//"Range", //18
	//"Section", //19
	//"SubSection", //20
	//"FireType", //27
	//"Slope", //23
	//"Elevation", //24
	//"Aspect", //25
	//"FuelModel", //26
	"DiscoveryTime", //1
	//"FirstAttackDate", // 2
	//"FirstAttackTime", //3
	//"GeneralCause", //13
	//"SpecificCause", //14
	//"ClassPeople", //15
	//"CauseNarr", //16
	"ContainDate", //28
	"ContainTime", //29
	"StrategyMetDate", //30
	"StrategyMetTime", //31
	//"FireOutDate", //32
	//"FireOutTime" //33
};



/*char *FireColNames[FireFields] = 
{
	//"Fire ID",
	"Discovery Date *",
	"Discovery Time",
	"First Attack Date",
	"First Attack Time",
	//"Agency ID",
	"Region ID *",
	"Unit ID",
	"Subunit ID",
	"Fire Number *",
	"State",
	"County",
	"Total Acres *",
	"Fire Name",
	"Statistical Cause *",
	"General Cause",
	"Specific Cause",
	"Class of People",
	"Cause Narrative",
	"Township",
	"Range",
	"Section",
	"Subsection",
	"Latitude",
	"Longitude",
	"Slope",
	"Elevation",
	"Aspect",
	"Fuel Model",
	"Fire Type",
	"Containment Date",
	"Containment Time",
	"Strategy Met Date",
	"Strategy Met Time",
	"Fire Out Date",
	"Fire Out Time"

};

char *FireFieldNames[FireFields] = 
{
	"DiscoveryDate", //0
	"DiscoveryTime", //1
	"FirstAttackDate", // 2
	"FirstAttackTime", //3
	"RegionID", //4
	"UnitID", //5
	"SubunitID", //6
	"FireNumber", //7
	"State", //8
	"County", //9
	"TotalAcres", //10
	"FireName", //11
	"StatisticalCause", //12
	"GeneralCause", //13
	"SpecificCause", //14
	"ClassPeople", //15
	"CauseNarr", //16
	"Township", //17
	"Range", //18
	"Section", //19
	"SubSection", //20
	"Latitude", //21
	"Longitude", //22
	"Slope", //23
	"Elevation", //24
	"Aspect", //25
	"FuelModel", //26
	"FireType", //27
	"ContainDate", //28
	"ContainTime", //29
	"StrategyMetDate", //30
	"StrategyMetTime", //31
	"FireOutDate", //32
	"FireOutTime" //33
};
*/
/////////////////////////////////////////////////////////////////////////////
// CGenericFireImportDialog dialog

CGenericFireImportDialog::CGenericFireImportDialog(CWnd* pParent /*=NULL*/, CFiresSet *_records /*=NULL*/, CDatabase *_pDB, long _agencyID)
	: CDialog(CGenericFireImportDialog::IDD, pParent),regSet(_pDB), unitSet(_pDB), subSet(_pDB)
	, m_RejectDups(FALSE)
{
	//{{AFX_DATA_INIT(CGenericFireImportDialog)
	//}}AFX_DATA_INIT
	records = _records;
	agencyID = _agencyID;
	pDB = _pDB;

	

}


void CGenericFireImportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenericFireImportDialog)
	DDX_Control(pDX, IDC_TIME1, time1);
	DDX_Control(pDX, IDC_TIME2, time2);
	DDX_Control(pDX, IDC_SPACE, space);
	DDX_Control(pDX, IDC_COMMA, comma);
	DDX_Control(pDX, IDC_SELECTEDLIST, selectedList);
	DDX_Control(pDX, IDC_FIELDSLIST, fieldsList);
	DDX_Control(pDX, IDC_DATE1, date1);
	DDX_Control(pDX, IDC_DATE2, date2);
	DDX_Control(pDX, IDC_DATE3, date3);

	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_USEREG, m_useRegion);
	DDX_Control(pDX, IDC_USEUNIT, m_useUnit);
	DDX_Control(pDX, IDC_USESUBUNIT, m_useSubunit);
	DDX_Control(pDX, IDC_REGS, m_comboRegions);
	DDX_Control(pDX, IDC_UNITS, m_comboUnits);
	DDX_Control(pDX, IDC_SUBUNITS, m_comboSubunits);
	DDX_Radio(pDX, IDC_DUP_OVERWRITE, m_RejectDups);
	DDX_Control(pDX, IDC_IMPORT_FIRE, m_ImportButton);
}


BEGIN_MESSAGE_MAP(CGenericFireImportDialog, CDialog)
	//{{AFX_MSG_MAP(CGenericFireImportDialog)
	ON_BN_CLICKED(IDC_DATE1, OnDate1)
	ON_BN_CLICKED(IDC_DATE2, OnDate2)
	ON_BN_CLICKED(IDC_DATE3, OnDate3)
	//ON_BN_CLICKED(IDC_DATE4, OnDate4)
	//ON_BN_CLICKED(IDC_DATE5, OnDate5)
	ON_BN_CLICKED(IDC_TIME1, OnTime1)
	ON_BN_CLICKED(IDC_TIME2, OnTime2)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_BOTTOM, OnBottom)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_LBN_DBLCLK(IDC_FIELDSLIST, OnDblclkFieldslist)
	ON_LBN_DBLCLK(IDC_SELECTEDLIST, OnDblclkSelectedlist)
	ON_BN_CLICKED(IDC_MOVELEFT, OnMoveleft)
	ON_BN_CLICKED(IDC_MOVERIGHT, OnMoveright)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_REMOVEALL, OnRemoveall)
	ON_BN_CLICKED(IDC_IMPORT_FIRE, OnGenericFire)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_USEREG, OnBnClickedUseReg)
	ON_CBN_SELCHANGE(IDC_REGS, OnCbnSelchangeRegs)
	ON_BN_CLICKED(IDC_USEUNIT, OnBnClickedUseUnit)
	ON_CBN_SELCHANGE(IDC_UNITS, OnCbnSelchangeUnits)
	ON_BN_CLICKED(IDC_USESUBUNIT, OnBnClickedUseSubunit)
	ON_CBN_SELCHANGE(IDC_SUBUNITS, OnCbnSelchangeSubunits)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenericFireImportDialog message handlers

BOOL CGenericFireImportDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString importBtnStr;
	char fNameTmp[MAX_PATH];
	strcpy_s(fNameTmp, fileName);
	for(int i = strlen(fNameTmp) - 1; i > 0; i--)
	{
		if(fNameTmp[i] == '\\')
		{
			strcpy_s(fNameTmp, &fNameTmp[i]);
			break;
		}
	}
	errors = 0;
	importBtnStr.Format("Import Fires from: ...%s", fNameTmp);
	m_ImportButton.SetWindowText(importBtnStr);
	dt = Full;
	tt = FullTime;
	unitSet.Open();
	subSet.Open();
	regSet.m_strFilter.Format("[AgencyID] = %ld", agencyID);
	regSet.Open();
	int loc;
	while(!regSet.IsEOF())
	{
		loc = m_comboRegions.AddString(regSet.m_Name);
		if(loc >= 0)
			m_comboRegions.SetItemData(loc, regSet.m_RegionID);
		regSet.MoveNext();
	}
 

	date1.SetCheck(TRUE);


	// all fields to available list
	for(int f = 0; f < FireFields; f++){
		   // other fields - not selected by default 
		   loc = fieldsList.AddString(FireColNames[f]);
		   fieldsList.SetItemData(loc, f);
	}

	upButton.AutoLoad(IDC_UP, this);
	downButton.AutoLoad(IDC_DOWN, this);
	topButton.AutoLoad(IDC_TOP, this);
	bottomButton.AutoLoad(IDC_BOTTOM, this);
	/*if ( dt == Packed)
        date2.SetCheck(TRUE);
	else if (dt == Reverse)
		date3.SetCheck(TRUE);*/

	
	if (tt == FullTime)
	{
		time1.SetCheck(TRUE);
        time2.SetCheck(FALSE);
	}
	else
	{
        time1.SetCheck(FALSE);
        time2.SetCheck(TRUE);
	}

	
	
	CButton *button = (CButton *)GetDlgItem(IDC_COMMA);
	button->SetCheck(TRUE);

	//look for settings from previous agency import
	CGenericFireImportSet gfiSet(pDoc->m_pDB);
	gfiSet.m_strFilter.Format("[AgencyID] = %ld", agencyID);
	gfiSet.Open();
	if(!gfiSet.IsEOF())
	{
		if(gfiSet.m_Delimiter == 0)
		{
			space.SetCheck(TRUE);
			comma.SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_NONE))->SetCheck(FALSE);
		}
		else if(gfiSet.m_Delimiter == 1)
		{
			space.SetCheck(FALSE);
			comma.SetCheck(TRUE);
			((CButton *)GetDlgItem(IDC_NONE))->SetCheck(FALSE);
		}
		else//semicolon
		{
			space.SetCheck(FALSE);
			comma.SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_NONE))->SetCheck(TRUE);
		}
		if(	gfiSet.m_DateFormat == 0)
		{
			((CButton *)GetDlgItem(IDC_DATE1))->SetCheck(TRUE);
			((CButton *)GetDlgItem(IDC_DATE2))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_DATE3))->SetCheck(FALSE);
			dt = Full;
		}
		else if(gfiSet.m_DateFormat == 1)
		{
			((CButton *)GetDlgItem(IDC_DATE1))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_DATE2))->SetCheck(TRUE);
			((CButton *)GetDlgItem(IDC_DATE3))->SetCheck(FALSE);
			dt = Packed;
		}
		else
		{
			((CButton *)GetDlgItem(IDC_DATE1))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_DATE2))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_DATE3))->SetCheck(TRUE);
			dt = Reverse;
		}
		if(gfiSet.m_TimeFormat == 0)
		{
			((CButton *)GetDlgItem(IDC_TIME1))->SetCheck(TRUE);
			((CButton *)GetDlgItem(IDC_TIME2))->SetCheck(FALSE);
			tt = TimeType::FullTime;
		}
		else
		{
			((CButton *)GetDlgItem(IDC_TIME1))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_TIME2))->SetCheck(TRUE);
			tt = TimeType::PackedTime;
		}
		m_RejectDups = gfiSet.m_DupHandling;
	}
	gfiSet.Close();
	CGenericFireFieldsSet gffSet(pDoc->m_pDB);
	gffSet.m_strFilter.Format("[AgencyID] = %ld", agencyID);
	gffSet.m_strSort = "[Rank]";
	gffSet.Open();
	while(!gffSet.IsEOF())
	{
		CString str;
		int find = gffSet.m_FieldNum;//fieldsList.GetCurSel();
		for(int i = 0; i < fieldsList.GetCount(); i++)
		{
			int data = fieldsList.GetItemData(i);
			if(data == find)
			{
				fieldsList.GetText(i, str);
				//int val = fieldsList.GetItemData(cur);
				int loc = selectedList.AddString(str);
				selectedList.SetItemData(loc, find);
				fieldsList.DeleteString(i);
				break;
			}
		}
		gffSet.MoveNext();
	}
	gffSet.Close();
/*	if (delim != ' '){
		if (delim == ','){
			  CButton *button = (CButton *)GetDlgItem(IDC_COMMA);
			  button->SetCheck(TRUE);
		} else if (delim == '\t'){
			  CButton *button = (CButton *)GetDlgItem(IDC_SPACE);
			  button->SetCheck(TRUE);
		} else {
			  CButton *button = (CButton *)GetDlgItem(IDC_NONE);
			  button->SetCheck(TRUE);
		}


	}*/
	//UpdateData(FALSE);
	//((CButton *)GetDlgItem(IDC_DUP_OVERWRITE))->SetCheck(m_RejectDups);
	//((CButton *)GetDlgItem(IDC_DUP_REJECT))->SetCheck(!m_RejectDups);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGenericFireImportDialog::OnDate1() 
{
	dt = Full;
}

void CGenericFireImportDialog::OnDate2() 
{
	dt = Packed;
}

void CGenericFireImportDialog::OnDate3() 
{
	dt = Reverse;
}

/*void CGenericFireImportDialog::OnDate4() 
{
	dt = MD;
}

void CGenericFireImportDialog::OnDate5() 
{
	dt = MDPacked;
}*/

void CGenericFireImportDialog::OnTime1() 
{
	tt = FullTime;
}

void CGenericFireImportDialog::OnTime2() 
{
	tt = PackedTime;
}

void CGenericFireImportDialog::OnTop() 
{
	CWaitCursor wait;
	if(selectedList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = selectedList.GetCurSel();
		if(cur > 0)
		{
			selectedList.GetText(cur, str);
			int val = selectedList.GetItemData(cur);
			selectedList.DeleteString(cur);
			int loc = selectedList.InsertString(0, str);
			selectedList.SetItemData(loc, val);
			selectedList.SetCurSel(loc);
		}
	}
}

void CGenericFireImportDialog::OnUp() 
{
	CWaitCursor wait;
	if(selectedList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = selectedList.GetCurSel();
		if(cur > 0)
		{
			selectedList.GetText(cur, str);
			int val = selectedList.GetItemData(cur);
			selectedList.DeleteString(cur);
			cur--;
			int loc = selectedList.InsertString(cur, str);
			selectedList.SetItemData(loc, val);
			selectedList.SetCurSel(loc);
		}
	}
}

void CGenericFireImportDialog::OnBottom() 
{
	CWaitCursor wait;
	if(selectedList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = selectedList.GetCurSel();
		if(cur < selectedList.GetCount() - 1)
		{
			selectedList.GetText(cur, str);
			int val = selectedList.GetItemData(cur);
			selectedList.DeleteString(cur);
			int loc = selectedList.InsertString(-1, str);
			selectedList.SetItemData(loc, val);
			selectedList.SetCurSel(loc);
		}
	}
}

void CGenericFireImportDialog::OnDown() 
{
	CWaitCursor wait;
	if(selectedList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = selectedList.GetCurSel();
		if(cur < selectedList.GetCount() - 1)
		{
			selectedList.GetText(cur, str);
			int val = selectedList.GetItemData(cur);
			selectedList.DeleteString(cur);
			cur++;
			int loc = selectedList.InsertString(cur, str);
			selectedList.SetItemData(loc, val);
			selectedList.SetCurSel(loc);
		}
	}
}

void CGenericFireImportDialog::OnDblclkFieldslist() 
{
	CString str;
	int cur = fieldsList.GetCurSel();
	fieldsList.GetText(cur, str);
	int val = fieldsList.GetItemData(cur);
	int loc = selectedList.AddString(str);
	selectedList.SetItemData(loc, val);
	fieldsList.DeleteString(cur);
	
	

	
}

void CGenericFireImportDialog::OnDblclkSelectedlist() 
{
	CString str;
	int cur = selectedList.GetCurSel();
	selectedList.GetText(cur, str);
	int val = selectedList.GetItemData(cur);
	int loc = fieldsList.AddString(str);
	fieldsList.SetItemData(loc, val);
	selectedList.DeleteString(cur);
}

void CGenericFireImportDialog::OnMoveleft() 
{
	if(selectedList.GetCurSel() != LB_ERR)
		OnDblclkSelectedlist();
}

void CGenericFireImportDialog::OnMoveright() 
{
	if(fieldsList.GetCurSel() != LB_ERR)
		OnDblclkFieldslist();
}

void CGenericFireImportDialog::OnSelectall() 
{
	while(fieldsList.GetCount() > 0)
	{
		fieldsList.SetCurSel(0);
		OnDblclkFieldslist();
	}
}

void CGenericFireImportDialog::OnRemoveall() 
{
	while(selectedList.GetCount() > 0)
	{
		selectedList.SetCurSel(0);
		OnDblclkSelectedlist();
	}	
}




void CGenericFireImportDialog::OnGenericFire(){

	UpdateData();
	

	long defaultRegion = -1, defaultUnit = -1, defaultSubunit = -1;
	if(m_useRegion.GetCheck() != FALSE)
	{
		int selLoc = m_comboRegions.GetCurSel();
		if(selLoc != CB_ERR)
			defaultRegion = m_comboRegions.GetItemData(selLoc);
	}
	if(m_useUnit.GetCheck() != FALSE)
	{
		int selLoc = m_comboUnits.GetCurSel();
		if(selLoc != CB_ERR)
			defaultUnit = m_comboUnits.GetItemData(selLoc);
	}
	if(m_useSubunit.GetCheck() != FALSE)
	{
		int selLoc = m_comboSubunits.GetCurSel();
		if(selLoc != CB_ERR)
			defaultSubunit = m_comboSubunits.GetItemData(selLoc);
	}

    errors = 0;
	long dupRejects = 0;
	CWaitCursor wait;
	
	fprintf(errLog, "**************************************************\nImport File Name: %s\n", fileName);
	 EnableWindow(FALSE);
	/*CCancelDialog *cd;

	cd = new CCancelDialog();

	if (cd != NULL)
	{
		BOOL ret = cd->Create(IDD_CANCELDIALOG,this);

      if (!ret)   //Create failed.
      {
        
		
		 cd = 0;
      } else
	  {
	     cd->Reset();
         cd->ShowWindow(SW_SHOW);
	     cd->SetStatusString("Reading: " + fileName);
		 cd->SetTitle("Importing Generic Fire Data");
	  }
	}*/
	
	
	
	
	ifstream stream;

	stream.open(fileName);

	long count = 0, updates = 0, rejects = 0;
    int lastRegionID = 0;
	//int lastFireID = 0;
    
    char line[MAX_INPUT_LINE];
   
    CString temp2, temp3;
    stringstream stream2;
    
    
	char delim;

	delim = '\t';//default to tab
	CButton *button = (CButton *)GetDlgItem(IDC_COMMA);
	if(button->GetCheck() == TRUE)
		delim = ',';
	else
	{
		button = (CButton *)GetDlgItem(IDC_NONE);
		if(button->GetCheck() == TRUE)
			delim = ';';
	}
   

	if(pDoc->HasTempFire())
	{
		try
		{
			CString strSql = "DROP TABLE [ffpTempFire]";
			pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		
		//pDoc->m_pDB->DeleteTableDef("ffpTempFire");
	}
	pDoc->CreateTempFire();

    CList<CString, CString> staList;

	CFiresSet fires(pDB);
	fires.m_strSort = "[FireNumber]";
	fires.Open();

	CFiresSet tfires(pDB);
	tfires.m_strSort = "[FireNumber]";
	tfires.Open(CRecordset::dynaset, "ffpTempFire");



	

	while (stream.good())// && (!cd || ! cd->Abort()))
	{

		 
	   char sDiscoveryDate[MAX_FIELD_LEN] = "",
		   sDiscoveryTime[MAX_FIELD_LEN] = "",
		   sFirstAttackDate[MAX_FIELD_LEN] = "",
		   sFirstAttackTime[MAX_FIELD_LEN] = "",
		   //sAgencyID[MAX_FIELD_LEN] = "",
		   sRegionID[MAX_FIELD_LEN] = "",
		   sUnitID[MAX_FIELD_LEN] = "",
		   sSubunitID[MAX_FIELD_LEN] = "",
		   sFireNumber[MAX_FIELD_LEN] = "",
		   sState[MAX_FIELD_LEN] = "",
		   sCounty[MAX_FIELD_LEN] = "",
		   sTotalAcres[MAX_FIELD_LEN] = "",
		   sFireName[MAX_FIELD_LEN] = "",
		   sStatisticalCause[MAX_FIELD_LEN] = "",
		   sGeneralCause[MAX_FIELD_LEN] = "",
		   sSpecificCause[MAX_FIELD_LEN] = "",
		   sClassPeople[MAX_FIELD_LEN] = "",
		   sCauseNarr[MAX_FIELD_LEN] = "",
		   sTownship[MAX_FIELD_LEN] = "",
		   sRange[MAX_FIELD_LEN] = "",
		   sSection[MAX_FIELD_LEN] = "",
		   sSubSection[MAX_FIELD_LEN] = "",
		   sLatitude[MAX_FIELD_LEN] = "",
		   sLongitude[MAX_FIELD_LEN] = "",
		   sSlope[MAX_FIELD_LEN] = "",
		   sElevation[MAX_FIELD_LEN] = "",
		   sAspect[MAX_FIELD_LEN] = "",
		   sFuelModel[MAX_FIELD_LEN] = "",
		   sFireType[MAX_FIELD_LEN] = "",
		   sContainDate[MAX_FIELD_LEN] = "",
		   sContainTime[MAX_FIELD_LEN] = "",
		   sStrategyMetDate[MAX_FIELD_LEN] = "",
		   sStrategyMetTime[MAX_FIELD_LEN] = "",
		   sFireOutDate[MAX_FIELD_LEN] = "",
		   sFireOutTime[MAX_FIELD_LEN] = "",
           sComplete[MAX_INPUT_LINE] = "";

	   
	   
	    stream.getline(line,MAX_INPUT_LINE,'\n');
        
		if (line[0] == '#' || line[0] == '\n' || line[0] == '\r'){ // not an input line
			continue;
		}

		// 2014 - quick scan for quoted records
		bool inQuote = false;
		for (int i=0; i <strlen(line);i++)
		{
			if (line[i] == '\0')
				exit;

			if (line[i] == '"')
				inQuote = !inQuote;
			
			// change embedded comma to semicolon
			if (line[i] == ',' && inQuote)
				line[i] = ';';

		}
		

        stringstream stream2;

		stream2 << line;

		


		for(int f = 0; f < selectedList.GetCount(); f++)
		{
			
			int loc = selectedList.GetItemData(f);

			switch(loc)
			{
			

			//case 0:
				 //stream2.getline(sFireID,MAX_FIELD_LEN,delim); break;
			case 3:
				 stream2.getline(sDiscoveryDate,MAX_FIELD_LEN,delim); break;
			case 21:
				 stream2.getline(sDiscoveryTime,MAX_FIELD_LEN,delim); break;
			case 22:
				 stream2.getline(sFirstAttackDate,MAX_FIELD_LEN,delim); break;
			case 23:
				 stream2.getline(sFirstAttackTime,MAX_FIELD_LEN,delim); break;
			case 0:
				 stream2.getline(sRegionID,MAX_FIELD_LEN,delim); break;
            case 1:
				 stream2.getline(sUnitID,MAX_FIELD_LEN,delim); break;
            case 2:
				 stream2.getline(sSubunitID,MAX_FIELD_LEN,delim); break;
            case 4:
				 stream2.getline(sFireNumber,MAX_FIELD_LEN,delim); break;				 
            case 10:
				 stream2.getline(sState,MAX_FIELD_LEN,delim); break;				 
            case 11:
				 stream2.getline(sCounty,MAX_FIELD_LEN,delim); break;
			case 5:
				 stream2.getline(sTotalAcres,MAX_FIELD_LEN,delim); break;
            case 7:
				 stream2.getline(sFireName,MAX_FIELD_LEN,delim); break;
            case 6:
				 stream2.getline(sStatisticalCause,MAX_FIELD_LEN,delim); break;
            case 24:
				 stream2.getline(sGeneralCause,MAX_FIELD_LEN,delim); break;
            case 25:
				 stream2.getline(sSpecificCause,MAX_FIELD_LEN,delim); break;
            case 26:
				 stream2.getline(sClassPeople,MAX_FIELD_LEN,delim); break;
            case 27:
				 stream2.getline(sCauseNarr,MAX_FIELD_LEN,delim); break;
            case 12:
				 stream2.getline(sTownship,MAX_FIELD_LEN,delim); break;
            case 13:
				 stream2.getline(sRange,MAX_FIELD_LEN,delim); break;
            case 14:
				 stream2.getline(sSection,MAX_FIELD_LEN,delim); break;
            case 15:
				 stream2.getline(sSubSection,MAX_FIELD_LEN,delim); break;
            case 8:
				 stream2.getline(sLatitude,MAX_FIELD_LEN,delim); break;
            case 9:
				 stream2.getline(sLongitude,MAX_FIELD_LEN,delim); break;
            case 17:
				 stream2.getline(sSlope,MAX_FIELD_LEN,delim); break;
            case 18:
				 stream2.getline(sElevation,MAX_FIELD_LEN,delim); break;
            case 19:
				 stream2.getline(sAspect,MAX_FIELD_LEN,delim); break;
            case 20:
				 stream2.getline(sFuelModel,MAX_FIELD_LEN,delim); break;
		    case 16:
				 stream2.getline(sFireType,MAX_FIELD_LEN,delim); break;
		    case 28:
				 stream2.getline(sContainDate,MAX_FIELD_LEN,delim); break;
		    case 29:
				 stream2.getline(sContainTime,MAX_FIELD_LEN,delim); break;
		    case 30:
				 stream2.getline(sStrategyMetDate,MAX_FIELD_LEN,delim); break;
            case 31:
				 stream2.getline(sStrategyMetTime,MAX_FIELD_LEN,delim); break;
		    case 32:
				 stream2.getline(sFireOutDate,MAX_FIELD_LEN,delim); break;
			case 33:
				 stream2.getline(sFireOutTime,MAX_FIELD_LEN,delim); break;
			default: break;
			}
		}
		
		// get any additional data from end of record...
		stream2.getline(sComplete,MAX_INPUT_LINE,delim);





		
		count ++;
		//if (cd)
       //    cd->Increment();

 		COleDateTime discoveryDate;  

		if (strlen(sFireNumber) < 1){
            fprintf(errLog, "\tRecord Number %d, no FireID or FireNumber\n", count );
			rejects ++;
			continue;
		}

		if (strlen(sDiscoveryDate) < 1){
            fprintf(errLog, "\tRecord Number %d, no Discovery date\n", count );
			rejects ++;
			continue;
		}

		if (strlen(sDiscoveryDate) < 8){
            fprintf(errLog, "\tRecord Number %d, Discovery date is not valid.\n", count );
			rejects ++;
			continue;
		}

		// format date & time properly
        int y=0,
			m=0,
			d=0,
			hr=13,
			min=0;


        char tdate[5];
		if (dt == Packed){ // MMDDYYYY
	      tdate[0] = sDiscoveryDate[0];
		  tdate[1] = sDiscoveryDate[1];
          tdate[2] = '\0';
		  m = atoi(tdate);
	      
		  tdate[0] = sDiscoveryDate[2];
		  tdate[1] = sDiscoveryDate[3];
          tdate[2] = '\0';
		  d = atoi(tdate);
		
		  tdate[0] = sDiscoveryDate[4];
		  tdate[1] = sDiscoveryDate[5];
	      tdate[2] = sDiscoveryDate[6];
		  tdate[3] = sDiscoveryDate[7];
          tdate[4] = '\0';
		  y = atoi(tdate);
		 


		} else if (dt == Reverse){ // YYYYMMDD

				
		  tdate[0] = sDiscoveryDate[0];
		  tdate[1] = sDiscoveryDate[1];
	      tdate[2] = sDiscoveryDate[2];
		  tdate[3] = sDiscoveryDate[3];
          tdate[4] = '\0';
		  y = atoi(tdate);

	      tdate[0] = sDiscoveryDate[4];
		  tdate[1] = sDiscoveryDate[5];
          tdate[2] = '\0';
		  m = atoi(tdate);
	      
		  tdate[0] = sDiscoveryDate[6];
		  tdate[1] = sDiscoveryDate[7];
          tdate[2] = '\0';
		  d = atoi(tdate);
	
		} else 
		{  // MM/DD/YYYY
			COleDateTime discDT;
			discDT.ParseDateTime(sDiscoveryDate);
			if(discDT.GetStatus() != COleDateTime::valid)
			{
				fprintf(errLog, "\tRecord Number %d, Discovery date is not valid.\n", count );
				rejects ++;
				continue;
			}
			m = discDT.GetMonth();//atoi(tdate);
			d = discDT.GetDay();//atoi(tdate);
			y = discDT.GetYear();//atoi(tdate);
	     // tdate[0] = sDiscoveryDate[0];
		 // tdate[1] = sDiscoveryDate[1];
          //tdate[2] = '\0';
	      
		  //tdate[0] = sDiscoveryDate[3];
		 // tdate[1] = sDiscoveryDate[4];
         // tdate[2] = '\0';
		
		  //tdate[0] = sDiscoveryDate[6];
		  //tdate[1] = sDiscoveryDate[7];
	     // tdate[2] = sDiscoveryDate[8];
		 // tdate[3] = sDiscoveryDate[9];
         // tdate[4] = '\0';

		}


		if (y < 1900 || y > 2100){
            fprintf(errLog, "\tRecord Number %d, Discovery date is not valid (bad year).\n", count );
			rejects ++;
			continue;
		}
		if (m < 1 || m > 12){
            fprintf(errLog, "\tRecord Number %d, Discovery date is not valid (bad month).\n", count );
			rejects ++;
			continue;
		}
		if (d < 1 || d > 31){
            fprintf(errLog, "\tRecord Number %d, Discovery date is not valid (bad day).\n", count );
			rejects ++;
			continue;
		}

		if (strlen(sDiscoveryTime) > 0){
           if (strlen(sDiscoveryTime) <= 2)
			   hr = atoi(sDiscoveryTime);
		   else if (strlen(sDiscoveryTime)==3 || strlen(sDiscoveryTime) > 5) {
               fprintf(errLog, "\tRecord Number %d, DiscoveryTime is not valid (bad format).\n", count );
			   rejects ++;
			   continue;
		   } else {   // normal format
			   if (tt == FullTime){  // HH:MM
                    		
		          tdate[0] = sDiscoveryTime[0];
		          tdate[1] = sDiscoveryTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sDiscoveryTime[3];
		          tdate[1] = sDiscoveryTime[4];
                  tdate[2] = '\0';
		          min = atoi(tdate);
			   } else { // HHMM
                  tdate[0] = sDiscoveryTime[0];
		          tdate[1] = sDiscoveryTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sDiscoveryTime[2];
		          tdate[1] = sDiscoveryTime[3];
                  tdate[2] = '\0';
		          min = atoi(tdate);
		       }
		   }

		}
		if (hr < 0 || hr > 23){
               fprintf(errLog, "\tRecord Number %d, Discovery time is not valid (bad hour).\n", count );
			   rejects ++;
			   continue;
		}
		if (min < 0 || min > 59){
		       fprintf(errLog, "\tRecord Number %d, Discovery time is not valid (bad minute).\n", count );
			   rejects ++;
			   continue;
		}

	
		discoveryDate.SetDateTime(y, m, d, hr, min, 0);  

		

		if(discoveryDate.GetStatus() != 0 || y < 1900)
		{
			fprintf(errLog, "\tRecord Number %ld, Discovery Date/Time is not valid: %d/%d/%04d\n", count ,  m, d, y);
			rejects ++;
			
			continue;
		}
		COleDateTime today;
		today = COleDateTime::GetCurrentTime();
		if(discoveryDate > today)
		{
			fprintf(errLog, "\tRecord Number %ld, Discovery Date/Time is not valid (future): %d/%d/%04d\n", count,  m, d, y);
			rejects ++;
			
			continue;
		}

        tfires.AddNew();


		// calculate fireID if necessary
		

		/*if (lastFireID > 0)
		{
			lastFireID ++;
			//sprintf(sFireID,"%d",lastFireID);
		} 
		else 
		{
			CFiresSet fires(pDB);
			//fires.m_strSort.Format("[FireID]");
			fires.Open();
			if (! fires.IsEOF())
			{
				fires.MoveLast();
				lastFireID = fires.m_FireID + 1;
				//sprintf(sFireID,"%d",fires.m_FireID + 1);
			} 
			else 
			{
				//sprintf(sFireID,"1");
				lastFireID = 1;
			}
            fires.Close();
		}
		tfires.m_Year.Format("%d",y);

		//if (strlen(sFireID) > 0)
		tfires.m_FireID = lastFireID;//atol(sFireID);*/
		//else
			//tfires.SetFieldNull(&tfires.m_FireID);

		tfires.m_Discovery = discoveryDate;

		/* FirstAttack 
		//COleDateTime firstattackDate;  

		if (strlen(sFirstAttackDate) > 0 ){

		   // format date & time properly
           int y=0,
			m=0,
			d=0,
			hr=13,
			min=0;


           char tdate[5];
		   if (dt == Packed){ // MMDDYYYY
	         tdate[0] = sFirstAttackDate[0];
		     tdate[1] = sFirstAttackDate[1];
             tdate[2] = '\0';
		     m = atoi(tdate);
	      
		  tdate[0] = sFirstAttackDate[2];
		  tdate[1] = sFirstAttackDate[3];
          tdate[2] = '\0';
		  d = atoi(tdate);
		
		  tdate[0] = sFirstAttackDate[4];
		  tdate[1] = sFirstAttackDate[5];
	      tdate[2] = sFirstAttackDate[6];
		  tdate[3] = sFirstAttackDate[7];
          tdate[4] = '\0';
		  y = atoi(tdate);
		 


		} else if (dt == Reverse){ // YYYYMMDD

				
		  tdate[0] = sFirstAttackDate[0];
		  tdate[1] = sFirstAttackDate[1];
	      tdate[2] = sFirstAttackDate[2];
		  tdate[3] = sFirstAttackDate[3];
          tdate[4] = '\0';
		  y = atoi(tdate);

	      tdate[0] = sFirstAttackDate[4];
		  tdate[1] = sFirstAttackDate[5];
          tdate[2] = '\0';
		  m = atoi(tdate);
	      
		  tdate[0] = sFirstAttackDate[6];
		  tdate[1] = sFirstAttackDate[7];
          tdate[2] = '\0';
		  d = atoi(tdate);
	
		} 
		else 
		{  // MM/DD/YYYY
			COleDateTime discDT;
			discDT.ParseDateTime(sDiscoveryDate);
			if(discDT.GetStatus() != COleDateTime::valid)
			{
				fprintf(errLog, "\tRecord Number %d, FirstAttack date is not valid.\n", count );
				rejects ++;
				continue;
			}
			m = discDT.GetMonth();//atoi(tdate);
			d = discDT.GetDay();//atoi(tdate);
			y = discDT.GetYear();//atoi(tdate);
		}


		if (y < 1900 || y > 2100){
            fprintf(errLog, "\tRecord Number %d, FirstAttack date is not valid (bad year).\n", count );
			rejects ++;
			continue;
		}
		if (m < 1 || m > 12){
            fprintf(errLog, "\tRecord Number %d, FirstAttack date is not valid (bad month).\n", count );
			rejects ++;
			continue;
		}
		if (d < 1 || d > 31){
            fprintf(errLog, "\tRecord Number %d, FirstAttack date is not valid (bad day).\n", count );
			rejects ++;
			continue;
		}

		if (strlen(sFirstAttackTime) > 0){
           if (strlen(sFirstAttackTime) <= 2)
			   hr = atoi(sFirstAttackTime);
		   else if (strlen(sFirstAttackTime)==3 || strlen(sFirstAttackTime) > 5) {
               fprintf(errLog, "\tRecord Number %d, FirstAttackTime is not valid (bad format).\n", count );
			   rejects ++;
			   continue;
		   } else {   // normal format
			   if (tt == FullTime){  // HH:MM
                    		
		          tdate[0] = sFirstAttackTime[0];
		          tdate[1] = sFirstAttackTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sFirstAttackTime[3];
		          tdate[1] = sFirstAttackTime[4];
                  tdate[2] = '\0';
		          min = atoi(tdate);
			   } else { // HHMM
                  tdate[0] = sFirstAttackTime[0];
		          tdate[1] = sFirstAttackTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sFirstAttackTime[2];
		          tdate[1] = sFirstAttackTime[3];
                  tdate[2] = '\0';
		          min = atoi(tdate);
		       }
		   }

		}
		if (hr < 0 || hr > 23){
               fprintf(errLog, "\tRecord Number %d, FirstAttack time is not valid (bad hour).\n", count );
			   rejects ++;
			   continue;
		}
		if (min < 0 || min > 59){
		       fprintf(errLog, "\tRecord Number %d, FirstAttack time is not valid (bad minute).\n", count );
			   rejects ++;
			   continue;
		}

	
		firstattackDate.SetDateTime(y, m, d, hr, min, 0);  

		if(firstattackDate.GetStatus() != 0 || y < 1900)
		{
			fprintf(errLog, "\tRecord Number %ld, FirstAttack Date/Time is not valid: %d/%d/%04d\n", count ,  m, d, y);
			rejects ++;
			
			continue;
		}
		COleDateTime today;
		today = COleDateTime::GetCurrentTime();
		if(firstattackDate > today)
		{
			fprintf(errLog, "\tRecord Number %ld, FirstAttack Date/Time is not valid (future): %d/%d/%04d\n", count,  m, d, y);
			rejects ++;
			
			continue;
		}
		  tfires.m_FirstAttack = firstattackDate;

		} else {
          tfires.SetFieldNull(&tfires.m_FirstAttack);
		}*/

        tfires.SetFieldNull(&tfires.m_AgencyID);
		tfires.SetFieldNull(&tfires.m_RegionID);
		tfires.SetFieldNull(&tfires.m_UnitID);
		tfires.SetFieldNull(&tfires.m_SubunitID);

		//bool unitFound = false;

		/* fill in agency/region/unit if empty */
		/*if (strlen(sSubunitID) > 0){
			if (strlen(sUnitID) < 1){
              // fill in unitID based on subunitID
                CFireSubunitSet subunitSet(pDB);
				if (atoi(sSubunitID) > 0)
	               subunitSet.m_strFilter.Format("[SubunitID]=%s", sSubunitID);
				else
                   subunitSet.m_strFilter.Format("[Name]='%s'", sSubunitID);
              
				if (! subunitSet.IsEOF()){
                   sprintf(sUnitID,"%d",subunitSet.m_UnitID);
				   sprintf(sSubunitID,"%d",subunitSet.m_SubunitID);
				   unitFound = true;
				} 

				subunitSet.Close();
			}
		}

		if (strlen(sUnitID) > 0){
			if (strlen(sRegionID) < 1 || atoi(sRegionID) < 1){
              // fill in regionID based on unitID
                CFireUnitSet unitSet(pDB);
				if (atoi(sUnitID) > 0)
	                unitSet.m_strFilter.Format("[UnitID]=%s", sUnitID);
				else
                    unitSet.m_strFilter.Format("[Name]='%s'", sUnitID);
                unitSet.Open();
			
				if (! unitSet.IsEOF()){
                   sprintf(sRegionID,"%d",unitSet.m_RegionID);
				   sprintf(sUnitID, "%d", unitSet.m_UnitID);
				   
				   lastRegionID = unitSet.m_RegionID;

				   unitFound = true;
				}
				unitSet.Close();
			}
		}*/

		/*if (strlen(sRegionID) > 0){
			if (strlen(sAgencyID) < 1 || atoi(sAgencyID) < 1){
              // fill in agencyID based on regionID
                CFireRegionSet regSet(pDB);
				if (atoi(sRegionID) > 0)
	               regSet.m_strFilter.Format("[RegionID]=%s", sRegionID);
				else
                   regSet.m_strFilter.Format("[Name]='%s'", sRegionID);
                regSet.Open();
				
				if (! regSet.IsEOF()){
                   sprintf(sAgencyID,"%d",regSet.m_AgencyID);
				   sprintf(sRegionID,"%d",regSet.m_RegionID);

				   lastRegionID = regSet.m_RegionID;
				}
				regSet.Close();
			}
		}*/


        /* haven't identified units yet?  check DOI codes */
		/*if ((strlen(sSubunitID) > 0 ||
			strlen(sUnitID) > 0) &&
			//(strlen(sAgencyID) < 1 ||
			//strlen(sRegionID) < 1)){

               CDOIUnitSet doiSet(pDB);
			   doiSet.m_strFilter.Format("[WFMICode]='%s'",sSubunitID);
	           doiSet.Open();
			   if (doiSet.IsEOF()){
				   doiSet.m_strFilter.Format("[WFMICode]='%s'",sUnitID);
				   doiSet.Requery();
 			   }
			   if (! doiSet.IsEOF()){ // WFMI code located - convert to "FFP" code
                    sprintf(sSubunitID,"%d", doiSet.m_SubunitID);
					sprintf(sUnitID,"%d", doiSet.m_UnitID);
					sprintf(sRegionID,"%d", doiSet.m_RegionID);
					sprintf(sAgencyID,"%d", doiSet.m_AgencyID);
					unitFound = true;

					lastRegionID = doiSet.m_RegionID;
			   }
               doiSet.Close();
		}*/

        // imported a unit that wasn't found? add it
	   // if (strlen(sUnitID) > 0 && !unitFound){
		//	fprintf(errLog, "\tRecord Number %ld, Unit: %s has no match in FireUnits, FireSubunits, or refDOIUnits, REJECTED \n", count, sUnitID);
		//	rejects++;
		//	continue;
			// add as a new unit, with name = sUnitID
					
            /*CFireUnitSet unitSet(pDB);
			unitSet.m_strSort.Format("[UnitID]");
			unitSet.Open();
			unitSet.MoveLast();
			
			int nextID = 1;
			if (! unitSet.IsEOF())
				 nextID = unitSet.m_UnitID + 1;
			unitSet.m_strFilter.Format("[UnitID]= -%d",nextID);
            unitSet.Requery();
			while (! unitSet.IsEOF()){
                 nextID ++;
                 unitSet.m_strFilter.Format("[UnitID]= -%d",nextID);
                 unitSet.Requery();
			}
			unitSet.Close();
			unitSet.Open();

			unitSet.AddNew();
            unitSet.m_Name.Format("%s",sUnitID);
			unitSet.m_UnitID = (0 - nextID);   // add as negative number for now (to flag as new) - will be changed later


			unitSet.m_RegionID = lastRegionID;*/

			/*CFireRegionSet regSet(pDB);
			
			regSet.m_strFilter.Format("[RegionID] = %d",lastRegionID);
            regSet.Open();
			if (! regSet.IsEOF()){
				sprintf(sAgencyID,"%d",regSet.m_AgencyID);
			}
			regSet.Close();*/

			/*unitSet.Update();

			sprintf(sUnitID,"%d",nextID);
		    sprintf(sRegionID,"%d", unitSet.m_RegionID);
			
			
			
            unitSet.Close();*/

			

		//}
		
		if (strlen(sFireNumber) > 0)
		{
			sFireNumber[12] = 0;//safety truncate!
			tfires.m_FireNumber = sFireNumber;
			tfires.m_FireNumber.Trim();
		} 
		else
		{//error, Fire number is required
			fprintf(errLog, "\tRecord Number %d, Missing required field: Fire Number.\n", count );
				rejects ++;
				continue;
		}

		/*if (strlen(sState) > 0)
		{
			sState[2] = 0;//safety truncate!
			tfires.m_State = sState;
			tfires.m_State.Trim();
		} 
		else
            tfires.SetFieldNull(&tfires.m_State);

		if (strlen(sCounty) > 0)
			tfires.m_County = atol(sCounty);
		else
            tfires.SetFieldNull(&tfires.m_County);
		*/
		if (strlen(sTotalAcres) > 0)
			tfires.m_TotalAcres = atof(sTotalAcres);
		else
		{//error, Total Acres is required
			fprintf(errLog, "\tRecord Number %d, Missing required field: Total Acres.\n", count );
				rejects ++;
				continue;
		}
           // tfires.SetFieldNull(&tfires.m_TotalAcres);

		if (strlen(sFireName) > 0)
		{
			sFireName[25] = 0;//safety truncate!

			// if we've replaced commas with semicolons, change them back
			for (int i = 0; i < strlen(sFireName);i++)
				if (sFireName[i]==';')
					sFireName[i] = ',';


			tfires.m_FireName = sFireName;
			tfires.m_FireName.Trim();
			

		} else
            tfires.SetFieldNull(&tfires.m_FireName);

		if (strlen(sStatisticalCause) > 0)
		{   
			long testVal = atol(sStatisticalCause);
			if (testVal < 1 || testVal > 9)
			{
                fprintf(errLog, "\tRecord Number %d, Invalid field: Statistical Cause '%s'.\n", count,sStatisticalCause );
				rejects ++;
				continue;

			} else
			tfires.m_StatisticalCause = testVal;
		} else
		{
			//error, cause is required
			fprintf(errLog, "\tRecord Number %d, Missing required field: Statistical Cause.\n", count );
				rejects ++;
				continue;
		}
           
		/*if (strlen(sGeneralCause) > 0)
			tfires.m_GeneralCause = atol(sGeneralCause);
		else
            tfires.SetFieldNull(&tfires.m_GeneralCause);

		if (strlen(sSpecificCause) > 0)
			tfires.m_SpecificCause = atol(sSpecificCause);
		else
            tfires.SetFieldNull(&tfires.m_SpecificCause);

		if (strlen(sClassPeople) > 0)
			tfires.m_ClassPeople = atol(sClassPeople);
		else
            tfires.SetFieldNull(&tfires.m_ClassPeople);

		if (strlen(sCauseNarr) > 0)
		{
			sCauseNarr[50] = 0;//safety truncate!
			tfires.m_CauseNarr = sCauseNarr;
			tfires.m_CauseNarr.Trim();
		} 
		else
            tfires.SetFieldNull(&tfires.m_CauseNarr);

		if (strlen(sTownship) > 0)
		{
			sTownship[5] = 0;//safety truncate!
			tfires.m_Township = sTownship;
			tfires.m_Township.Trim();
		} else
            tfires.SetFieldNull(&tfires.m_Township);

		if (strlen(sRange) > 0)
		{
			sRange[5] = 0;//safety truncate!
			tfires.m_Range = sRange;
			tfires.m_Range.Trim();
		} else
            tfires.SetFieldNull(&tfires.m_Range);

		if (strlen(sSection) > 0)
			tfires.m_Section = atol(sSection);
		else
            tfires.SetFieldNull(&tfires.m_Section);

		if (strlen(sSubSection) > 0)
		{
			sSubSection[4] = 0;//safety truncate!
			tfires.m_SubSection = sSubSection;
			tfires.m_SubSection.Trim();
		} 
		else
            tfires.SetFieldNull(&tfires.m_SubSection);
		*/
		if (strlen(sLatitude) > 0)
		{
			tfires.m_latitude = atof(sLatitude);
			/*int dd, mm, ss;
			DecimalDegree2DDMMSS(atof(sLatitude), &dd, &mm, &ss);
			tfires.m_LatDD = dd;
			tfires.m_LatMM = mm;
			tfires.m_LatSS = ss;*/
		   /*tfires.m_LatDD = atoi(sLatitude);
		   tfires.m_LatMM = (int) ((atof(sLatitude) + .000001 - (float) (tfires.m_LatDD)) * 60.0);
           tfires.m_LatSS = (int) ((atof(sLatitude) + .000001 - ((float) (tfires.m_LatDD) + 0.0000001) - (float) (tfires.m_LatMM) / 60.0) * 60.0);

           if (tfires.m_LatMM < 0)
		    	tfires.m_LatMM = 0 - tfires.m_LatMM;
		   if (tfires.m_LatSS < 0)
			   tfires.m_LatSS = 0 - tfires.m_LatSS;*/
		} 
		else 
		{
			   tfires.SetFieldNull(&tfires.m_latitude);
			   //tfires.SetFieldNull(&tfires.m_LatMM);
			   //tfires.SetFieldNull(&tfires.m_LatSS);
		}
		



		if (strlen(sLongitude) > 0)
		{
			tfires.m_longitude = atof(sLongitude);
			/*int dd, mm, ss;
			DecimalDegree2DDMMSS(atof(sLongitude), &dd, &mm, &ss);
			tfires.m_LonDD = dd;
			tfires.m_LonMM = mm;
			tfires.m_LonSS = ss;*/
			/*tfires.m_LonDD = atoi(sLongitude);
		    tfires.m_LonMM = (int) ((atof(sLongitude) + .000001 - (float) (tfires.m_LonDD)) * 60.0);
            tfires.m_LonSS = (int) ((atof(sLongitude) + .000001 - ((float) (tfires.m_LonDD) + 0.000001) - (float) (tfires.m_LonMM) / 60.0) * 60.0);

		    if (tfires.m_LonMM < 0)
			    tfires.m_LonMM = 0 - tfires.m_LonMM;
		    if (tfires.m_LonSS < 0)
			    tfires.m_LonSS = 0 - tfires.m_LonSS;*/
		} 
		else 
		{
			   tfires.SetFieldNull(&tfires.m_longitude);
			   //tfires.SetFieldNull(&tfires.m_LonMM);
			   //tfires.SetFieldNull(&tfires.m_LonSS);
		}
		

		/*if (strlen(sSlope) > 0)
		{
			sSlope[1] = 0;//safety truncate!
			tfires.m_Slope = sSlope;
			tfires.m_Slope.Trim();
		} else
            tfires.SetFieldNull(&tfires.m_Slope);

		if (strlen(sElevation) > 0)
		{
			sElevation[1] = 0;////safety truncate! Ask Larry about this hoooey
			tfires.m_Elevation = sElevation;
			tfires.m_Elevation.Trim();
		} else
            tfires.SetFieldNull(&tfires.m_Elevation);

		if (strlen(sAspect) > 0)
		{
			sAspect[1] = 0;//safety truncate! Ask Larry about this hoooey
			tfires.m_Aspect = sAspect;
			tfires.m_Aspect.Trim();
		} else
            tfires.SetFieldNull(&tfires.m_Aspect);

		if (strlen(sFuelModel) > 0)
		{
			sFuelModel[1] = 0;//safety truncate! Ask Larry about this hoooey
			tfires.m_FuelModel = sFuelModel;
			tfires.m_FuelModel.Trim();
		} else
            tfires.SetFieldNull(&tfires.m_FuelModel);

		if (strlen(sFireType) > 0)
		{
			sFireType[1] = 0;//safety truncate! Ask Larry about this hoooey
			tfires.m_FireType = sFireType;
			tfires.m_FireType.Trim();
		} else
            tfires.SetFieldNull(&tfires.m_FireType);*/

	    /* contain */

		COleDateTime containDate;  

		if (strlen(sContainDate) > 0 ){

		   // format date & time properly
           int y=0,
			m=0,
			d=0,
			hr=13,
			min=0;


           char tdate[5];
		   if (dt == Packed){ // MMDDYYYY
	         tdate[0] = sContainDate[0];
		     tdate[1] = sContainDate[1];
             tdate[2] = '\0';
		     m = atoi(tdate);
	      
		  tdate[0] = sContainDate[2];
		  tdate[1] = sContainDate[3];
          tdate[2] = '\0';
		  d = atoi(tdate);
		
		  tdate[0] = sContainDate[4];
		  tdate[1] = sContainDate[5];
	      tdate[2] = sContainDate[6];
		  tdate[3] = sContainDate[7];
          tdate[4] = '\0';
		  y = atoi(tdate);
		 


		} else if (dt == Reverse){ // YYYYMMDD

				
		  tdate[0] = sContainDate[0];
		  tdate[1] = sContainDate[1];
	      tdate[2] = sContainDate[2];
		  tdate[3] = sContainDate[3];
          tdate[4] = '\0';
		  y = atoi(tdate);

	      tdate[0] = sContainDate[4];
		  tdate[1] = sContainDate[5];
          tdate[2] = '\0';
		  m = atoi(tdate);
	      
		  tdate[0] = sContainDate[6];
		  tdate[1] = sContainDate[7];
          tdate[2] = '\0';
		  d = atoi(tdate);
	
		} 
		else 
		{  // MM/DD/YYYY
			COleDateTime discDT;
			discDT.ParseDateTime(sDiscoveryDate);
			if(discDT.GetStatus() != COleDateTime::valid)
			{
				fprintf(errLog, "\tRecord Number %d, Contain date is not valid.\n", count );
				rejects ++;
				continue;
			}
			m = discDT.GetMonth();//atoi(tdate);
			d = discDT.GetDay();//atoi(tdate);
			y = discDT.GetYear();//atoi(tdate);
		}


		if (y < 1900 || y > 2100){
            fprintf(errLog, "\tRecord Number %d, Contain date is not valid (bad year).\n", count );
			rejects ++;
			continue;
		}
		if (m < 1 || m > 12){
            fprintf(errLog, "\tRecord Number %d, Contain date is not valid (bad month).\n", count );
			rejects ++;
			continue;
		}
		if (d < 1 || d > 31){
            fprintf(errLog, "\tRecord Number %d, Contain date is not valid (bad day).\n", count );
			rejects ++;
			continue;
		}

		if (strlen(sContainTime) > 0){
           if (strlen(sContainTime) <= 2)
			   hr = atoi(sContainTime);
		   else if (strlen(sContainTime)==3 || strlen(sContainTime) > 5) {
               fprintf(errLog, "\tRecord Number %d, ContainTime is not valid (bad format).\n", count );
			   rejects ++;
			   continue;
		   } else {   // normal format
			   if (tt == FullTime){  // HH:MM
                    		
		          tdate[0] = sContainTime[0];
		          tdate[1] = sContainTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sContainTime[3];
		          tdate[1] = sContainTime[4];
                  tdate[2] = '\0';
		          min = atoi(tdate);
			   } else { // HHMM
                  tdate[0] = sContainTime[0];
		          tdate[1] = sContainTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sContainTime[2];
		          tdate[1] = sContainTime[3];
                  tdate[2] = '\0';
		          min = atoi(tdate);
		       }
		   }

		}
		if (hr < 0 || hr > 23){
               fprintf(errLog, "\tRecord Number %d, Contain time is not valid (bad hour).\n", count );
			   rejects ++;
			   continue;
		}
		if (min < 0 || min > 59){
		       fprintf(errLog, "\tRecord Number %d, Contain time is not valid (bad minute).\n", count );
			   rejects ++;
			   continue;
		}

	
		containDate.SetDateTime(y, m, d, hr,min, 0);

		if(containDate.GetStatus() != 0 || y < 1900)
		{
			fprintf(errLog, "\tRecord Number %ld, Contain Date/Time is not valid: %d/%d/%04d\n", count ,  m, d, y);
			rejects ++;
			
			continue;
		}
		COleDateTime today;
		today = COleDateTime::GetCurrentTime();
		if(containDate > today)
		{
			fprintf(errLog, "\tRecord Number %ld, Contain Date/Time is not valid (future): %d/%d/%04d\n", count,  m, d, y);
			rejects ++;
			
			continue;
		}
		  tfires.m_Contain = containDate;

		} else {
          tfires.SetFieldNull(&tfires.m_Contain);
		}

		/* end contain */
        /* strategy met */

		COleDateTime StrategyMetDate;  

		if (strlen(sStrategyMetDate) > 0 ){

		   // format date & time properly
           int y=0,
			m=0,
			d=0,
			hr=13,
			min=0;


           char tdate[5];
		   if (dt == Packed){ // MMDDYYYY
	         tdate[0] = sStrategyMetDate[0];
		     tdate[1] = sStrategyMetDate[1];
             tdate[2] = '\0';
		     m = atoi(tdate);
	      
		  tdate[0] = sStrategyMetDate[2];
		  tdate[1] = sStrategyMetDate[3];
          tdate[2] = '\0';
		  d = atoi(tdate);
		
		  tdate[0] = sStrategyMetDate[4];
		  tdate[1] = sStrategyMetDate[5];
	      tdate[2] = sStrategyMetDate[6];
		  tdate[3] = sStrategyMetDate[7];
          tdate[4] = '\0';
		  y = atoi(tdate);
		 


		} else if (dt == Reverse){ // YYYYMMDD

				
		  tdate[0] = sStrategyMetDate[0];
		  tdate[1] = sStrategyMetDate[1];
	      tdate[2] = sStrategyMetDate[2];
		  tdate[3] = sStrategyMetDate[3];
          tdate[4] = '\0';
		  y = atoi(tdate);

	      tdate[0] = sStrategyMetDate[4];
		  tdate[1] = sStrategyMetDate[5];
          tdate[2] = '\0';
		  m = atoi(tdate);
	      
		  tdate[0] = sStrategyMetDate[6];
		  tdate[1] = sStrategyMetDate[7];
          tdate[2] = '\0';
		  d = atoi(tdate);
	
		} 
		else 
		{  // MM/DD/YYYY
			COleDateTime discDT;
			discDT.ParseDateTime(sDiscoveryDate);
			if(discDT.GetStatus() != COleDateTime::valid)
			{
				fprintf(errLog, "\tRecord Number %d, StrategyMet date is not valid.\n", count );
				rejects ++;
				continue;
			}
			m = discDT.GetMonth();//atoi(tdate);
			d = discDT.GetDay();//atoi(tdate);
			y = discDT.GetYear();//atoi(tdate);
		}


		if (y < 1900 || y > 2100){
            fprintf(errLog, "\tRecord Number %d, StrategyMet date is not valid (bad year).\n", count );
			rejects ++;
			continue;
		}
		if (m < 1 || m > 12){
            fprintf(errLog, "\tRecord Number %d, StrategyMet date is not valid (bad month).\n", count );
			rejects ++;
			continue;
		}
		if (d < 1 || d > 31){
            fprintf(errLog, "\tRecord Number %d, StrategyMet date is not valid (bad day).\n", count );
			rejects ++;
			continue;
		}

		if (strlen(sStrategyMetTime) > 0){
           if (strlen(sStrategyMetTime) <= 2)
			   hr = atoi(sStrategyMetTime);
		   else if (strlen(sStrategyMetTime)==3 || strlen(sStrategyMetTime) > 5) {
               fprintf(errLog, "\tRecord Number %d, StrategyMetTime is not valid (bad format).\n", count );
			   rejects ++;
			   continue;
		   } else {   // normal format
			   if (tt == FullTime){  // HH:MM
                    		
		          tdate[0] = sStrategyMetTime[0];
		          tdate[1] = sStrategyMetTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sStrategyMetTime[3];
		          tdate[1] = sStrategyMetTime[4];
                  tdate[2] = '\0';
		          min = atoi(tdate);
			   } else { // HHMM
                  tdate[0] = sStrategyMetTime[0];
		          tdate[1] = sStrategyMetTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sStrategyMetTime[2];
		          tdate[1] = sStrategyMetTime[3];
                  tdate[2] = '\0';
		          min = atoi(tdate);
		       }
		   }

		}
		if (hr < 0 || hr > 23){
               fprintf(errLog, "\tRecord Number %d, StrategyMet time is not valid (bad hour).\n", count );
			   rejects ++;
			   continue;
		}
		if (min < 0 || min > 59){
		       fprintf(errLog, "\tRecord Number %d, StrategyMet time is not valid (bad minute).\n", count );
			   rejects ++;
			   continue;
		}

	
		StrategyMetDate.SetDateTime(y, m, d, hr, min, 0);  

		if(StrategyMetDate.GetStatus() != 0 || y < 1900)
		{
			fprintf(errLog, "\tRecord Number %ld, StrategyMet Date/Time is not valid: %d/%d/%04d\n", count ,  m, d, y);
			rejects ++;
			
			continue;
		}
		COleDateTime today;
		today = COleDateTime::GetCurrentTime();
		if(StrategyMetDate > today)
		{
			fprintf(errLog, "\tRecord Number %ld, StrategyMet Date/Time is not valid (future): %d/%d/%04d\n", count,  m, d, y);
			rejects ++;
			
			continue;
		}
		tfires.m_StrategyMet = StrategyMetDate;

		} else {
          tfires.SetFieldNull(&tfires.m_StrategyMet);
		}

		/* end strategy met */
        /* fire out 

		COleDateTime FireOutDate;  

		if (strlen(sFireOutDate) > 0 ){

		   // format date & time properly
           int y=0,
			m=0,
			d=0,
			hr=13,
			min=0;


           char tdate[5];
		   if (dt == Packed){ // MMDDYYYY
	         tdate[0] = sFireOutDate[0];
		     tdate[1] = sFireOutDate[1];
             tdate[2] = '\0';
		     m = atoi(tdate);
	      
		  tdate[0] = sFireOutDate[2];
		  tdate[1] = sFireOutDate[3];
          tdate[2] = '\0';
		  d = atoi(tdate);
		
		  tdate[0] = sFireOutDate[4];
		  tdate[1] = sFireOutDate[5];
	      tdate[2] = sFireOutDate[6];
		  tdate[3] = sFireOutDate[7];
          tdate[4] = '\0';
		  y = atoi(tdate);
		 


		} else if (dt == Reverse){ // YYYYMMDD

				
		  tdate[0] = sFireOutDate[0];
		  tdate[1] = sFireOutDate[1];
	      tdate[2] = sFireOutDate[2];
		  tdate[3] = sFireOutDate[3];
          tdate[4] = '\0';
		  y = atoi(tdate);

	      tdate[0] = sFireOutDate[4];
		  tdate[1] = sFireOutDate[5];
          tdate[2] = '\0';
		  m = atoi(tdate);
	      
		  tdate[0] = sFireOutDate[6];
		  tdate[1] = sFireOutDate[7];
          tdate[2] = '\0';
		  d = atoi(tdate);
	
		} 
		else 
		{  // MM/DD/YYYY
			COleDateTime discDT;
			discDT.ParseDateTime(sDiscoveryDate);
			if(discDT.GetStatus() != COleDateTime::valid)
			{
				fprintf(errLog, "\tRecord Number %d, FireOut date is not valid.\n", count );
				rejects ++;
				continue;
			}
			m = discDT.GetMonth();//atoi(tdate);
			d = discDT.GetDay();//atoi(tdate);
			y = discDT.GetYear();//atoi(tdate);
		}


		if (y < 1900 || y > 2100){
            fprintf(errLog, "\tRecord Number %d, FireOut date is not valid (bad year).\n", count );
			rejects ++;
			continue;
		}
		if (m < 1 || m > 12){
            fprintf(errLog, "\tRecord Number %d, FireOut date is not valid (bad month).\n", count );
			rejects ++;
			continue;
		}
		if (d < 1 || d > 31){
            fprintf(errLog, "\tRecord Number %d, FireOut date is not valid (bad day).\n", count );
			rejects ++;
			continue;
		}

		if (strlen(sFireOutTime) > 0){
           if (strlen(sFireOutTime) <= 2)
			   hr = atoi(sFireOutTime);
		   else if (strlen(sFireOutTime)==3 || strlen(sFireOutTime) > 5) {
               fprintf(errLog, "\tRecord Number %d, FireOutTime is not valid (bad format).\n", count );
			   rejects ++;
			   continue;
		   } else {   // normal format
			   if (tt == FullTime){  // HH:MM
                    		
		          tdate[0] = sFireOutTime[0];
		          tdate[1] = sFireOutTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sFireOutTime[3];
		          tdate[1] = sFireOutTime[4];
                  tdate[2] = '\0';
		          min = atoi(tdate);
			   } else { // HHMM
                  tdate[0] = sFireOutTime[0];
		          tdate[1] = sFireOutTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sFireOutTime[2];
		          tdate[1] = sFireOutTime[3];
                  tdate[2] = '\0';
		          min = atoi(tdate);
		       }
		   }

		}
		if (hr < 0 || hr > 23){
               fprintf(errLog, "\tRecord Number %d, FireOut time is not valid (bad hour).\n", count );
			   rejects ++;
			   continue;
		}
		if (min < 0 || min > 59){
		       fprintf(errLog, "\tRecord Number %d, FireOut time is not valid (bad minute).\n", count );
			   rejects ++;
			   continue;
		}

	
		FireOutDate.SetDateTime(y, m, d, hr, min, 0); 

		if(FireOutDate.GetStatus() != 0 || y < 1900)
		{
			fprintf(errLog, "\tRecord Number %ld, FireOut Date/Time is not valid: %d/%d/%04d\n", count ,  m, d, y);
			rejects ++;
			
			continue;
		}
		COleDateTime today;
		today = COleDateTime::GetCurrentTime();
		if(FireOutDate > today)
		{
			fprintf(errLog, "\tRecord Number %ld, FireOut Date/Time is not valid (future): %d/%d/%04d\n", count,  m, d, y);
			rejects ++;
			
			continue;
		}
		   tfires.m_FireOut = FireOutDate;

		} else {
          tfires.SetFieldNull(&tfires.m_FireOut);
		}*/


		//Region, Unit, Subunit check
		// 2014: moved this to the end of processing, so we don't add new units/subunits for rejected records
		//

		long regID = -1;
		if(strlen(sRegionID) > 0)
		{
			regID = GetFireRegion(&regSet, agencyID, sRegionID);
		}
		if(regID == -1 && m_useRegion)
		{
			//int rloc = m_comboRegions.GetCurSel();
			//if(rloc != CB_ERR)
			regID = defaultRegion;//m_comboRegions.GetItemData(rloc);
		}
		if(regID == -1)
		{
			//unknown region (required field, reject)
			//if(strlen(sRegionID) > 0)
				fprintf(errLog, "\tRecord Number %ld, Bad or Unknown region: %s\n", count, sRegionID);
			rejects ++;			
			continue;
		}
		long uID = -1;
		if(strlen(sUnitID) > 0)
		{
			uID = GetFireUnit(&unitSet, agencyID, regID, sUnitID);
			if(uID <= 0)
			{
				uID = AddFireUnit(&unitSet, agencyID, regID, sUnitID);
				fprintf(errLog, "\tRecord Number %ld, Added New Unit: '%s Unknown'\n", count, sUnitID);
			}
		}
		if(uID == -1 && m_useUnit)
		{
			int rloc = m_comboUnits.GetCurSel();
			if(rloc != CB_ERR)
				uID = m_comboUnits.GetItemData(rloc);
		}
		long suID = -1;
		if(strlen(sSubunitID) > 0)
		{
			suID = GetFireSubunit(&subSet, agencyID, uID, sSubunitID);
			if(suID <= 0)
			{
				suID = AddFireSubUnit(&subSet, uID, sSubunitID);
				fprintf(errLog, "\tRecord Number %ld, Added New Unit Subunit: '%s Unknown'\n", count, sSubunitID);
			}
		}
		if(suID == -1 && m_useSubunit)
		{
			int rloc = m_comboSubunits.GetCurSel();
			if(rloc != CB_ERR)
				suID = m_comboSubunits.GetItemData(rloc);
		}


        //if (strlen(sAgencyID) > 0)
		tfires.m_AgencyID = agencyID;//atol(sAgencyID);
		//else
          //  tfires.SetFieldNull(&tfires.m_AgencyID);

		//if (strlen(sRegionID) > 0)
		tfires.m_RegionID = regID;//atol(sRegionID);
		//else if(defaultRegion > 0)
            //tfires.m_RegionID = defaultRegion;
		//else
		//{//error, region is required
		//	fprintf(errLog, "\tRecord Number %d, Missing required field: Region.\n", count );
		//		rejects ++;
		//		continue;
		//}
		/*if (strlen(sUnitID) > 0)
			tfires.m_UnitID = atol(sUnitID);
		else if(defaultUnit > 0)
            tfires.m_UnitID = defaultUnit;
		else
            tfires.SetFieldNull(&tfires.m_UnitID);

        if (strlen(sSubunitID) > 0)
			tfires.m_SubunitID = atol(sSubunitID);
		else if(defaultSubunit > 0)
			tfires.m_SubunitID = defaultSubunit;
		else
            tfires.SetFieldNull(&tfires.m_SubunitID);*/
		if(uID > 0)
		{
			tfires.m_UnitID = uID;
		}
		if(suID > 0)
		{
			tfires.m_SubunitID = suID;
		}

		/* end fire out */
		//tfires.m_FireID = lastFireID;
		tfires.Update();	
    
	}
	
		
	

	stream.close();
	
	 long appends = 0, edits = 0;

	 
	//if (!cd ||  !cd->Abort())
	//{
    	COleDateTime lastDay;
	   //CTime lastDay;  long edits = 0, appends = 0;
	  /* if (cd)
	   {
	      cd->Reset();
	      cd->SetStatusString("Updating Database");	
	   }*/
	   if(tfires.IsBOF() && tfires.IsEOF())
	   {//error, no valid fires
			fprintf(errLog, "\tNo valid fires to import!\n");

	   }
	   else
	   {
			tfires.MoveFirst();
		   
			
			while(!tfires.IsEOF() )//&& (!cd || !cd->Abort()))
			{
				//if (cd)
				//   cd->Increment();

				if(tfires.IsFieldNull(&tfires.m_UnitID))//no unitID, check for dups at region level
					fires.m_strFilter.Format("[AgencyID]=%ld AND [RegionID]=%ld AND [FireNumber]='%s'", 
						tfires.m_AgencyID, tfires.m_RegionID, tfires.m_FireNumber);
				else//check for dups at unit level
					fires.m_strFilter.Format("[AgencyID]=%ld AND [RegionID]=%ld AND [UnitID]=%ld AND [FireNumber]='%s'", 
						tfires.m_AgencyID, tfires.m_RegionID, tfires.m_UnitID, tfires.m_FireNumber);

				fires.Requery();


				int newRec = false;

				if(!fires.IsEOF())
				{
					if(!m_RejectDups)
					{
						fires.Edit();
						edits++;
					}
					else
					{
						fprintf(errLog, "\tDuplicate record rejected  Date: %d/%d/%d FireNumber: %s\n",
							tfires.m_Discovery.GetMonth(), tfires.m_Discovery.GetDay(), tfires.m_Discovery.GetYear(),
							tfires.m_FireNumber);
						rejects++;
						dupRejects++;
						tfires.MoveNext();
						continue;
					}
				} 
				else 
				{
					fires.AddNew();
					appends++;
					newRec = true;
				}

				//if (newRec)
					//if (! tfires.IsFieldNull(&tfires.m_FireID))				
					//	fires.m_FireID = tfires.m_FireID;
				
				if (! tfires.IsFieldNull(&tfires.m_Discovery))
					fires.m_Discovery = tfires.m_Discovery;

				/*if (!tfires.IsFieldNull(&tfires.m_FirstAttack))
					fires.m_FirstAttack = tfires.m_FirstAttack;
				if(newRec && tfires.IsFieldNull(&tfires.m_FirstAttack))
					fires.SetFieldNull(&fires.m_FirstAttack);*/

				if (! tfires.IsFieldNull(&tfires.m_AgencyID))
					fires.m_AgencyID = tfires.m_AgencyID;
				if(newRec && tfires.IsFieldNull(&tfires.m_AgencyID))
					fires.SetFieldNull(&fires.m_AgencyID);

				if (! tfires.IsFieldNull(&tfires.m_RegionID))
					fires.m_RegionID = tfires.m_RegionID;
				if(newRec && tfires.IsFieldNull(&tfires.m_RegionID))
					fires.SetFieldNull(&fires.m_RegionID);

				if (! tfires.IsFieldNull(&tfires.m_UnitID))
	   				fires.m_UnitID = tfires.m_UnitID;
				if(newRec && tfires.IsFieldNull(&tfires.m_UnitID))
					fires.SetFieldNull(&fires.m_UnitID);

				/*if (!tfires.IsFieldNull(&tfires.m_Year))
	   				fires.m_Year = tfires.m_Year;
				if(newRec && tfires.IsFieldNull(&tfires.m_Year))
					fires.SetFieldNull(&fires.m_Year);*/

				if (! tfires.IsFieldNull(&tfires.m_SubunitID))
					fires.m_SubunitID = tfires.m_SubunitID;
				if(newRec && tfires.IsFieldNull(&tfires.m_SubunitID))
					fires.SetFieldNull(&fires.m_SubunitID);

			
				if (! tfires.IsFieldNull(&tfires.m_FireNumber))
					fires.m_FireNumber = tfires.m_FireNumber;
				if(newRec && tfires.IsFieldNull(&tfires.m_FireNumber))
					fires.SetFieldNull(&fires.m_FireNumber);
			
				/*if (!tfires.IsFieldNull(&tfires.m_State))
					fires.m_State = tfires.m_State;
				if(newRec && tfires.IsFieldNull(&tfires.m_State))
					fires.SetFieldNull(&fires.m_State);

				if (! tfires.IsFieldNull(&tfires.m_County))
   					fires.m_County = tfires.m_County;
				if(newRec && tfires.IsFieldNull(&tfires.m_County))
					fires.SetFieldNull(&fires.m_County);*/

				if (! tfires.IsFieldNull(&tfires.m_TotalAcres))
					fires.m_TotalAcres = tfires.m_TotalAcres;
				if(newRec && tfires.IsFieldNull(&tfires.m_TotalAcres))
					fires.SetFieldNull(&fires.m_TotalAcres);

				if (! tfires.IsFieldNull(&tfires.m_FireName))
					fires.m_FireName = tfires.m_FireName;
				if(newRec && tfires.IsFieldNull(&tfires.m_FireName))
					fires.SetFieldNull(&fires.m_FireName);
					
				if (! tfires.IsFieldNull(&tfires.m_StatisticalCause))
					fires.m_StatisticalCause = tfires.m_StatisticalCause;
				if(newRec && tfires.IsFieldNull(&tfires.m_StatisticalCause))
					fires.SetFieldNull(&fires.m_StatisticalCause);

				/*if (!tfires.IsFieldNull(&tfires.m_GeneralCause))
					fires.m_GeneralCause = tfires.m_GeneralCause;
				if(newRec && tfires.IsFieldNull(&tfires.m_GeneralCause))
					fires.SetFieldNull(&fires.m_GeneralCause);

				if (! tfires.IsFieldNull(&tfires.m_SpecificCause))
 					fires.m_SpecificCause = tfires.m_SpecificCause;
				if(newRec && tfires.IsFieldNull(&tfires.m_SpecificCause))
					fires.SetFieldNull(&fires.m_SpecificCause);

				if (! tfires.IsFieldNull(&tfires.m_ClassPeople))
					fires.m_ClassPeople = tfires.m_ClassPeople;
				if(newRec && tfires.IsFieldNull(&tfires.m_ClassPeople))
					fires.SetFieldNull(&fires.m_ClassPeople);

				if (! tfires.IsFieldNull(&tfires.m_CauseNarr))
					fires.m_CauseNarr = tfires.m_CauseNarr;
				if(newRec && tfires.IsFieldNull(&tfires.m_CauseNarr))
					fires.SetFieldNull(&fires.m_CauseNarr);

				if (! tfires.IsFieldNull(&tfires.m_Township))
					fires.m_Township = tfires.m_Township;
				if(newRec && tfires.IsFieldNull(&tfires.m_Township))
					fires.SetFieldNull(&fires.m_Township);

				if (! tfires.IsFieldNull(&tfires.m_Range))
					fires.m_Range = tfires.m_Range;
				if(newRec && tfires.IsFieldNull(&tfires.m_Range))
					fires.SetFieldNull(&fires.m_Range);

				if (! tfires.IsFieldNull(&tfires.m_Section))
					fires.m_Section = tfires.m_Section;
				if(newRec && tfires.IsFieldNull(&tfires.m_Section))
					fires.SetFieldNull(&fires.m_Section);

				if (! tfires.IsFieldNull(&tfires.m_SubSection))
					fires.m_SubSection = tfires.m_SubSection;
				if(newRec && tfires.IsFieldNull(&tfires.m_SubSection))
					fires.SetFieldNull(&fires.m_SubSection);*/

				if (!tfires.IsFieldNull(&tfires.m_latitude))
					fires.m_latitude = tfires.m_latitude;
				else
					fires.SetFieldNull(&fires.m_latitude);
				/*if (newRec && tfires.IsFieldNull(&tfires.m_LatDD))
					fires.SetFieldNull(&fires.m_LatDD);

				if (! tfires.IsFieldNull(&tfires.m_LatMM))
					fires.m_LatMM = tfires.m_LatMM;
				if(newRec && tfires.IsFieldNull(&tfires.m_LatMM))
					fires.SetFieldNull(&fires.m_LatMM);

				if (! tfires.IsFieldNull(&tfires.m_LatSS))
					fires.m_LatSS = tfires.m_LatSS;
				if(newRec && tfires.IsFieldNull(&tfires.m_LatSS))
					fires.SetFieldNull(&fires.m_LatSS);*/

				if (! tfires.IsFieldNull(&tfires.m_longitude))
					fires.m_longitude = tfires.m_longitude;
				else
					fires.SetFieldNull(&fires.m_longitude);
				/*if (newRec && tfires.IsFieldNull(&tfires.m_LonDD))
					fires.SetFieldNull(&fires.m_LonDD);

				if (! tfires.IsFieldNull(&tfires.m_LonMM))
					fires.m_LonMM = tfires.m_LonMM;
				if(newRec && tfires.IsFieldNull(&tfires.m_LonMM))
					fires.SetFieldNull(&fires.m_LonMM);

				if (! tfires.IsFieldNull(&tfires.m_LonSS))
					fires.m_LonSS = tfires.m_LonSS;
				if(newRec && tfires.IsFieldNull(&tfires.m_LonSS))
					fires.SetFieldNull(&fires.m_LonSS);

				if (! tfires.IsFieldNull(&tfires.m_Slope))
					fires.m_Slope = tfires.m_Slope;
				if(newRec && tfires.IsFieldNull(&tfires.m_Slope))
					fires.SetFieldNull(&fires.m_Slope);

				if (! tfires.IsFieldNull(&tfires.m_Elevation))
					fires.m_Elevation = tfires.m_Elevation;
				if(newRec && tfires.IsFieldNull(&tfires.m_Elevation))
					fires.SetFieldNull(&fires.m_Elevation);

				if (! tfires.IsFieldNull(&tfires.m_Aspect))
					fires.m_Aspect = tfires.m_Aspect;
				if(newRec && tfires.IsFieldNull(&tfires.m_Aspect))
					fires.SetFieldNull(&fires.m_Aspect);

				if (! tfires.IsFieldNull(&tfires.m_FuelModel))
					fires.m_FuelModel = tfires.m_FuelModel;
				if(newRec && tfires.IsFieldNull(&tfires.m_FuelModel))
					fires.SetFieldNull(&fires.m_FuelModel);

				if (! tfires.IsFieldNull(&tfires.m_FireType))
					fires.m_FireType = tfires.m_FireType;
				if(newRec && tfires.IsFieldNull(&tfires.m_FireType))
					fires.SetFieldNull(&fires.m_FireType);*/

				if (! tfires.IsFieldNull(&tfires.m_Contain))
					fires.m_Contain = tfires.m_Contain;
				if(newRec && tfires.IsFieldNull(&tfires.m_Contain))
					fires.SetFieldNull(&fires.m_Contain);

				if (! tfires.IsFieldNull(&tfires.m_StrategyMet))
					fires.m_StrategyMet = tfires.m_StrategyMet;
				if(newRec && tfires.IsFieldNull(&tfires.m_StrategyMet))
					fires.SetFieldNull(&fires.m_StrategyMet);

				//if (! tfires.IsFieldNull(&tfires.m_FireOut))
				//	fires.m_FireOut = tfires.m_FireOut;
				//if(newRec && tfires.IsFieldNull(&tfires.m_FireOut))
				//	fires.SetFieldNull(&fires.m_FireOut);



				fires.Update();
				LogFireDataWarnings(errLog, &tfires);
				tfires.MoveNext();
			}
		}
	/*	if(!cd.Abort())
			tfires.m_pDatabase->CommitTrans(); //so we can requery
		else
			tfires.m_pDatabase->Rollback();
	*/	
	//}
	/*
	else
		tfires.m_pDatabase->Rollback();
		*/

	//to maintain referential integrity,
	

	//if(!cd || !cd->Abort())
	//{
		if(!m_RejectDups)
			fprintf(errLog, "Processed %ld fire records :\n"
	
				"\tAppended %ld records.\n"
				"\tUpdated %ld existing records.\n"
				"\tRejected %ld records for bad data.\n",
				count, appends, edits,  rejects );
		else
			fprintf(errLog, "Processed %ld fire records :\n"
	
				"\tAppended %ld records.\n"
				"\tRejected %ld duplicate records.\n"
				"\tRejected %ld records for bad data.\n",
				count, appends, dupRejects,  rejects - dupRejects );
		errors = rejects;
	//}
	//else
	//{
	//	errors = -1;
	//}

	// look for new units (which have been flagged with a negative UnitID)
   /* CFireUnitSet unitSet(pDB);
	unitSet.m_strFilter.Format("[UnitID] < 0");
    unitSet.Open();
	if (! unitSet.IsEOF()){
	    CString msgStr;
		msgStr.Format("The fire import file included new units, which have been added to this database.\n\nThe following new units have been added:\n\n");
             
		while (! unitSet.IsEOF()){
				CString unitStr,
					    unitStr2,
						unitStr3;

                CFireAgencySet agencySet(pDB);
				CFireRegionSet regSet(pDB);
			
				regSet.m_strFilter.Format("[RegionID] = %d", unitSet.m_RegionID);
				regSet.Open();
				
				
				
				if (! regSet.IsEOF()) {
                    agencySet.m_strFilter.Format("[AgencyID] = %d", regSet.m_AgencyID);
			        agencySet.Open();
				    if (! agencySet.IsEOF())
					    unitStr.Format("Agency: %s", agencySet.m_Name);
				    else
					    unitStr.Format("Agency: N/A");
				    agencySet.Close();

					unitStr2.Format("\tRegion: %s", regSet.m_Name);
				} else {
					unitStr.Format("Agency: N/A");
					unitStr2.Format("\tRegion: N/A");
				}
				regSet.Close();
  
				unitStr3.Format("\tUnit: (%d) %s\n", 0 - unitSet.m_UnitID, unitSet.m_Name);
			
                
				msgStr.Append(unitStr);
				msgStr.Append(unitStr2);
				msgStr.Append(unitStr3);

                 
                unitSet.Edit();
				unitSet.m_UnitID = 0 - unitSet.m_UnitID;
			   
			    unitSet.Update();
                unitSet.MoveNext();
		}
        AfxMessageBox(msgStr);

	}
    unitSet.Close();*/


	tfires.Close();
    fires.Close();
	

	try
	{
		pDoc->m_pDB->ExecuteSQL("DROP TABLE [ffpTempFire]");
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
//	pDoc->m_pDB->DeleteTableDef("ffpTempFire");
	//save dialog layout to database
	CGenericFireImportSet gfiSet(pDoc->m_pDB);
	gfiSet.m_strFilter.Format("[AgencyID] = %ld", agencyID);
	gfiSet.Open();
	if(!gfiSet.IsEOF())
		gfiSet.Edit();
	else
		gfiSet.AddNew();
	gfiSet.m_AgencyID = agencyID;
	if(space.GetCheck())
		gfiSet.m_Delimiter = 0;
	else if(comma.GetCheck())
		gfiSet.m_Delimiter = 1;
	else//semicolon
		gfiSet.m_Delimiter = 2;
	gfiSet.m_DateFormat = dt;
	gfiSet.m_TimeFormat = tt;
	gfiSet.m_UseRegion = m_useRegion.GetCheck();
	gfiSet.m_UseUnit = m_useUnit.GetCheck();
	gfiSet.m_UseSubunit = m_useSubunit.GetCheck();
	gfiSet.m_DefaultRegionID = defaultRegion;
	gfiSet.m_DupHandling = m_RejectDups;
	if(gfiSet.m_UseUnit)
	{
		int defUnit = 0;
		int uloc = m_comboUnits.GetCurSel();
		if(uloc != CB_ERR)
			defUnit = m_comboUnits.GetItemData(uloc);
		gfiSet.m_DefaultUnitID = defUnit;
	}
	else
		gfiSet.m_DefaultUnitID = -1;
	if(gfiSet.m_UseSubunit)
	{
		int defUnit = 0;
		int uloc = m_comboSubunits.GetCurSel();
		if(uloc != CB_ERR)
			defUnit = m_comboSubunits.GetItemData(uloc);
		gfiSet.m_DefaultSubunitID = defUnit;
	}
	else
		gfiSet.m_DefaultSubunitID = -1;
	gfiSet.Update();
	gfiSet.Close();
	//now add field selections
	CGenericFireFieldsSet gffSet(pDoc->m_pDB);
	gffSet.m_strFilter.Format("[AgencyID] = %ld", agencyID);
	gffSet.Open();
	while(!gffSet.IsEOF())
	{
		gffSet.Delete();
		gffSet.MoveNext();
	}
	for(int i = 0; i < selectedList.GetCount(); i++)
	{
		gffSet.AddNew();
		int field = selectedList.GetItemData(i);
		gffSet.m_AgencyID = (int)agencyID;
		gffSet.m_FieldNum = field;
		gffSet.m_Rank = i;
		gffSet.Update();
	}
	gffSet.Close();
//	if (cd)
//		cd->DestroyWindow();

	CDialog::OnOK();

	EnableWindow(TRUE);

	

 




}

void CGenericFireImportDialog::LogFireDataWarnings(FILE *logFile, CFiresSet *fireSet)
{
	
}
void CGenericFireImportDialog::OnBnClickedUseReg()
{
	if(m_useRegion.GetCheck() == TRUE)
	{
		m_comboRegions.EnableWindow(TRUE);	
	}
	else
	{
		m_comboRegions.SetCurSel(-1);
		m_comboRegions.EnableWindow(FALSE);	
		m_useUnit.SetCheck(FALSE);
		m_useUnit.EnableWindow(FALSE);
		OnBnClickedUseUnit();
	}
}

void CGenericFireImportDialog::OnCbnSelchangeRegs()
{
	int loc = m_comboRegions.GetCurSel();
	if(loc >= 0)
	{
		m_useUnit.EnableWindow(true);
		FillUnits(m_comboRegions.GetItemData(loc));
	}
}

void CGenericFireImportDialog::OnBnClickedUseUnit()
{
	if(m_useUnit.GetCheck() == TRUE)
	{
		m_comboUnits.EnableWindow(TRUE);	
	}
	else
	{
		m_comboUnits.SetCurSel(-1);
		m_comboUnits.EnableWindow(FALSE);	
		m_useSubunit.SetCheck(FALSE);
		m_useSubunit.EnableWindow(FALSE);
		OnBnClickedUseSubunit();
	}
}

void CGenericFireImportDialog::OnCbnSelchangeUnits()
{
	int loc = m_comboUnits.GetCurSel();
	if(loc >= 0)
	{
		m_useSubunit.EnableWindow(true);
		FillSubunits(m_comboUnits.GetItemData(loc));
	}	
}

void CGenericFireImportDialog::OnBnClickedUseSubunit()
{
	if(m_useSubunit.GetCheck() == TRUE)
	{
		m_comboSubunits.EnableWindow(TRUE);	
	}
	else
	{
		m_comboSubunits.SetCurSel(-1);
		m_comboSubunits.EnableWindow(FALSE);	
	}
}

void CGenericFireImportDialog::OnCbnSelchangeSubunits()
{
	// TODO: Add your control notification handler code here
}

void CGenericFireImportDialog::FillUnits(long regionID)
{
	m_comboUnits.ResetContent();
	unitSet.m_strFilter.Format("[RegionID] = %ld", regionID);
	unitSet.Requery();
	int loc;
	while(!unitSet.IsEOF())
	{
		loc = m_comboUnits.AddString(unitSet.m_Name);
		if(loc >= 0)
			m_comboUnits.SetItemData(loc, unitSet.m_UnitID);
		unitSet.MoveNext();
	}
	m_useSubunit.SetCheck(false);
	m_useSubunit.EnableWindow(false);
	OnBnClickedUseSubunit();
}

void CGenericFireImportDialog::FillSubunits(long unitID)
{
	m_comboSubunits.ResetContent();
	subSet.m_strFilter.Format("[UnitID] = %ld", unitID);
	subSet.Requery();
	int loc;
	while(!subSet.IsEOF())
	{
		loc = m_comboSubunits.AddString(subSet.m_Name);
		if(loc >= 0)
			m_comboSubunits.SetItemData(loc, subSet.m_SubunitID);
		subSet.MoveNext();
	}
}

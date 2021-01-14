// GenericWxImportDialog.cpp : implementation file
//
 
#include "stdafx.h"
#include <math.h>
#include <direct.h>
#include <cderr.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "GenericWxImportSet.h"
#include "GenericWxFieldsSet.h"

#include "fireplus.h"
#include "Fireplusview.h"
#include "FireplusSet.h"
#include "FireplusDoc.h"
#include "SIGStationSet.h"
#include "SelStationDialog.h"

#include "ImportDoneDialog.h"
#include "GenericWxImportDialog.h"

#include "WxSet.h"
#include ".\genericwximportdialog.h"
#include "WxStationSet.h"

using namespace std;   // needed for stream stuff.... std::<whatever> won't work :(

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;

#define WxFields 32
#define defaultWxFields 0

char *WxColNames[WxFields] =
{
	"Station ID*",//0
	"Obs Date*",
	"Obs Time*",
	"Obs Type*",
	"SOW",
	"Temp",//5
	"RH",
	"Wind Dir",
	"Wind Speed",
	"OMC10",
	"Obs MC (Wood)",//10
	"OMC Wood Date",
	"Temp - Max",
	"Temp - Min",
	"RH - Max",
	"RH - Min",//15
	"Precip Duration",
	"Precip Amt",
	"Season",
	"Herb Greenness",
	"Woody Greenness",//20
	"1-hr Moisture",
	"10-hr Moisture",
	"100-hr Moisture",
	"1000-hr Moisture",
	"Solar Radiation",//25
	"WetFlag",
	"Gust Dir",
	"Gust Speed",
    "Hourly Precip",
	"Rain Gauge",//30
	"Snow Flag"
};

char *WxFieldNames[WxFields] =
{
	"StationID", // 0
	"ObsDate", // 1
	"ObsTime", //2
	"ObsType", //3
	"SOW",  //4
	"Temp", //5
	"RH", //6
	"WDir",  //7
	"WS", //8
	"OMC10", //9
	"OMCWood", //10
	"OMCWoodDate", //11
	"TmpMax", //12
	"TmpMin", //13
	"RHMax", //14
	"RHMin", //15
	"PPTDUR", //16
	"PPTAMT", //17
	"Season", //18
	"GreenHerb", //19
	"GreenShrub", //20
	"MC1", //21
	"MC10", //22
	"MC100", //23
	"MC1000", //24
	"SolarRadiation", //25
	"WetFlag", //26
	"GustDir", //27
	"GustSpd", //28
    "HourlyPrecip", //29
	"RainGauge",//30
	"SnowFlag"//31
};

/////////////////////////////////////////////////////////////////////////////
// CGenericWxImportDialog dialog

CGenericWxImportDialog::CGenericWxImportDialog(CWnd* pParent /*=NULL*/, CWxSet *_records /*=NULL*/, CDatabase *_pDB )
	: CDialog(CGenericWxImportDialog::IDD, pParent)
	, m_radioRejectDups(0)
{
	//{{AFX_DATA_INIT(CGenericWxImportDialog)
	//}}AFX_DATA_INIT
	records = _records;
	m_RejectDups = FALSE;
	pDB = _pDB;
}

void CGenericWxImportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenericWxImportDialog)
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
	DDX_Control(pDX, IDC_STATIONS, m_comboStation);
	DDX_Control(pDX, IDC_OBSTYPE, m_comboObsType);
	DDX_Control(pDX, IDC_DATETIMEPICKER_OBSTIME, m_dtObsTime);
	DDX_Control(pDX, IDC_USESTATIONID, m_checkUseStation);
	DDX_Control(pDX, IDC_USEOBSTYPE, m_checkObsType);
	DDX_Control(pDX, IDC_USEOBSTIME, m_checkObsTime);
	DDX_Control(pDX, IDC_DUP_OVERWRITE, m_radioOverwrite);
	DDX_Control(pDX, IDC_CHECK_ALLSTATIONS, m_checkAllStations);
	DDX_Radio(pDX, IDC_DUP_OVERWRITE, m_RejectDups);
}

BEGIN_MESSAGE_MAP(CGenericWxImportDialog, CDialog)
	//{{AFX_MSG_MAP(CGenericWxImportDialog)
	ON_BN_CLICKED(IDC_DATE1, OnDate1)
	ON_BN_CLICKED(IDC_DATE2, OnDate2)
	ON_BN_CLICKED(IDC_DATE3, OnDate3)
	ON_BN_CLICKED(IDC_DATE4, OnDate4)
	ON_BN_CLICKED(IDC_DATE5, OnDate5)
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
	ON_BN_CLICKED(IDC_IMPORT_WX, OnGenericWx)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_USESTATIONID, OnBnClickedUseStationid)
	ON_BN_CLICKED(IDC_USEOBSTYPE, OnBnClickedUseObsType)
	ON_BN_CLICKED(IDC_USEOBSTIME, OnBnClickedUseObsTime)
	ON_CBN_SELCHANGE(IDC_STATIONS, OnCbnSelchangeStations)
	ON_BN_CLICKED(IDC_CHECK_ALLSTATIONS, OnBnClickedCheckAllstations)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenericWxImportDialog message handlers

BOOL CGenericWxImportDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_checkAllStations.SetCheck(FALSE);
	OnBnClickedCheckAllstations();

	dt = Full;
	tt = FullTime;

	date1.SetCheck(TRUE);

	int loc;
    //int defaults[WxFields];
    //for (int i=0;i<WxFields;i++)
		//defaults[i] = -1;

	// scan first line of the file... see if we can figure out the fields and delimiter....

    char line1[MAX_INPUT_LINE];
    char temp[MAX_INPUT_LINE];
	char *temp2;

	//ifstream stream;

	char delim = ',';

	//stream.open(fileName);
    //int fieldCount = 0;

	/*if( stream.good()){
		stream.getline(line1,MAX_INPUT_LINE-2,'\n');
		CString line;
		line.Format("%s",line1);
        stringstream stream2;
		stream2 << line1;
		int i=0;
        for (i=0; i < line.GetLength(); i++)
			if (line1[i] == ',' || line1[i]=='\t' || line1[i]==';'){
				delim = line1[i];
				break;
			}

        int f=0;
		for(f = 0; f < WxFields; f++){
		   stream2.getline(temp,MAX_FIELD_LEN,delim);
		   if (strlen(temp) > 0){
				temp2 = temp;
				// skip comments or quotes on field names
				if (temp[0]=='#' || temp[0] == '\'')
					temp2 = &(temp[1]);
				int l = strlen(temp2);
				if (temp2[l-1]=='\'')
				 temp2[l-1] = '\0';

				CString inputField, checkField;
				inputField.Format("%s",temp2);
				for (int i=0; i<WxFields;i++){
				   checkField.Format("%s",WxFieldNames[i]);
				   if (inputField.CompareNoCase(checkField.GetString()) == 0){
					   defaults[fieldCount] = i;
					   fieldCount ++;
				   }
				}
		   }
		}

        // now read first line of data
		stream.getline(line1,MAX_INPUT_LINE-2,'\n');
		line.Format("%s",line1);

		stream2 << line1;

		for(f = 0; f < WxFields; f++){
		   stream2.getline(temp,MAX_FIELD_LEN,delim);
		   if (strlen(temp) > 0){
				temp2 = temp;
				CString inputField, checkField;
				inputField.Format("%s",temp2);
				for (int i=0; i<WxFields;i++){
				   checkField.Format("%s",WxFieldNames[i]);
				   if (inputField.CompareNoCase(checkField.GetString()) == 0){
	                   // check date and time formats
					   if (i==1) { // date
						   if (temp2[2]=='/' && temp2[5]=='/')
	                              dt = Full;
						   else {
                               char datetemp[8];
							   datetemp[0]=temp2[0];
							   datetemp[1]=temp2[1];
							   datetemp[2] = '\0';
							   if (atoi(datetemp) <= 12)
								   dt = Packed;
							   else
							       dt = Reverse;
						   }
					   }
                       else if (i==2) { // time
							if (temp2[2]==':' )
	                              tt = FullTime;
						    else
								  tt = PackedTime;
					   }
				   }
				}
		   }
	   }
	}
	stream.close();*/
	// all fields to available list
	for(int f = 0; f < WxFields; f++){
		   // other fields - not selected by default
		   loc = fieldsList.AddString(WxColNames[f]);
		   fieldsList.SetItemData(loc, f);
	}
	// now move 'identified' fields to selected list
	/*for(int f = 0; f < WxFields; f++)
		if (defaults[f] > -1){
		   // add some defaults to selected list
		   int cur = fieldsList.FindString(0,WxColNames[defaults[f]]);
	       fieldsList.DeleteString(cur);

		   loc = selectedList.AddString(WxColNames[defaults[f]]);
		   selectedList.SetItemData(f,defaults[f]);
		} */

	upButton.AutoLoad(IDC_UP, this);
	downButton.AutoLoad(IDC_DOWN, this);
	topButton.AutoLoad(IDC_TOP, this);
	bottomButton.AutoLoad(IDC_BOTTOM, this);
	if ( dt == Packed)
        date2.SetCheck(TRUE);
	else if (dt == Reverse)
		date3.SetCheck(TRUE);

	if (tt == FullTime)
	{
		time1.SetCheck(TRUE);
        time2.SetCheck(FALSE);
		m_dtObsTime.SetFormat("HH:MM");
	}
	else
	{
        time1.SetCheck(FALSE);
        time2.SetCheck(TRUE);
		m_dtObsTime.SetFormat("HHMM");
	}

	CButton *button = (CButton *)GetDlgItem(IDC_COMMA);
	button->SetCheck(TRUE);

	//look for settings from previous agency import
	CGenericWxImportSet gfiSet(pDoc->m_pDB);
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
		}
		else if(gfiSet.m_DateFormat == 1)
		{
			((CButton *)GetDlgItem(IDC_DATE1))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_DATE2))->SetCheck(TRUE);
			((CButton *)GetDlgItem(IDC_DATE3))->SetCheck(FALSE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_DATE1))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_DATE2))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_DATE3))->SetCheck(TRUE);
		}
		if(gfiSet.m_TimeFormat == 0)
		{
			((CButton *)GetDlgItem(IDC_TIME1))->SetCheck(TRUE);
			((CButton *)GetDlgItem(IDC_TIME2))->SetCheck(FALSE);
		}
		else
		{
			((CButton *)GetDlgItem(IDC_TIME1))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_TIME2))->SetCheck(TRUE);
		}
		m_RejectDups = gfiSet.m_DupHandling;
	}
	gfiSet.Close();
	CGenericWxFieldsSet gffSet(pDoc->m_pDB);
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

/*	if (delim == ',')
	{
		CButton *button = (CButton *)GetDlgItem(IDC_COMMA);
		button->SetCheck(TRUE);
	}
	else if (delim == '\t')
	{
		CButton *button = (CButton *)GetDlgItem(IDC_SPACE);
		button->SetCheck(TRUE);
	}
	else
	{
		CButton *button = (CButton *)GetDlgItem(IDC_NONE);
		button->SetCheck(TRUE);
	}
	this->m_dtObsTime.SetFormat("H:m");

	bool overwriteDups = true;
	if(overwriteDups)
	{
		m_radioOverwrite.SetCheck(TRUE);
		m_radioRejectDups = FALSE;
		((CButton *)GetDlgItem(IDC_DUP_REJECT))->SetCheck(m_radioRejectDups);
	}
	else
	{
		m_radioOverwrite.SetCheck(FALSE);
		m_radioRejectDups = TRUE;
		((CButton *)GetDlgItem(IDC_DUP_REJECT))->SetCheck(m_radioRejectDups);
	}*/

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGenericWxImportDialog::OnDate1()
{
	dt = Full;
}

void CGenericWxImportDialog::OnDate2()
{
	dt = Packed;
}

void CGenericWxImportDialog::OnDate3()
{
	dt = Reverse;
}

void CGenericWxImportDialog::OnDate4()
{
	dt = MD;
}

void CGenericWxImportDialog::OnDate5()
{
	dt = MDPacked;
}

void CGenericWxImportDialog::OnTime1()
{
	tt = FullTime;
	m_dtObsTime.SetFormat("HH:MM");
}

void CGenericWxImportDialog::OnTime2()
{
	tt = PackedTime;
	m_dtObsTime.SetFormat("HHMM");
}

void CGenericWxImportDialog::OnTop()
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

void CGenericWxImportDialog::OnUp()
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

void CGenericWxImportDialog::OnBottom()
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

void CGenericWxImportDialog::OnDown()
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

void CGenericWxImportDialog::OnDblclkFieldslist()
{
	CString str;
	int cur = fieldsList.GetCurSel();
	fieldsList.GetText(cur, str);
	int val = fieldsList.GetItemData(cur);
	int loc = selectedList.AddString(str);
	selectedList.SetItemData(loc, val);
	fieldsList.DeleteString(cur);
}

void CGenericWxImportDialog::OnDblclkSelectedlist()
{
	CString str;
	int cur = selectedList.GetCurSel();
	selectedList.GetText(cur, str);
	int val = selectedList.GetItemData(cur);
	int loc = fieldsList.AddString(str);
	fieldsList.SetItemData(loc, val);
	selectedList.DeleteString(cur);
}

void CGenericWxImportDialog::OnMoveleft()
{
	if(selectedList.GetCurSel() != LB_ERR)
		OnDblclkSelectedlist();
}

void CGenericWxImportDialog::OnMoveright()
{
	if(fieldsList.GetCurSel() != LB_ERR)
		OnDblclkFieldslist();
}

void CGenericWxImportDialog::OnSelectall()
{
	while(fieldsList.GetCount() > 0)
	{
		fieldsList.SetCurSel(0);
		OnDblclkFieldslist();
	}
}

void CGenericWxImportDialog::OnRemoveall()
{
	while(selectedList.GetCount() > 0)
	{
		selectedList.SetCurSel(0);
		OnDblclkSelectedlist();
	}
}

void CGenericWxImportDialog::OnGenericWx(){
    errors = 0;

	CWaitCursor wait;

	fprintf(errLog, "**************************************************\nImport File Name: %s\n", fileName);
	// EnableWindow(FALSE);
/*	//CCancelDialog *cd;
	//cd = new CCancelDialog();

	//if (cd != NULL)
	//{
		BOOL ret = cd->Create(IDD_CANCELDIALOG,this);

      if (!ret)   //Create failed.
      {
        // AfxMessageBox(_T("FFP will now perform database upgrade checks..."));
		
		 cd = 0;
      } else
	  {
	     cd->Reset();
         cd->ShowWindow(SW_SHOW);
	    cd->SetStatusString("Reading: " + fileName);
		cd->SetTitle("Importing Generic Wx Data");
	  }
	//}*/
	

	ifstream stream;

	stream.open(fileName);

	long count = 0, updates = 0, rejects = 0;
    int lastRegionID = 0;

    char line[MAX_INPUT_LINE];
    char temp[MAX_INPUT_LINE];
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

	if(pDoc->HasTempWx())
	{
		try
		{
			CString strSql = "DROP TABLE [ffpTempWx]";
			pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}

		//pDoc->m_pDB->DeleteTableDef("ffpTempWx");
	}
	pDoc->CreateTempWx();

    CList<CString, CString> staList;

	CWxSet wxObs(pDB);
	wxObs.m_strSort = "[ObsDate]";
	wxObs.Open();

	CWxSet twxObs(pDB);
	twxObs.m_strSort = "[ObsDate]";
	twxObs.Open(CRecordset::dynaset, "ffpTempWx");

	//do we have station in list?
	// stationID from dialog (if not read from file)
	CString stationID="";

	bool hasStationID = false;
	for(int f = 0; f < selectedList.GetCount(); f++)
	{
		int loc = selectedList.GetItemData(f);
		if(loc == 0)
		{
			hasStationID = true;
			break;
		}
	}
	if(!hasStationID)
	{//need to get a stationID to apply to imports...
		//first see if one was selected...
		if(m_checkUseStation.GetCheck())
		{
			char tStaStr[64];
			m_comboStation.GetWindowText(tStaStr,63);
			tStaStr[6] = 0;
			stationID = tStaStr;
		}
		if(stationID.GetLength() <= 0)
		{
			CSIGStationSet stations(wxObs.m_pDatabase);
			stations.m_strSort = "[StationID]";
			stations.Open();
			CSelStationDialog sd(NULL, &stations);
			int stares = sd.DoModal();
			stations.Close();
			if(stares != IDOK)
				return;
			stationID = sd.selectedID;
		}
	}
	char obsType[16] = "", obsTime[16] = "";
	bool staticObsType = false, staticObsTime = false;
	if(this->m_checkObsType.GetCheck())
	{
		this->m_comboObsType.GetWindowText(obsType, 16);
		staticObsType = true;
	}
	if(this->m_checkObsTime.GetCheck())
	{
		this->m_dtObsTime.GetWindowText(obsTime, 16);
		staticObsTime = true;
	}
	twxObs.m_pDatabase->BeginTrans();

	float oldRainGauge = 0.0;

	/* 2014 new flag - set to true if wDir > 8 */

	bool wdir360 = false;


	while (stream.good())// && (! cd ||  ! cd->Abort()))
	{
	   // stationID from file
	   char sStationID[MAX_FIELD_LEN] = "";

	   char sObsDate[MAX_FIELD_LEN] = "",
		sObsTime[MAX_FIELD_LEN] = "",
		sObsType[MAX_FIELD_LEN] = "",
		sSOW[MAX_FIELD_LEN] = "",
		sTemp[MAX_FIELD_LEN] = "",
		sRH[MAX_FIELD_LEN] = "",
		sWDir[MAX_FIELD_LEN] = "",
		sWS[MAX_FIELD_LEN] = "",
		sOMC10[MAX_FIELD_LEN] = "",
		sOMCWood[MAX_FIELD_LEN] = "",
		sOMCWoodDate[MAX_FIELD_LEN] = "",
		sTmpMax[MAX_FIELD_LEN] = "",
		sTmpMin[MAX_FIELD_LEN] = "",
		sRHMax[MAX_FIELD_LEN] = "",
		sRHMin[MAX_FIELD_LEN] = "",
		sPPTDUR[MAX_FIELD_LEN] = "",
		sPPTAMT[MAX_FIELD_LEN] = "",
		sSeason[MAX_FIELD_LEN] = "",
		sGreenHerb[MAX_FIELD_LEN] = "",
		sGreenShrub[MAX_FIELD_LEN] = "",
		sMC1[MAX_FIELD_LEN] = "",
		sMC10[MAX_FIELD_LEN] = "",
		sMC100[MAX_FIELD_LEN] = "",
		sMC1000[MAX_FIELD_LEN] = "",
		sSolarRadiation[MAX_FIELD_LEN] = "",
		sWetFlag[MAX_FIELD_LEN] = "",
		sGustDir[MAX_FIELD_LEN] = "",
		sGustSpd[MAX_FIELD_LEN] = "",
		sHourlyPrecip[MAX_FIELD_LEN] = "",
		sRainGauge[MAX_FIELD_LEN] = "",
		sComplete[MAX_INPUT_LINE] = "",
		   sSnowFlag[MAX_INPUT_LINE] = "";

	   char logMsg[1000] = "";

	   stream.getline(line,MAX_INPUT_LINE,'\n');

		temp[0] = line[0];
		temp[2] = '\0';
		if (temp[0]=='#' || temp[0]=='\n' || temp[0]=='\r' || temp[0] == 0)// || (temp[0] != 0 && atoi(temp) == 0) )
		{ // not an input line
			continue;
		}

        stringstream stream2;

		stream2 << line;

		for(int f = 0; f < selectedList.GetCount(); f++)
		{
			int loc = selectedList.GetItemData(f);

			switch(loc)
			{
			case 0:
				 stream2.getline(sStationID,MAX_FIELD_LEN,delim); break;
			case 1:
				 stream2.getline(sObsDate,MAX_FIELD_LEN,delim); break;
			case 2:
				 stream2.getline(sObsTime,MAX_FIELD_LEN,delim); break;
			case 3:
				 stream2.getline(sObsType,MAX_FIELD_LEN,delim); break;
			case 4:
				 stream2.getline(sSOW,MAX_FIELD_LEN,delim); break;
			case 5:
				 stream2.getline(sTemp,MAX_FIELD_LEN,delim); break;
			case 6:
				 stream2.getline(sRH,MAX_FIELD_LEN,delim); break;
            case 7:
				 stream2.getline(sWDir,MAX_FIELD_LEN,delim); break;
            case 8:
				 stream2.getline(sWS,MAX_FIELD_LEN,delim); break;
            case 9:
				 stream2.getline(sOMC10,MAX_FIELD_LEN,delim); break;
            case 10:
				 stream2.getline(sOMCWood,MAX_FIELD_LEN,delim); break;
            case 11:
				 stream2.getline(sOMCWoodDate,MAX_FIELD_LEN,delim); break;
			case 12:
				 stream2.getline(sTmpMax,MAX_FIELD_LEN,delim); break;
            case 13:
				 stream2.getline(sTmpMin,MAX_FIELD_LEN,delim); break;
            case 14:
				 stream2.getline(sRHMax,MAX_FIELD_LEN,delim); break;
            case 15:
				 stream2.getline(sRHMin,MAX_FIELD_LEN,delim); break;
            case 16:
				 stream2.getline(sPPTDUR,MAX_FIELD_LEN,delim); break;
            case 17:
				 stream2.getline(sPPTAMT,MAX_FIELD_LEN,delim); break;
            case 18:
				 stream2.getline(sSeason,MAX_FIELD_LEN,delim); break;
            case 19:
				 stream2.getline(sGreenHerb,MAX_FIELD_LEN,delim); break;
            case 20:
				 stream2.getline(sGreenShrub,MAX_FIELD_LEN,delim); break;
            case 21:
				 stream2.getline(sMC1,MAX_FIELD_LEN,delim); break;
            case 22:
				 stream2.getline(sMC10,MAX_FIELD_LEN,delim); break;
            case 23:
				 stream2.getline(sMC100,MAX_FIELD_LEN,delim); break;
            case 24:
				 stream2.getline(sMC1000,MAX_FIELD_LEN,delim); break;
            case 25:
				 stream2.getline(sSolarRadiation,MAX_FIELD_LEN,delim); break;
            case 26:
				 stream2.getline(sWetFlag,MAX_FIELD_LEN,delim); break;
            case 27:
				 stream2.getline(sGustDir,MAX_FIELD_LEN,delim); break;
            case 28:
				 stream2.getline(sGustSpd,MAX_FIELD_LEN,delim); break;
		    case 29:
				 stream2.getline(sHourlyPrecip,MAX_FIELD_LEN,delim); break;
 		    case 30:
				 stream2.getline(sRainGauge,MAX_FIELD_LEN,delim); break;
			case 31:
				stream2.getline(sSnowFlag, MAX_FIELD_LEN, delim); break;
				break;
			default: break;
			}
		}

		// get any additional data from end of record...
		stream2.getline(sComplete,MAX_INPUT_LINE,delim);
		//static values?
		if(staticObsType)
			strcpy(sObsType, obsType);
		if(staticObsTime)
			strcpy(sObsTime, obsTime);

		COleDateTime obsDate;

		count ++;
	/*	if (cd)
           cd->Increment();*/

		sprintf(logMsg,"%s %s",sObsDate,sObsTime);

		if(strlen(sStationID) <= 0 && stationID.GetLength() > 0)
			strcpy(sStationID, stationID);
		if (strlen(sStationID) < 1)
		{
			if (stationID.GetLength() < 1)
			{
               // no stationID? error
               fprintf(errLog, "\tRecord Number %d, no StationID\n", count );
			   rejects ++;
			   continue;
			}
		}

		if (strlen(sStationID) < 6)
		{
			  // no stationID? error
               fprintf(errLog, "\tRecord Number %d, StationID '%s' is not 6 chars in length\n", count,sStationID );
			   rejects ++;
			   continue;
		}

		if (IsNullValue(sObsDate)){
            fprintf(errLog, "\tRecord Number %d, no ObsDate\n", count );
			rejects ++;
			continue;
		}

		if (strlen(sObsDate) < 8 ){
            fprintf(errLog, "\tRecord Number %d, ObsDate is not valid: %s\n", count,logMsg );
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
	      tdate[0] = sObsDate[0];
		  tdate[1] = sObsDate[1];
          tdate[2] = '\0';
		  m = atoi(tdate);

		  tdate[0] = sObsDate[2];
		  tdate[1] = sObsDate[3];
          tdate[2] = '\0';
		  d = atoi(tdate);

		  tdate[0] = sObsDate[4];
		  tdate[1] = sObsDate[5];
	      tdate[2] = sObsDate[6];
		  tdate[3] = sObsDate[7];
          tdate[4] = '\0';
		  y = atoi(tdate);
		} else if (dt == Reverse){ // YYYYMMDD
		  tdate[0] = sObsDate[0];
		  tdate[1] = sObsDate[1];
	      tdate[2] = sObsDate[2];
		  tdate[3] = sObsDate[3];
          tdate[4] = '\0';
		  y = atoi(tdate);

	      tdate[0] = sObsDate[4];
		  tdate[1] = sObsDate[5];
          tdate[2] = '\0';
		  m = atoi(tdate);

		  tdate[0] = sObsDate[6];
		  tdate[1] = sObsDate[7];
          tdate[2] = '\0';
		  d = atoi(tdate);
		} else {  // MM/DD/YYYY
			int nPos = 0;

		  if (sObsDate[1]=='/') // we need to add a leading zero
		  {
				tdate[0] = '0';
				tdate[1] = sObsDate[0];			
				tdate[2] = '\0';
				m = atoi(tdate);
				nPos = 2;

		
		  } else {
			    tdate[0] = sObsDate[0];
				tdate[1] = sObsDate[1];
				tdate[2] = '\0';
				m = atoi(tdate);
				nPos = 3;

		  }

		  if (sObsDate[nPos + 1]=='/') // we need to add a leading zero
		  {
				tdate[0] = '0';
				tdate[1] = sObsDate[nPos];
				tdate[2] = '\0';
				d = atoi(tdate);
				nPos += 2; 

		  } else
		  {
				tdate[0] = sObsDate[3];
				tdate[1] = sObsDate[4];
				tdate[2] = '\0';
				d = atoi(tdate);
				nPos += 3;
		  }

		  tdate[0] = sObsDate[nPos];
		  tdate[1] = sObsDate[nPos+1];
	      tdate[2] = sObsDate[nPos+2];
		  tdate[3] = sObsDate[nPos+3];
          tdate[4] = '\0';
		  y = atoi(tdate);

		}

		if (y < 1900 || y > 2100){
            fprintf(errLog, "\tRecord Number %d, ObsDate is not valid (bad year): %s.\n", count,logMsg );
			rejects ++;
			continue;
		}
		if (m < 1 || m > 12){
            fprintf(errLog, "\tRecord Number %d, ObsDate is not valid (bad month): %s.\n", count,logMsg );
			rejects ++;
			continue;
		}
		if (d < 1 || d > 31){
            fprintf(errLog, "\tRecord Number %d, ObsDate is not valid (bad day): %s.\n", count,logMsg );
			rejects ++;
			continue;
		}

		if (strlen(sObsTime) > 0){
           if (strlen(sObsTime) <= 2)
			   hr = atoi(sObsTime);
		   else if (strlen(sObsTime)==3 || strlen(sObsTime) > 5) {
               fprintf(errLog, "\tRecord Number %d, ObsTime is not valid (bad format): %s.\n", count,logMsg );
			   rejects ++;
			   continue;
		   } else {   // normal format
			   if (tt == FullTime){  // HH:MM
		          tdate[0] = sObsTime[0];
		          tdate[1] = sObsTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sObsTime[3];
		          tdate[1] = sObsTime[4];
                  tdate[2] = '\0';
		          min = atoi(tdate);
			   } else { // HHMM
                  tdate[0] = sObsTime[0];
		          tdate[1] = sObsTime[1];
                  tdate[2] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sObsTime[2];
		          tdate[1] = sObsTime[3];
                  tdate[2] = '\0';
		          min = atoi(tdate);
		       }
		   }
		}
		if (hr < 0 || hr > 23){
               fprintf(errLog, "\tRecord Number %d, ObsTime is not valid (bad hour): %s.\n", count,logMsg );
			   rejects ++;
			   continue;
		}
		if (min < 0 || min > 59){
		       fprintf(errLog, "\tRecord Number %d, ObsTime is not valid (bad minute): %s.\n", count,logMsg );
			   rejects ++;
			   continue;
		}

		obsDate.SetDateTime(y, m, d, hr, 0 /*min*/, 0);  // set minute to 0

		if(obsDate.GetStatus() != 0 || y < 1900)
		{
			fprintf(errLog, "\tRecord Number %ld, Date/Time is not valid: %d/%d/%04d\n", count ,  m, d, y);
			rejects ++;

			continue;
		}
		COleDateTime today;
		today = COleDateTime::GetCurrentTime();

		if(!theApp.m_enableFuture)
		if(obsDate > today)
		{
			fprintf(errLog, "\tRecord Number %ld, Date is not valid (future): %d/%d/%04d\n", count,  m, d, y);
			rejects ++;

			continue;
		}

        twxObs.AddNew();
		if(staList.IsEmpty())
			staList.AddTail(sStationID);
		else
		{
			bool there = false;
			POSITION pos = staList.GetHeadPosition();
			while(pos)
			{
				if(staList.GetNext(pos) == sStationID)
				{
					there = true;
					break;
				}
			}
			if(!there)
				staList.AddHead(sStationID);
		}
		twxObs.m_StationID = sStationID;
        twxObs.m_ObsDate = obsDate;
		if (hr != 13)
			twxObs.m_ObsType = "R";
		else
			twxObs.m_ObsType = "O";
		twxObs.m_Hour = obsDate.GetHour();
		if (!IsNullValue(sSOW))
			twxObs.m_SOW = atoi(sSOW);
		else
		    twxObs.SetFieldNull(&twxObs.m_SOW);

		if (!IsNullValue(sTemp))
			twxObs.m_Temp = atoi(sTemp);
		else
			twxObs.SetFieldNull(&twxObs.m_Temp);

		if (!IsNullValue(sRH))
			twxObs.m_RH = max(atoi(sRH), 1);
		else
			twxObs.SetFieldNull(&twxObs.m_RH);
		
		/* note: Generic Wx modified 2014 to handle wind direction / azimuth properly 
		   - first import raw value, 
		     then check it later and convert to 8-point system if necessary
		*/

		if (!IsNullValue(sWDir))
		{
			twxObs.m_WDir = atoi(sWDir);
			if (twxObs.m_WDir > 8)
				wdir360 = true;
		} else
			twxObs.SetFieldNull(&twxObs.m_WDir);

		if (!IsNullValue(sWS))
			twxObs.m_WS = atoi(sWS);
		else
			twxObs.SetFieldNull(&twxObs.m_WS);

		if (!IsNullValue(sOMC10))
			twxObs.m_OMC10 = atof(sOMC10);
		else
			twxObs.SetFieldNull(&twxObs.m_OMC10);

		if (!IsNullValue(sOMCWood))
			twxObs.m_OMCWood = atof(sOMCWood);
		else
			twxObs.SetFieldNull(&twxObs.m_OMCWood);

		/* if (strlen(sOMC10Date) > 0)
			twxObs.m_OMCWoodDate = sOMC10WoodDate;
		else */
			twxObs.SetFieldNull(&twxObs.m_OMCWoodDate);

		if (!IsNullValue(sTmpMax))
			twxObs.m_TmpMax = atoi(sTmpMax);
		else
			twxObs.SetFieldNull(&twxObs.m_TmpMax);

		if (!IsNullValue(sTmpMin))
			twxObs.m_TmpMin = atoi(sTmpMin);
		else
			twxObs.SetFieldNull(&twxObs.m_TmpMin);

		if (!IsNullValue(sRHMax))
			twxObs.m_RHMax = atoi(sRHMax);
		else
			twxObs.SetFieldNull(&twxObs.m_RHMax);

		if (!IsNullValue(sRHMin))
			twxObs.m_RHMin = max(atoi(sRHMin), 1);
		else
			twxObs.SetFieldNull(&twxObs.m_RHMin);

		if (!IsNullValue(sPPTDUR))
			twxObs.m_PPTDUR = atoi(sPPTDUR);
		else
			twxObs.SetFieldNull(&twxObs.m_PPTDUR);

		if (!IsNullValue(sPPTAMT))
			twxObs.m_PPTAMT = atof(sPPTAMT);
		else
			twxObs.SetFieldNull(&twxObs.m_PPTAMT);

		if (!IsNullValue(sSeason))
			twxObs.m_Season = atoi(sSeason);
		else
			twxObs.SetFieldNull(&twxObs.m_Season);

		if (!IsNullValue(sGreenHerb))
			twxObs.m_GreenHerb = atoi(sGreenHerb);
		else
			twxObs.SetFieldNull(&twxObs.m_GreenHerb);

		if (!IsNullValue(sGreenShrub))
			twxObs.m_GreenShrub = atoi(sGreenShrub);
		else
			twxObs.SetFieldNull(&twxObs.m_GreenShrub);

		if (!IsNullValue(sMC1))
			twxObs.m_MC1 = atof(sMC1);
		else
			twxObs.SetFieldNull(&twxObs.m_MC1);

		if (!IsNullValue(sMC10))
			twxObs.m_MC10 = atof(sMC10);
		else
			twxObs.SetFieldNull(&twxObs.m_MC10);

		if (!IsNullValue(sMC100))
			twxObs.m_MC100 = atof(sMC100);
		else
			twxObs.SetFieldNull(&twxObs.m_MC100);

		if (!IsNullValue(sMC1000))
			twxObs.m_MC1000 = atof(sMC1000);
		else
			twxObs.SetFieldNull(&twxObs.m_MC1000);

		if (!IsNullValue(sSolarRadiation))
			twxObs.m_SolarRadiation = atol(sSolarRadiation);
		else
			twxObs.SetFieldNull(&twxObs.m_SolarRadiation);

		if (!IsNullValue(sWetFlag)){
			if (sWetFlag[0] == 'Y' || sWetFlag[0]=='y' || sWetFlag[0]=='1')
				twxObs.m_WetFlag = 1;
			else
				twxObs.m_WetFlag = 0;
		} else
			twxObs.SetFieldNull(&twxObs.m_WetFlag);

		if (!IsNullValue(sGustDir))
			twxObs.m_GustDir = atoi(sGustDir);
		else
			twxObs.SetFieldNull(&twxObs.m_GustDir);

		if (!IsNullValue(sGustSpd))
			twxObs.m_GustSpd = atoi(sGustSpd);
		else
			twxObs.SetFieldNull(&twxObs.m_GustSpd);

		if (!IsNullValue(sHourlyPrecip))
			twxObs.m_HourlyPrecip = atof(sHourlyPrecip);
		else
			twxObs.SetFieldNull(&twxObs.m_HourlyPrecip);

		if (!IsNullValue(sRainGauge)){
           if (count == 1)
			   twxObs.m_HourlyPrecip = 0;
		   else
			   twxObs.m_HourlyPrecip = atof(sRainGauge) - oldRainGauge;
		   oldRainGauge = atof(sRainGauge);
		}
		if (!IsNullValue(sSnowFlag))
		{
			twxObs.m_SnowFlag = atoi(sSnowFlag);
		}
		else
			twxObs.SetFieldNull(&twxObs.m_SnowFlag);
		twxObs.Update();
	}

	stream.close();

	 long appends = 0, edits = 0;

	//if (!cd || ! cd->Abort()){
    	COleDateTime lastDay;
	   //CTime lastDay;  long edits = 0, appends = 0;
	   twxObs.m_pDatabase->CommitTrans(); //so we can requery
	   /*if (cd)
	   {
	      cd->Reset();
	      cd->SetStatusString("Updating Database");
	   }*/
	   POSITION pos = staList.GetHeadPosition();
		while(pos)// && !cd->Abort())
		{//process for each station
			CString strSQL, sta;
			sta = staList.GetNext(pos);
			strSQL.Format("[StationID] = '%6.6s'", sta);
			wxObs.m_strFilter = strSQL;
			twxObs.m_strFilter = strSQL;
			wxObs.Requery();
			twxObs.Requery();
			lastDay.SetDateTime(1900,1, 1, 1, 1, 1);//minimum start date
			//lastDay.SetTime(1900,1, 1, 1, 1, 1);//minimum start date
			twxObs.m_pDatabase->BeginTrans();

			CSIGStationSet stnSet(pDB);
			stnSet.Open();
			// look for station record
			stnSet.m_strFilter.Format("[StationID] = '%6.6s'", sta);
			stnSet.Requery();
			int regHour = 13;
			if(! stnSet.IsEOF())
			{
				regHour = stnSet.m_RegSchdObs;
			}
			stnSet.Close();

			while(!twxObs.IsEOF())// && !cd->Abort())
			{
				//if (cd)
				 //  cd->Increment();
				COleDateTime tTime(twxObs.m_ObsDate.GetYear(), twxObs.m_ObsDate.GetMonth(),
					twxObs.m_ObsDate.GetDay(), twxObs.m_ObsDate.GetHour(), twxObs.m_ObsDate.GetMinute(),
					twxObs.m_ObsDate.GetSecond());
				while(!twxObs.IsEOF() && tTime <= lastDay)//may be dups in twxObs
				{
					twxObs.MoveNext();
					tTime.SetDateTime(twxObs.m_ObsDate.GetYear(), twxObs.m_ObsDate.GetMonth(),
					twxObs.m_ObsDate.GetDay(), twxObs.m_ObsDate.GetHour(), twxObs.m_ObsDate.GetMinute(),
					twxObs.m_ObsDate.GetSecond());
				}
				if(twxObs.IsEOF())
					break;
				lastDay.SetDateTime(twxObs.m_ObsDate.GetYear(), twxObs.m_ObsDate.GetMonth(),
					twxObs.m_ObsDate.GetDay(), twxObs.m_ObsDate.GetHour(), twxObs.m_ObsDate.GetMinute(),
					twxObs.m_ObsDate.GetSecond());
					//= twxObs.m_ObsDate;
				while(!wxObs.IsEOF() && (wxObs.m_ObsDate < twxObs.m_ObsDate))
					wxObs.MoveNext();
				int newRec = false;

				if(!wxObs.IsEOF() && (wxObs.m_ObsDate == twxObs.m_ObsDate))
				{
					wxObs.Edit();
					edits++;
				} else {
				   wxObs.AddNew();
				   appends++;
				   newRec = true;
				}

				/*for(int i = 0; i < wxObs.GetODBCFieldCount(); i++)
				{
					CDBVariant var;
					twxObs.GetFieldValue(i, var);
					wxObs.SetSetFieldValue(i, var);
				}*/
				if (! twxObs.IsFieldNull(&twxObs.m_StationID))
				    wxObs.m_StationID = twxObs.m_StationID;
				if (! twxObs.IsFieldNull(&twxObs.m_ObsDate))
				{
					wxObs.m_ObsDate = twxObs.m_ObsDate;
					wxObs.m_Hour = twxObs.m_ObsDate.GetHour();
				}
				if (! twxObs.IsFieldNull(&twxObs.m_ObsType))
					wxObs.m_ObsType = twxObs.m_ObsType;

				if (! twxObs.IsFieldNull(&twxObs.m_UserWX))
					wxObs.m_UserWX = twxObs.m_UserWX;

				if(newRec && twxObs.IsFieldNull(&twxObs.m_UserWX))
					wxObs.SetFieldNull(&wxObs.m_UserWX);

				if (! twxObs.IsFieldNull(&twxObs.m_SOW))
					wxObs.m_SOW = twxObs.m_SOW;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_SOW))
					wxObs.SetFieldNull(&wxObs.m_SOW);

				if (! twxObs.IsFieldNull(&twxObs.m_Temp))
					wxObs.m_Temp = twxObs.m_Temp;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_Temp))
					wxObs.SetFieldNull(&wxObs.m_Temp);

				if (! twxObs.IsFieldNull(&twxObs.m_RH))
					wxObs.m_RH = max(twxObs.m_RH, 1);
				if(newRec && twxObs.IsFieldNull(&twxObs.m_RH))
					wxObs.SetFieldNull(&wxObs.m_RH);

				if (! twxObs.IsFieldNull(&twxObs.m_WDir))
				{
					if (wdir360) // imported values in degrees: 0-360
					{
						wxObs.m_WDir = twxObs.m_WDir;
						int gDir = wxObs.m_WDir;
						int d=0;
						
						if(gDir >= 0 && gDir <= 360)
						 {
							if(gDir > 337 || gDir < 23)
								 d = 8;
							else if(gDir > 22 && gDir < 68)
								d = 1;
							else if(gDir > 67 && gDir < 113)
								d = 2;
							else if(gDir > 112 && gDir < 158)
								d = 3;
							else if(gDir > 157 && gDir < 203)
								d = 4;
							else if(gDir > 202 && gDir < 248)
								d = 5;
							else if(gDir > 247 && gDir < 293)
								d = 6;
							else if(gDir > 292 && gDir < 338)
								d = 7;
							wxObs.m_WDir = d;
							
						 }
						wxObs.m_WAzimuth = gDir;

					} else // imported values in 'points': 1-8
					{
						wxObs.m_WDir = twxObs.m_WDir;
						wxObs.m_WAzimuth = (wxObs.m_WDir * 45);
						if (wxObs.m_WAzimuth < 0)
							wxObs.m_WAzimuth = wxObs.m_WAzimuth + 360;
						if (wxObs.m_WAzimuth > 359)
							wxObs.m_WAzimuth = 0;
						
                   }
               }
               else
               {
                   twxObs.SetFieldNull(&twxObs.m_WDir);
                   twxObs.SetFieldNull(&twxObs.m_WAzimuth);
               }

				

				
				if(newRec && twxObs.IsFieldNull(&twxObs.m_WDir))
				{
					wxObs.SetFieldNull(&wxObs.m_WDir);
					wxObs.SetFieldNull(&wxObs.m_WAzimuth);
				}

				if (! twxObs.IsFieldNull(&twxObs.m_WS))
					wxObs.m_WS = twxObs.m_WS;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_WS))
					wxObs.SetFieldNull(&wxObs.m_WS);

				if (! twxObs.IsFieldNull(&twxObs.m_OMC10))
					wxObs.m_OMC10 = twxObs.m_OMC10;

				if(newRec && twxObs.IsFieldNull(&twxObs.m_OMC10))
					wxObs.SetFieldNull(&wxObs.m_OMC10);

				if (! twxObs.IsFieldNull(&twxObs.m_OMCWood))
					wxObs.m_OMCWood = twxObs.m_OMCWood;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_OMCWood))
					wxObs.SetFieldNull(&wxObs.m_OMCWood);

				if (! twxObs.IsFieldNull(&twxObs.m_OMCWoodDate))
					wxObs.m_OMCWoodDate = twxObs.m_OMCWoodDate;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_OMCWoodDate))
					wxObs.SetFieldNull(&wxObs.m_OMCWoodDate);

				if (! twxObs.IsFieldNull(&twxObs.m_TmpMax))
					wxObs.m_TmpMax = twxObs.m_TmpMax;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_TmpMax))
					wxObs.SetFieldNull(&wxObs.m_TmpMax);

				if (! twxObs.IsFieldNull(&twxObs.m_TmpMin))
				    wxObs.m_TmpMin = twxObs.m_TmpMin;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_TmpMin))
					wxObs.SetFieldNull(&wxObs.m_TmpMin);

				if (! twxObs.IsFieldNull(&twxObs.m_RHMax))
					wxObs.m_RHMax = twxObs.m_RHMax;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_RHMax))
					wxObs.SetFieldNull(&wxObs.m_RHMax);

				if (! twxObs.IsFieldNull(&twxObs.m_RHMin))
					wxObs.m_RHMin = max(twxObs.m_RHMin, 1);
				if(newRec && twxObs.IsFieldNull(&twxObs.m_RHMin))
					wxObs.SetFieldNull(&wxObs.m_RHMin);

				if (! twxObs.IsFieldNull(&twxObs.m_PPTDUR))
					wxObs.m_PPTDUR = twxObs.m_PPTDUR;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_PPTDUR))
					wxObs.SetFieldNull(&wxObs.m_PPTDUR);

				if (! twxObs.IsFieldNull(&twxObs.m_PPTAMT))
					wxObs.m_PPTAMT = twxObs.m_PPTAMT;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_PPTAMT))
					wxObs.SetFieldNull(&wxObs.m_PPTAMT);

				if (! twxObs.IsFieldNull(&twxObs.m_Season))
					wxObs.m_Season = twxObs.m_Season;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_Season))
					wxObs.SetFieldNull(&wxObs.m_Season);

				if (! twxObs.IsFieldNull(&twxObs.m_GreenHerb))
					wxObs.m_GreenHerb = twxObs.m_GreenHerb;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_GreenHerb))
					wxObs.SetFieldNull(&wxObs.m_GreenHerb);

				if (! twxObs.IsFieldNull(&twxObs.m_GreenShrub))
					wxObs.m_GreenShrub = twxObs.m_GreenShrub;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_GreenShrub))
					wxObs.SetFieldNull(&wxObs.m_GreenShrub);

				if (! twxObs.IsFieldNull(&twxObs.m_MC1))
					wxObs.m_MC1 = twxObs.m_MC1;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_MC1))
					wxObs.SetFieldNull(&wxObs.m_MC1);

				if (! twxObs.IsFieldNull(&twxObs.m_MC10))
				    wxObs.m_MC10 = twxObs.m_MC10;

				if(newRec && twxObs.IsFieldNull(&twxObs.m_MC10))
					wxObs.SetFieldNull(&wxObs.m_MC10);
				wxObs.m_MC100 = twxObs.m_MC100;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_MC100))
					wxObs.SetFieldNull(&wxObs.m_MC100);

				if (! twxObs.IsFieldNull(&twxObs.m_MC1000))
					wxObs.m_MC1000 = twxObs.m_MC1000;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_MC1000))
					wxObs.SetFieldNull(&wxObs.m_MC1000);

				if (! twxObs.IsFieldNull(&twxObs.m_X1000))
					wxObs.m_X1000 = twxObs.m_X1000;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_X1000))
					wxObs.SetFieldNull(&wxObs.m_X1000);

				if (! twxObs.IsFieldNull(&twxObs.m_MCHerb))
					wxObs.m_MCHerb = twxObs.m_MCHerb;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_MCHerb))
					wxObs.SetFieldNull(&wxObs.m_MCHerb);

				if (! twxObs.IsFieldNull(&twxObs.m_MCWood))
					wxObs.m_MCWood = twxObs.m_MCWood;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_MCWood))
					wxObs.SetFieldNull(&wxObs.m_MCWood);

				if (! twxObs.IsFieldNull(&twxObs.m_GREN))
					wxObs.m_GREN = twxObs.m_GREN;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_GREN))
					wxObs.SetFieldNull(&wxObs.m_GREN);

				if (! twxObs.IsFieldNull(&twxObs.m_HStage))
					wxObs.m_HStage = twxObs.m_HStage;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_HStage))
					wxObs.SetFieldNull(&wxObs.m_HStage);

				if (! twxObs.IsFieldNull(&twxObs.m_ROS))
					wxObs.m_ROS = twxObs.m_ROS;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_ROS))
					wxObs.SetFieldNull(&wxObs.m_ROS);

				if (! twxObs.IsFieldNull(&twxObs.m_ERC))
					wxObs.m_ERC = twxObs.m_ERC;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_ERC))
					wxObs.SetFieldNull(&wxObs.m_ERC);

				if (! twxObs.IsFieldNull(&twxObs.m_BI))
					wxObs.m_BI = twxObs.m_BI;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_BI))
					wxObs.SetFieldNull(&wxObs.m_BI);

				if (! twxObs.IsFieldNull(&twxObs.m_FIL))
					wxObs.m_FIL = twxObs.m_FIL;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_FIL))
					wxObs.SetFieldNull(&wxObs.m_FIL);

				

				// new FFP4 fields here     5/2006
				if (! twxObs.IsFieldNull(&twxObs.m_SolarRadiation))
                    wxObs.m_SolarRadiation = twxObs.m_SolarRadiation;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_SolarRadiation))
					wxObs.SetFieldNull(&wxObs.m_SolarRadiation);

				if (! twxObs.IsFieldNull(&twxObs.m_WetFlag))
					wxObs.m_WetFlag = twxObs.m_WetFlag;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_WetFlag))
					wxObs.SetFieldNull(&wxObs.m_WetFlag);

				if (! twxObs.IsFieldNull(&twxObs.m_GustDir))
					 wxObs.m_GustDir = twxObs.m_GustDir;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_GustDir))
					wxObs.SetFieldNull(&wxObs.m_GustDir);

				if (! twxObs.IsFieldNull(&twxObs.m_GustSpd))
					 wxObs.m_GustSpd = twxObs.m_GustSpd;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_GustSpd))
					wxObs.SetFieldNull(&wxObs.m_GustSpd);

				if (! twxObs.IsFieldNull(&twxObs.m_HourlyPrecip))
					 wxObs.m_HourlyPrecip = twxObs.m_HourlyPrecip;
				if(newRec && twxObs.IsFieldNull(&twxObs.m_HourlyPrecip))
					wxObs.SetFieldNull(&wxObs.m_HourlyPrecip);

				if (!twxObs.IsFieldNull(&twxObs.m_SnowFlag))
					wxObs.m_SnowFlag = twxObs.m_SnowFlag;
				if (newRec && twxObs.IsFieldNull(&twxObs.m_SnowFlag))
					wxObs.SetFieldNull(&wxObs.m_SnowFlag);
				/* 09/2012   set DailyObs field appropriately */
				wxObs.m_DailyObs = 0;
				if (wxObs.m_ObsType.CompareNoCase("O") == 0)
					wxObs.m_DailyObs = 1;
				else
				if ((wxObs.m_ObsType.CompareNoCase("R") == 0)
						&& (wxObs.m_Hour == regHour)
						&& (! wxObs.IsFieldNull(&wxObs.m_SOW)))
					wxObs.m_DailyObs = 1;

				wxObs.Update();
				LogWxDataWarnings(errLog, &twxObs);
				twxObs.MoveNext();
			}
			//if(!cd || !cd->Abort())
				twxObs.m_pDatabase->CommitTrans(); //so we can requery
			//else
			//	twxObs.m_pDatabase->Rollback();
		}
/*	}
	else
		twxObs.m_pDatabase->Rollback();*/
	//to maintain referential integrity,
	int staAdded = 0;
	//if(!cd || !cd->Abort())
	//{
		CSIGStationSet stnSet(pDB);
		stnSet.Open();
		pos = staList.GetHeadPosition();
		while(pos)
		{//check each station
			CString strSQL, sta;
			sta = staList.GetNext(pos);
			stnSet.m_strFilter.Format("[StationID] = '%6.6s'", sta);
			stnSet.Requery();
			if(stnSet.IsEOF())
			{
				COleDateTime lookG(1999, 4, 15, 14, 0, 0);
				COleDateTime lookF(1999, 10, 15, 14, 0, 0);
				stnSet.AddNew();
				stnSet.m_StationID = _T(sta);
				stnSet.m_Name = "                    ";
				stnSet.m_NFDRSFM = " ";
				stnSet.m_Use88 = 0;
				stnSet.m_LatDegrees = 0;
				stnSet.m_Elevation = 0;
				stnSet.m_ClimateCls = 0;
				stnSet.m_SlopeCls = 0;
				stnSet.m_HerbAnnual = 0;
				stnSet.m_GreenJulian = lookG;
				stnSet.m_FreezeJulian = lookF;
				stnSet.m_StartKBDI = 0;
				stnSet.m_Deciduous = 0;
				stnSet.m_AvgPrecip = 0.0;
				stnSet.m_Start1000 = 0.0;
				stnSet.m_FM1Eq10 = 0;
				stnSet.m_Aspect = " ";
				stnSet.m_PsnOnSlope = " ";
				stnSet.Update();
				fprintf(errLog, "\tAdded Station: %s\n", sta);
				staAdded++;
			}
		}
		stnSet.Close();
	//}
	EnableWindow(TRUE);
	//if(!cd || !cd->Abort())
	//{
			fprintf(errLog, "Processed %ld weather records for %d stations:\n"

				"\tAppended %ld records.\n"
				"\tUpdated %ld existing records.\n"
				"\tRejected %ld records for bad data.\n"
				"\tAdded %d new stations.\n",
				count, staList.GetCount(), appends, edits,  rejects, staAdded);

			errors = rejects;
			if (wdir360)
				fprintf(errLog,"Wind direction values were imported as degrees (0-360).\n");
			else
				fprintf(errLog,"Wind direction values were imported as points (1-8).\n");
	/*}
	else
	{
		errors = -1;
	}*/

	twxObs.Close();
	staList.RemoveAll();

	try
	{
		pDoc->m_pDB->ExecuteSQL("DROP TABLE [ffpTempWx]");
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
//	pDoc->m_pDB->DeleteTableDef("ffpTempWx");

	//save dialog layout to database
	CGenericWxImportSet gfiSet(pDoc->m_pDB);
	gfiSet.Open();
	while(!gfiSet.IsEOF())
	{
		gfiSet.Delete();
		gfiSet.MoveNext();
	}
	gfiSet.AddNew();
	if(space.GetCheck())
		gfiSet.m_Delimiter = 0;
	else if(comma.GetCheck())
		gfiSet.m_Delimiter = 1;
	else//semicolon
		gfiSet.m_Delimiter = 2;
	gfiSet.m_DateFormat = dt;
	gfiSet.m_TimeFormat = tt;
	/*gfiSet.m_UseStation = m_useStation.GetCheck();
	gfiSet.m_UseUnit = m_useUnit.GetCheck();
	gfiSet.m_UseSubunit = m_useSubunit.GetCheck();
	gfiSet.m_DefaultRegionID = defaultRegion;*/
	gfiSet.m_DupHandling = m_RejectDups;
	/*if(gfiSet.m_UseStat)
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
		gfiSet.m_DefaultSubunitID = -1;*/
	gfiSet.Update();
	gfiSet.Close();
	//now add field selections
	CGenericWxFieldsSet gffSet(pDoc->m_pDB);
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
		gffSet.m_FieldNum = field;
		gffSet.m_Rank = i;
		gffSet.Update();
	}
	gffSet.Close();
	//if (cd)
	//	cd->DestroyWindow();

	CDialog::OnOK();

	EnableWindow(TRUE);
}

void CGenericWxImportDialog::LogWxDataWarnings(FILE *logFile, CWxSet *wxSet)
{
	//precip
	if(wxSet->m_PPTAMT > 2.00)
	{
		fprintf(logFile, "Warning: StationID %s Date: %s Precip Amount = %.2f inches (> 2.00 inches)\n",
			wxSet->m_StationID, wxSet->m_ObsDate.Format("%m/%d/%Y"), wxSet->m_PPTAMT);
	}
}

// check for null values, especially from WRCC files
int CGenericWxImportDialog::IsNullValue(char *value){
       if (! value)
		   return TRUE;
	   if (strlen(value) < 1)
		   return TRUE;

	   if (!strcmp(value,"-9999"))
		   return TRUE;
	   if (!strcmp(value,"-9999."))
		   return TRUE;
	   if (!strcmp(value,"-999"))
		   return TRUE;
	   if (!strcmp(value,"-99"))
		   return TRUE;

	   if (!strcmp(value,"-"))
		   return TRUE;
	   if (!strcmp(value,"**"))
		   return TRUE;
	   if (!strcmp(value," "))
		   return TRUE;

	   return FALSE;
}
void CGenericWxImportDialog::OnBnClickedUseStationid()
{
	m_comboStation.EnableWindow(m_checkUseStation.GetCheck());
}

void CGenericWxImportDialog::OnBnClickedUseObsType()
{
	m_comboObsType.EnableWindow(m_checkObsType.GetCheck());
}

void CGenericWxImportDialog::OnBnClickedUseObsTime()
{
	m_dtObsTime.EnableWindow(m_checkObsTime.GetCheck());
}

void CGenericWxImportDialog::OnCbnSelchangeStations()
{
	// TODO: Add your control notification handler code here
}

void CGenericWxImportDialog::OnBnClickedCheckAllstations()
{
	CSIGStationSet staSet(pDB);
	staSet.m_strSort = "[StationID]";
	staSet.Open();
	m_comboStation.ResetContent();
	if(m_checkAllStations.GetCheck() == TRUE)
	{
		while(!staSet.IsEOF())
		{
			CString str;
			str.Format("%6.6s - %s", staSet.m_StationID, staSet.m_Name);
			m_comboStation.AddString(str);
			staSet.MoveNext();
		}
	}
	else
	{
		CWxStationSet stations(pDB);
		//stations.m_strSort = "[StationID]";
		stations.Open(CRecordset::dynaset, "SELECT DISTINCT WXObs.StationID FROM WXObs   ORDER BY WXObs.StationID;");
		while(!stations.IsEOF())
		{
			staSet.m_strFilter.Format("[StationID] = '%s'", stations.m_StationID);
			staSet.Requery();
			CString str;
			str.Format("%6.6s - %s", staSet.m_StationID, staSet.m_Name);
			m_comboStation.AddString(str);
			stations.MoveNext();
		}
	}
}
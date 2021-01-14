// FireShapeFileExportDialog.cpp : implementation file
//
#include "stdafx.h"
#include "fireplus.h"
#include "ReportOptionsSet.h"
#include "FireAssocSet.h"
#include "FireAgencySet.h"
#include "FireRegionSet.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"

#include "FireplusSet.h"
#include "FireShapeFileExportDialog.h"
#include "shapefil.h"
#include "utils.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;


#define defaultshapeFields 15
#define shapeFields  23

char *shapefileColNames[shapeFields] = 
{
	"Region/Unit/Subunit (NNNNNN)",
	"Year",
	"Fire Number",
	"Cause (USFS)",
	"Size Class",
	"Total Acres",
	"Latitude (DD.DDDD)",
	"Longitude (DD.DDDD)",
	"Township",
	"Range",
	"Section",
	"Subsection",
	"Discovery Date",
	"Fire ID",
	"Fire Name",

	// following are not selected by default
	"County",
	"State",
	"Discovery Time",
	"Agency Name",

	"Region",
	"Unit",
	"Subunit",
	"Cause (DOI)"
};

const char prjStr[] = "GEOGCS[\"GCS_North_American_1983\",DATUM[\"D_North_American_1983\",SPHEROID[\"GRS_1980\",6378137.0,298.257222101]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]]";

/////////////////////////////////////////////////////////////////////////////
// CFireShapeFileExportDialog dialog

CFireShapeFileExportDialog::CFireShapeFileExportDialog(CWnd* pParent /*=NULL*/, CFireEditSet *_records /*=NULL*/, CFireplusSet *_fpSet )
	: CDialog(CFireShapeFileExportDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFireShapeFileExportDialog)
	//}}AFX_DATA_INIT
	records = _records;
	fpSet = _fpSet;
}


void CFireShapeFileExportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFireShapeFileExportDialog)
	DDX_Control(pDX, IDC_TIME1, time1);
	DDX_Control(pDX, IDC_SPACE, space);
	DDX_Control(pDX, IDC_COMMA, comma);
	DDX_Control(pDX, IDC_SELECTEDLIST, selectedList);
	DDX_Control(pDX, IDC_FIELDSLIST, fieldsList);
	DDX_Control(pDX, IDC_DATE1, date1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFireShapeFileExportDialog, CDialog)
	//{{AFX_MSG_MAP(CFireShapeFileExportDialog)
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
	ON_BN_CLICKED(IDC_SHAPEFILE, OnExportTextFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireShapeFileExportDialog message handlers

BOOL CFireShapeFileExportDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int loc;

	int f;
	for(f = 0; f < defaultshapeFields; f++)
	{
		// add some defaults to selected list 
		loc = selectedList.AddString(shapefileColNames[f]);
		selectedList.SetItemData(loc, f);
	}

 	for (;f < shapeFields;f++)
    {
		// other fields - not selected by default 
		loc = fieldsList.AddString(shapefileColNames[f]);
		fieldsList.SetItemData(loc, f);
	}
	
	upButton.AutoLoad(IDC_UP, this);
	downButton.AutoLoad(IDC_DOWN, this);
	topButton.AutoLoad(IDC_TOP, this);
	bottomButton.AutoLoad(IDC_BOTTOM, this);
	dt = Full;
	date1.SetCheck(TRUE);
	tt = FullTime;
	time1.SetCheck(TRUE);
	comma.SetCheck(TRUE);
	
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFireShapeFileExportDialog::OnDate1() 
{
	dt = Full;
}

void CFireShapeFileExportDialog::OnDate2() 
{
	dt = Packed;
}

void CFireShapeFileExportDialog::OnDate3() 
{
	dt = Reverse;
}

void CFireShapeFileExportDialog::OnDate4() 
{
	dt = MD;
}

void CFireShapeFileExportDialog::OnDate5() 
{
	dt = MDPacked;
}

void CFireShapeFileExportDialog::OnTime1() 
{
	tt = FullTime;
}

void CFireShapeFileExportDialog::OnTime2() 
{
	tt = PackedTime;
}

void CFireShapeFileExportDialog::OnTop() 
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

void CFireShapeFileExportDialog::OnUp() 
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

void CFireShapeFileExportDialog::OnBottom() 
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

void CFireShapeFileExportDialog::OnDown() 
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

void CFireShapeFileExportDialog::OnDblclkFieldslist() 
{
	CString str;
	int cur = fieldsList.GetCurSel();
	fieldsList.GetText(cur, str);
	int val = fieldsList.GetItemData(cur);
	int loc = selectedList.AddString(str);
	selectedList.SetItemData(loc, val);
	fieldsList.DeleteString(cur);
	
	

	
}

void CFireShapeFileExportDialog::OnDblclkSelectedlist() 
{
	CString str;
	int cur = selectedList.GetCurSel();
	selectedList.GetText(cur, str);
	int val = selectedList.GetItemData(cur);
	int loc = fieldsList.AddString(str);
	fieldsList.SetItemData(loc, val);
	selectedList.DeleteString(cur);
}

void CFireShapeFileExportDialog::OnMoveleft() 
{
	if(selectedList.GetCurSel() != LB_ERR)
		OnDblclkSelectedlist();
}

void CFireShapeFileExportDialog::OnMoveright() 
{
	if(fieldsList.GetCurSel() != LB_ERR)
		OnDblclkFieldslist();
}

void CFireShapeFileExportDialog::OnSelectall() 
{
	while(fieldsList.GetCount() > 0)
	{
		fieldsList.SetCurSel(0);
		OnDblclkFieldslist();
	}
}

void CFireShapeFileExportDialog::OnRemoveall() 
{
	while(selectedList.GetCount() > 0)
	{
		selectedList.SetCurSel(0);
		OnDblclkSelectedlist();
	}	
}

void CFireShapeFileExportDialog::OnOK() 
{
	if(selectedList.GetCount() <= 0)
	{
		AfxMessageBox("ERROR: At least one field must be selected\nfor custom exports.");
		return;
	}
	int status = ExportFires();
	if(status)
		CDialog::OnOK();
}

int CFireShapeFileExportDialog::ExportFires()
{
	CString buf, temp;
	long count = 0;
	CString delim = " ";//default to space
	CButton *button = (CButton *)GetDlgItem(IDC_COMMA);
	if(button->GetCheck() == TRUE)
		delim = ",";
	else
	{
		button = (CButton *)GetDlgItem(IDC_NONE);
		if(button->GetCheck() == TRUE)
			delim = "";
	}
	CString msg;
	chdir(theApp.userDir);
	CFileDialog fd(FALSE, "shp", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Shape Files (*.shp)|*.shp|All Files (*.*)|*.*||", this);
	if(fd.DoModal() != IDOK)
		return 0;

	CWaitCursor wait;
	outFileName = fd.GetPathName();
	

    CString tName;
	tName.Format("%s",outFileName);

    // create .shp and .dbf files
    tName.MakeLower();
    int loc = tName.Find(".shp");
    tName.SetAt(loc, 0);

    char dbfName[MAX_PATH];
	char prjName[MAX_PATH];
    sprintf(dbfName, "%s.dbf", tName);
    sprintf(prjName, "%s.prj", tName);

    SHPHandle shp = SHPCreate( tName, SHPT_POINT);

    DBFHandle dbf = DBFCreate( dbfName );
	utils util;
    int rec = 0;

    if(!dbf){
		CString errMsg;
		errMsg.Format("ERROR: Could not create database file %s", dbfName);
        AfxMessageBox(errMsg);
		return 0;
	}

	if(!shp){
		AfxMessageBox("ERROR: Could not create shape file - problem with shapelib");
		return 0;
	}
         
    // required fields are:
	//    LAT/LONG to create a point
	//    fireID   to identify

    // entity is produced by adding to shape file
	int entField = DBFAddField(dbf,"ENTITY", FTDouble, 20, 0);
    int latField = DBFAddField(dbf, "LATITUDE", FTDouble, 20, 4);
	int lonField = DBFAddField(dbf, "LONGITUDE", FTDouble, 20, 4);
	int fireIDField = DBFAddField(dbf, "FIREID", FTInteger, 20, 0);
 



	// optional fields
	int regField, 
		yearField,
		firenumberField,
		causeField,
		sizeclassField,
		totalacresField,
		townshipField,
		rangeField,
		sectionField,
		subsectionField,
		discoverydateField,
		firenameField,
		countyField,
		stateField,
		discoverytimeField,
		agencynameField,
		regionField,
		unitField,
		subunitField,
		generalcauseField;

    int lastRegionID = 0;
	CString lastRegion;

    // get agency names from separate table
	CFireAgencySet agencySet(records->m_pDatabase);
	agencySet.m_strSort = _T("[AgencyID]");
	agencySet.Open();
	int iAgencies = agencySet.GetRecordCount();
	// arbitrary # here:
    CString agencies[20];
	while (!agencySet.IsEOF()){
         agencies[agencySet.m_AgencyID - 1].Format("%s",agencySet.m_Name);
		 agencySet.MoveNext();
	}
	agencySet.Close();

    for(int f = 0; f < selectedList.GetCount(); f++)
		{
			
			int loc = selectedList.GetItemData(f);

			switch(loc)
			{
			case 0:
				 regField = DBFAddField(dbf,"REGUNITSUB",FTString,6,0); break;
			case 1:
				 yearField = DBFAddField(dbf,"YEAR",FTString,4,0); break;
            case 2:
				 firenumberField = DBFAddField(dbf,"FIRENUMBER",FTString,12,0); break;
            case 3:
				 causeField = DBFAddField(dbf,"CAUSE_USFS",FTInteger,1,0); break;
			case 4:
				 sizeclassField = DBFAddField(dbf,"SIZECLASS",FTString,1,0); break;
            case 5:
				 totalacresField = DBFAddField(dbf,"TOTALACRES",FTDouble,10,1); break;
            case 6: // lat/longitude already added by default
			case 7: break;
            case 8:
				 townshipField = DBFAddField(dbf,"TOWNSHIP",FTString,5,0); break;
			case 9:
				 rangeField = DBFAddField(dbf,"RANGE",FTString,5,0); break;
			case 10:
				 sectionField = DBFAddField(dbf,"SECTION",FTInteger,2,0); break;
            case 11:
				 subsectionField = DBFAddField(dbf,"SUBSECTION",FTString,2,0); break;
			case 12:
                 discoverydateField = DBFAddField(dbf,"DISCOVERYDATE",FTString,12,0); break;
			case 13: break;// fireID added by default
			case 14:
				 firenameField = DBFAddField(dbf,"FIRENAME",FTString,25,0); break;
			case 15: 
				 countyField = DBFAddField(dbf,"COUNTY",FTInteger,20,0); break;
			case 16:
				 stateField = DBFAddField(dbf,"STATE",FTString,2,0); break;
			case 17:
				 discoverytimeField = DBFAddField(dbf,"DISCOVERYTIME",FTString,12,0); break;
			case 18:
				 agencynameField = DBFAddField(dbf,"AGENCYNAME",FTString,25,0); break;
			case 19:
				 regionField = DBFAddField(dbf,"REGION",FTString,40,0); break;
			case 20:
				 unitField = DBFAddField(dbf,"UNIT",FTString,40,0); break;
			case 21:
				 subunitField = DBFAddField(dbf,"SUBUNIT",FTString,40,0); break;
			case 22:
				 generalcauseField = DBFAddField(dbf,"CAUSE_DOI",FTInteger,1,0); break;
			
			default: break;
			}
		}

	// end optional field loop		
    DBFClose(dbf);

    dbf = DBFOpen(dbfName, "rb+");



   int skippedRecords = 0;


CRecordsetStatus recStatus;
	records->GetStatus(recStatus);
	long retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
	//CDBVariant varRecordToReturnTo;
	//records->GetBookmark( varRecordToReturnTo );
	records->MoveFirst();
	while(!records->IsEOF())
	{
		

		// skip records that don't have lat/long/ID
	    if(records->IsFieldNull(&records->m_LatDD) ||
			records->IsFieldNull(&records->m_LatMM) ||
			records->IsFieldNull(&records->m_LatSS) ||
		   records->IsFieldNull(&records->m_LonDD) ||
		    records->IsFieldNull(&records->m_LonMM) ||
			records->IsFieldNull(&records->m_LonSS) ||
			records->IsFieldNull(&records->m_FireID)){
		        records->MoveNext();
				skippedRecords++;
                continue;
			}

		if (records->m_LatDD > 90 || records->m_LatDD < 1 || records->m_LatMM > 60 || records->m_LatSS > 60){
			   records->MoveNext();
				skippedRecords++;
			   continue;
			}



        double padfX, padfY, padfZ;
		DDMMSS2DecimalDegree(records->m_LatDD, records->m_LatMM, records->m_LatSS, &padfY);
		DDMMSS2DecimalDegree(records->m_LonDD, records->m_LonMM, records->m_LonSS, &padfX);
        /*if (records->m_LatDD > 0)
    		padfY =  (records->m_LatDD) + (double) (records->m_LatMM)/60 + (double) (records->m_LatSS)/3600;
		else
    		padfY =  (records->m_LatDD) - (double) (records->m_LatMM)/60 -  (double) (records->m_LatSS)/3600;

		if (records->m_LonDD > 0)
		   padfX =  (records->m_LonDD) + (double) (records->m_LonMM)/60 + (double) (records->m_LonSS)/3600;
		else
		   padfX =  (records->m_LonDD) - (double) (records->m_LonMM)/60 - (double) (records->m_LonSS)/3600;*/
        padfZ = 0;

        SHPObject * pShape = SHPCreateSimpleObject( SHPT_POINT, 1, &padfX, &padfY,&padfZ);

        int entity = SHPWriteObject( shp, -1, pShape );

        DBFWriteDoubleAttribute(dbf, entity, entField, (double)entity);
        DBFWriteDoubleAttribute(dbf, entity, latField, padfY);
        DBFWriteDoubleAttribute(dbf, entity, lonField, padfX);
		DBFWriteIntegerAttribute(dbf, entity, fireIDField, records->m_FireID);

        SHPDestroyObject(pShape);

		for(int f = 0; f < selectedList.GetCount(); f++)
		{
			
			CString temp;
			int loc = selectedList.GetItemData(f);
	
			switch(loc)
			{
			case 0:  // region/unit/subunit
				
			    temp.Format("%02d%02d%02d",records->m_RegionID,records->m_UnitID,records->m_SubunitID);
			    DBFWriteStringAttribute(dbf,entity,regField,temp);

				break;
			case 1: // year
			    if(!records->IsFieldNull(&records->m_Year))
    				DBFWriteStringAttribute(dbf,entity,yearField,records->m_Year);
				else
                    DBFWriteStringAttribute(dbf,entity,yearField,"");
			    break;
			case 2: // fire number
				if(!records->IsFieldNull(&records->m_FireNumber))
					temp.Format("%12.12s", records->m_FireNumber);
				else
					temp = "            ";
				DBFWriteStringAttribute(dbf,entity,firenumberField,temp);
				break;
            case 3://cause
				
				if(!records->IsFieldNull(&records->m_StatisticalCause))
					DBFWriteIntegerAttribute(dbf,entity,causeField,util.TranslateCauseToUSFS(records->m_StatisticalCause,records->m_AgencyID));
				else
					DBFWriteIntegerAttribute(dbf,entity,causeField,0);
				break;
			case 4: // size class
				if(!records->IsFieldNull(&records->m_SizeClass))
    				temp.Format("%-1s",records->m_SizeClass);
				else
					temp = " ";
				DBFWriteStringAttribute(dbf,entity,sizeclassField,temp);
			    break;
			case 5: // total acres
                if(!records->IsFieldNull(&records->m_TotalAcres))
					DBFWriteDoubleAttribute(dbf,entity,totalacresField,records->m_TotalAcres);
				else
					DBFWriteDoubleAttribute(dbf,entity,totalacresField,0);
				break;
			case 6:  // Latitude DD already in
             
				break;
			case 7: // Longitude DD already in
                
				break;
			case 8: // township
                if(!records->IsFieldNull(&records->m_Township))
					temp.Format("%5s", records->m_Township);
				else
					temp = "     ";
				DBFWriteStringAttribute(dbf,entity,townshipField,temp);
				break;
			case 9: // Range
                if(!records->IsFieldNull(&records->m_Range))
					temp.Format("%5s", records->m_Range);
				else
					temp = "";
				DBFWriteStringAttribute(dbf,entity,rangeField,temp);
				break;
			case 10:  // section
				if(!records->IsFieldNull(&records->m_Section))
					DBFWriteIntegerAttribute(dbf,entity,sectionField,records->m_Section);
				else
					DBFWriteIntegerAttribute(dbf,entity,sectionField,0);
				break;
			case 11:  // subsection
				if(!records->IsFieldNull(&records->m_SubSection))
					temp.Format("%5s", records->m_SubSection);
				else
					temp = "";
				DBFWriteStringAttribute(dbf,entity,subsectionField,temp);
                break;
			case 12: // discovery
				if(!records->IsFieldNull(&records->m_Discovery))
				{
					switch(dt)
					{
					case Full:
						temp.Format("%02d/%02d/%4d", records->m_Discovery.GetMonth(),
							records->m_Discovery.GetDay(), records->m_Discovery.GetYear());
						break;
					case Packed:
						temp.Format("%02d%02d%4d", records->m_Discovery.GetMonth(),
							records->m_Discovery.GetDay(), records->m_Discovery.GetYear());
						break;
					case Reverse:
						temp.Format("%4d%02d%02d", records->m_Discovery.GetYear(), records->m_Discovery.GetMonth(),
							records->m_Discovery.GetDay());
						break;
					case MD:
						temp.Format("%02d/%02d", records->m_Discovery.GetMonth(),
							records->m_Discovery.GetDay());
						break;
					case MDPacked:
						temp.Format("%02d%02d", records->m_Discovery.GetMonth(),
							records->m_Discovery.GetDay());
						break;
					}
				}
				else
				{
					switch(dt)
					{
					case Full:
						temp = "          ";
						break;
					case Packed:
					case Reverse:
						temp = "        ";
						break;
					case MD:
						temp = "     ";
						break;
					case MDPacked:
						temp = "    ";
						break;
					}
				}
				DBFWriteStringAttribute(dbf,entity,discoverydateField,temp);
				break;
			case 13: // fire ID - already in
               break;
			case 14: // fire name
                if(!records->IsFieldNull(&records->m_FireName))
					temp.Format("%-25s", records->m_FireName.Trim());
				else
					temp = "";
				DBFWriteStringAttribute(dbf,entity,firenameField,temp);
				break;
			case 15: // county
                if(!records->IsFieldNull(&records->m_County))
					DBFWriteIntegerAttribute(dbf,entity,countyField,records->m_County);
				else
					DBFWriteIntegerAttribute(dbf,entity,countyField,0);
				break;
			case 16: // state
                if(!records->IsFieldNull(&records->m_State))
					temp.Format("%s", records->m_State);
				else
					temp = "";
				DBFWriteStringAttribute(dbf,entity,stateField,temp);
				break;

			case 17://time
				if(!records->IsFieldNull(&records->m_Discovery))
				{
					switch(tt)
					{
					case FullTime:
						temp.Format("%02d:%02d", records->m_Discovery.GetHour(),
							records->m_Discovery.GetMinute());
						break;
					case PackedTime:
						temp.Format("%02d%02d", records->m_Discovery.GetHour(),
							records->m_Discovery.GetMinute());
						break;
					}
				}
				else
				{
					switch(tt)
					{
					case FullTime:
						temp = "     ";
						break;
					case PackedTime:
						temp = "    ";
						break;
					}
				}
				DBFWriteStringAttribute(dbf,entity,discoverytimeField,temp);
				break;
		    case 18: // agency name
                if(!records->IsFieldNull(&records->m_AgencyID))
					temp.Format("%s", agencies[records->m_AgencyID - 1]);
				else
					temp = "";
				DBFWriteStringAttribute(dbf,entity,agencynameField,temp);
				break;
			case 19:  // region name
				
				if(!records->IsFieldNull(&records->m_RegionID)){
					if (records->m_RegionID == lastRegionID){
                        temp.Format("%s",lastRegion);
					} else {
			           CFireRegionSet regSet(records->m_pDatabase);
			           CString strFilter;
                       strFilter.Format("[RegionID] = %d AND [AgencyID] = %d",records->m_RegionID,records->m_AgencyID);
			           regSet.m_strFilter = strFilter;
                       regSet.Open();
			           CString regStr;
			           if (! regSet.IsEOF())
				          regStr.Format("%s",regSet.m_Name);
			           else
				          regStr = "";
			           regSet.Close();
			           temp.Format("%s",regStr);
					   lastRegion.Format("%s",regStr);
					   lastRegionID = records->m_RegionID;
					}
		         } else
			        temp = "";
				 DBFWriteStringAttribute(dbf,entity,regionField,temp);
				 break;
 			case 20:  // unit name
				if(!records->IsFieldNull(&records->m_UnitID)){
			        CFireUnitSet unitSet(records->m_pDatabase);
			        CString strFilter;
                    strFilter.Format("[RegionID] = %d AND [UnitID] = %d",records->m_RegionID,records->m_UnitID);
			        unitSet.m_strFilter = strFilter;
                    unitSet.Open();
			        CString unitStr;
			        if (! unitSet.IsEOF())
				       unitStr.Format("%s",unitSet.m_Name);
			        else
				       unitStr = "";
			        unitSet.Close();
			        temp.Format("%s",unitStr);
		         } else
			        temp = "";
				 DBFWriteStringAttribute(dbf,entity,unitField,temp);
				 break;
 			case 21:  // subunit name
				if(!records->IsFieldNull(&records->m_SubunitID)){
			        CFireSubunitSet subunitSet(records->m_pDatabase);
			        CString strFilter;
                    strFilter.Format("[SubunitID] = %d AND [UnitID] = %d",records->m_SubunitID,records->m_UnitID);
			        subunitSet.m_strFilter = strFilter;
                    subunitSet.Open();
			        CString subunitStr;
			        if (! subunitSet.IsEOF())
				       subunitStr.Format("%s",subunitSet.m_Name);
			        else
				       subunitStr = "";
			        subunitSet.Close();
			        temp.Format("%s",subunitStr);
		         } else
			        temp = "";
                 DBFWriteStringAttribute(dbf,entity,subunitField,temp);
                 break;
		    case 22://doi cause
		
				if(!records->IsFieldNull(&records->m_StatisticalCause))
					DBFWriteIntegerAttribute(dbf,entity,generalcauseField,util.TranslateCauseToDOI(records->m_StatisticalCause,records->m_AgencyID));
				else
					DBFWriteIntegerAttribute(dbf,entity,generalcauseField,0);
				break;
			default:
				temp = "";
			}
			
		}
		
		
            


	
		count++;
		records->MoveNext();
	
	}




	records->SetAbsolutePosition(retRec + 1);
	//records->SetBookmark( varRecordToReturnTo );


    // close files
    SHPClose(shp);
    DBFClose(dbf);
//now add the prj file
	int prjSuccess = 0;
	FILE *prj = fopen(prjName, "wt");
	if(prj)
	{
		fprintf(prj,"%s", prjStr);
		fclose(prj);
		prjSuccess = 1;
	}
	chdir(theApp.dbDir);//.workDir);
	theApp.SetUserDir(fd.GetPathName());
	if (skippedRecords > 0)
		msg.Format("Shape File export complete!\n\n%ld records written to\n%s (+.shx)\nwith DBF file\n%s\nwith PRJ file\n%s\nWARNING: %d records were skipped (missing LAT / LON)\n", count, outFileName,dbfName, prjName,skippedRecords);
	else
	   msg.Format("Shape File export complete!\n\n%ld records written to\n%s (+.shx)\nwith DBF file\n%s\nwith PRJ file\n%s ", count, outFileName,dbfName, prjName);
	AfxMessageBox(msg);
	return 1;

}


void CFireShapeFileExportDialog::OnExportTextFile(){ 

	utils util;

	CString buf, temp;
	long count = 0;
	
	CString delim = " ";//default to space
	CButton *button = (CButton *)GetDlgItem(IDC_COMMA);
	if(button->GetCheck() == TRUE)
		delim = ",";
	else
	{
		button = (CButton *)GetDlgItem(IDC_NONE);
		if(button->GetCheck() == TRUE)
			delim = "";
	}
	CString msg;
	chdir(theApp.userDir);
		CFileDialog fd(FALSE, "txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", this);
	if(fd.DoModal() != IDOK)
		return;

	CWaitCursor wait;

	outFileName = fd.GetPathName();
	FILE *out = fopen(outFileName, "wt");
	if(!out)
	{
		msg.Format("ERROR: Unable to create output file:\n%s", outFileName);
		AfxMessageBox(msg);
		return;
	}
	CRecordsetStatus recStatus;
	records->GetStatus(recStatus);
	long retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
	//CDBVariant varRecordToReturnTo;
	//records->GetBookmark( varRecordToReturnTo );

 // get agency names from separate table
	CFireAgencySet agencySet(records->m_pDatabase);
	agencySet.m_strSort = _T("[AgencyID]");
	agencySet.Open();
	int iAgencies = agencySet.GetRecordCount();
	// arbitrary # here:
    CString agencies[20];
	while (!agencySet.IsEOF()){
         agencies[agencySet.m_AgencyID - 1].Format("%s",agencySet.m_Name);
		 agencySet.MoveNext();
	}
	agencySet.Close();

    int lastRegionID = 0;
	CString lastRegion;

	//now build list of associations...
    CFireAssocSet associations(records->m_pDatabase);
	char tStr[64];
	if(fpSet)
		strcpy(tStr, fpSet->m_SIG_Station);
	else
		strcpy(tStr, "######");
	associations.m_strFilter.Format("[SIG/Station] = '%s'", strlen(tStr) > 6 ? &tStr[6] : tStr);
	associations.Open();
	if(associations.IsBOF() || associations.IsEOF())
	{
		AfxMessageBox("ERROR:No associations specified.");
		return;
	}

	CString assocStr;
    CFireAgencySet agency(records->m_pDatabase);
	agency.Open();
	CFireUnitSet units(records->m_pDatabase);
	units.Open();
	CFireSubunitSet subUnits(records->m_pDatabase);
    subUnits.Open();
	CFireRegionSet regions(records->m_pDatabase);
	regions.Open();

	assocStr.Format("Fire Associations used:\n\n");
	while(!associations.IsEOF())
	{
		agency.m_strFilter.Format("[AgencyID] = %ld", associations.m_AgencyID);
		agency.Requery();
		regions.m_strFilter.Format("[RegionID] = %ld", associations.m_RegionID);
		regions.Requery();
		temp.Format("\t%s, %s", agency.m_Name, regions.m_Name);
		assocStr += temp;
		if(!associations.IsFieldNull(&associations.m_UnitID))
		{
			units.m_strFilter.Format("[UnitID] = %ld", associations.m_UnitID);
			units.Requery();
			temp.Format(", %s", units.m_Name);
			assocStr += temp;
			if(!associations.IsFieldNull(&associations.m_SubunitID))
			{
				subUnits.m_strFilter.Format("[SubunitID] = %ld", associations.m_SubunitID);
				subUnits.Requery();
				temp.Format(", %s", subUnits.m_Name);
				assocStr += temp;
			}
		}
		assocStr += "\n";
		associations.MoveNext();
	}
    associations.Close();
	agency.Close();
	regions.Close();
	units.Close();
	subUnits.Close();

	//output header
		
	CReportOptionsSet reportSet(records->m_pDatabase);
	reportSet.Open();

	CString dStr;

	fprintf(out, "FireFamily Plus Fires Textfile Export (for shapefile)\n");
	
	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(out,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(out,"   using database: %s\n", records->m_pDatabase->GetDatabaseName());

	fprintf(out,"\n");
	CString headerStr;

	// add report headers (as needed)
	if(fpSet)
		reportSet.listWorkingSet(&headerStr,fpSet);
	else
		headerStr = "\nGeneral Fire Associations\n";
	fprintf(out,"%s",headerStr);
	reportSet.Close();

    fprintf(out,"%s\n",assocStr);

	// end added for batch

	 for(int f = 0; f < selectedList.GetCount(); f++)
		{
			if (f > 0)
  			   fprintf(out,"%s",delim);
			int loc = selectedList.GetItemData(f);

			switch(loc)
			{
			case 0:
				 fprintf(out,"RegionUnitSubunit");
				 break;
			case 1:
				 fprintf(out,"Year"); break;
            case 2:
				 fprintf(out,"Fire#"); break;
            case 3:
				 fprintf(out,"Cause(USFS)"); break;
			case 4:
				 fprintf(out,"Size"); break;
            case 5:
				 fprintf(out,"TotalAcres"); break;
            case 6: fprintf(out,"Latitude");break;
			case 7: fprintf(out,"Longitude");break;
            case 8:
				 fprintf(out,"Township"); break;
			case 9:
				 fprintf(out,"Range"); break;
			case 10:
				 fprintf(out,"Section"); break;
            case 11:
				 fprintf(out,"Subsection"); break;
			case 12:
                 fprintf(out,"DiscoveryDate"); break;
			case 13: fprintf(out,"FireID");break;
			case 14:
				 fprintf(out,"FireName"); break;
			case 15: 
				 fprintf(out,"County"); break;
			case 16:
				 fprintf(out,"State"); break;
			case 17:
				 fprintf(out,"DiscoveryTime"); break;
			case 18:
				 fprintf(out,"AgencyName"); break;
			case 19:
				 fprintf(out,"Region"); break;
			case 20:
				 fprintf(out,"Unit"); break;
			case 21:
				 fprintf(out,"Subunit"); break;
			case 22:
				 fprintf(out,"Cause(DOI)"); break;
			default: break;
			}
		}

    fprintf(out,"\n");

	records->MoveFirst();
	while(!records->IsEOF())
	{
		for(int f = 0; f < selectedList.GetCount(); f++)
		{
			if(f > 0)
				buf += delim;
			CString temp, fmt;
			int loc = selectedList.GetItemData(f);
	//		fmt.Format("%s", outFormats[loc]);
			switch(loc)
			{
			case 0:  // region/unit/subunit
				
			    temp.Format("%02d%02d%02d",records->m_RegionID,records->m_UnitID,records->m_SubunitID);
				break;
			case 1: // year
				if(!records->IsFieldNull(&records->m_Year))
				   temp.Format("%s",records->m_Year);
				else
				   temp = "";
			    break;
			case 2: // fire number
				if(!records->IsFieldNull(&records->m_FireNumber))
					temp.Format("%s", records->m_FireNumber);
				else
					temp = "";
				break;
            case 3://cause
		
				if(!records->IsFieldNull(&records->m_StatisticalCause))
					temp.Format("%ld", util.TranslateCauseToUSFS(records->m_StatisticalCause,records->m_AgencyID));
				else
					temp = "";
				break;
			case 4: // size class
				if(!records->IsFieldNull(&records->m_SizeClass))
    				temp.Format("%-1s",records->m_SizeClass);
				else
					temp = "";
			    break;
			case 5: // total acres
                if(!records->IsFieldNull(&records->m_TotalAcres))
					temp.Format("%10.1f", records->m_TotalAcres);
				else
					temp = "0.0";
				break;
			case 6:  // Latitude DD
				if(!records->IsFieldNull(&records->m_LatDD) && !records->IsFieldNull(&records->m_LatMM)
					&& !records->IsFieldNull(&records->m_LatSS))
				{
					double lat;
					DDMMSS2DecimalDegree(records->m_LatDD, records->m_LatMM, records->m_LatSS, &lat);
					temp.Format("%8.4f", lat);
			      /*if (records->m_LatDD > 0)
        			  temp.Format("%8.4f", (double) (records->m_LatDD) + (double) (records->m_LatMM)/60 + (double) (records->m_LatSS)/3600);
				  else
        			  temp.Format("%8.4f", (double) (records->m_LatDD) - (double) (records->m_LatMM)/60 - (double) (records->m_LatSS)/3600);*/
				} 
				else
				  temp.Format("NA      ");
				break;
			case 7: // Longitude DD
				if(!records->IsFieldNull(&records->m_LonDD) && !records->IsFieldNull(&records->m_LonMM)
					&& !records->IsFieldNull(&records->m_LonSS))
				{
					double lon;
					DDMMSS2DecimalDegree(records->m_LonDD, records->m_LonMM, records->m_LonSS, &lon);
					temp.Format("%8.4f", lon);
				} 
				else
				  temp.Format("NA      ");
				/*if(!records->IsFieldNull(&records->m_LonDD)){
				   if (records->m_LonDD < 0)
                      temp.Format("%8.4f", (double) (records->m_LonDD) - (double) (records->m_LonMM)/60 - (double) (records->m_LonSS)/3600);
				   else
                      temp.Format("%8.4f", (double) (records->m_LonDD) + (double) (records->m_LonMM)/60 + (double) (records->m_LonSS)/3600);
				} else
			       temp.Format("0.0");*/
				break;
			case 8: // township
                if(!records->IsFieldNull(&records->m_Township))
					temp.Format("%s", records->m_Township);
				else
					temp = "";
				break;
			case 9: // Range
                if(!records->IsFieldNull(&records->m_Range))
					temp.Format("%s", records->m_Range);
				else
					temp = "";
				break;
			case 10:  // section
				if(!records->IsFieldNull(&records->m_Section))
					temp.Format("%2d", records->m_Section);
				else
					temp = "";
				break;
			case 11:  // subsection
				if(!records->IsFieldNull(&records->m_SubSection))
					temp.Format("%s", records->m_SubSection);
				else
					temp = "";
				break;

			case 12: // discovery
				if(!records->IsFieldNull(&records->m_Discovery))
				{
					switch(dt)
					{
					case Full:
						temp.Format("%02d/%02d/%4d", records->m_Discovery.GetMonth(),
							records->m_Discovery.GetDay(), records->m_Discovery.GetYear());
						break;
					case Packed:
						temp.Format("%02d%02d%4d", records->m_Discovery.GetMonth(),
							records->m_Discovery.GetDay(), records->m_Discovery.GetYear());
						break;
					case Reverse:
						temp.Format("%4d%02d%02d", records->m_Discovery.GetYear(), records->m_Discovery.GetMonth(),
							records->m_Discovery.GetDay());
						break;
					case MD:
						temp.Format("%02d/%02d", records->m_Discovery.GetMonth(),
							records->m_Discovery.GetDay());
						break;
					case MDPacked:
						temp.Format("%02d%02d", records->m_Discovery.GetMonth(),
							records->m_Discovery.GetDay());
						break;
					}
				}
				else
					temp = "";

				break;
			case 13: // fire ID
                if(!records->IsFieldNull(&records->m_FireID))
					temp.Format("%6d", records->m_FireID);
				else
					temp = "";
				break;
            case 14: // fire name
                if(!records->IsFieldNull(&records->m_FireName))
					temp.Format("%s", records->m_FireName.Trim());
				else
					temp = "";
				break;
			case 15: // county
                if(!records->IsFieldNull(&records->m_County))
					temp.Format("%d", records->m_County);
				else
					temp = "";
				break;
			case 16: // state
                if(!records->IsFieldNull(&records->m_State))
					temp.Format("%s", records->m_State);
				else
					temp = "";
				break;

			case 17://time
				if(!records->IsFieldNull(&records->m_Discovery))
				{
					switch(tt)
					{
					case FullTime:
						temp.Format("%02d:%02d", records->m_Discovery.GetHour(),
							records->m_Discovery.GetMinute());
						break;
					case PackedTime:
						temp.Format("%02d%02d", records->m_Discovery.GetHour(),
							records->m_Discovery.GetMinute());
						break;
					}
				}
				else
				   temp = "";
				break;
			case 18: // agency name
                if(!records->IsFieldNull(&records->m_AgencyID))
					temp.Format("%s", agencies[records->m_AgencyID-1]);
				else
					temp = "";
				
				break;
			case 19:  // region name
				if(!records->IsFieldNull(&records->m_RegionID)){
					if (records->m_RegionID == lastRegionID)
						temp.Format("%s",lastRegion);
					else {
			           CFireRegionSet regSet(records->m_pDatabase);
			           CString strFilter;
                       strFilter.Format("[RegionID] = %d AND [AgencyID] = %d",records->m_RegionID,records->m_AgencyID);
			           regSet.m_strFilter = strFilter;
                       regSet.Open();
			           CString regStr;
			           if (! regSet.IsEOF())
				          regStr.Format("%s",regSet.m_Name);
			           else
				          regStr = "";
			           regSet.Close();
			           temp.Format("%s",regStr);
					   lastRegionID = records->m_RegionID;
					   lastRegion.Format("%s",regStr);
					}
		         } else
			        temp = "";
				 break;
 			case 20:  // unit name
				if(!records->IsFieldNull(&records->m_UnitID)){
			        CFireUnitSet unitSet(records->m_pDatabase);
			        CString strFilter;
                    strFilter.Format("[RegionID] = %d AND [UnitID] = %d",records->m_RegionID,records->m_UnitID);
			        unitSet.m_strFilter = strFilter;
                    unitSet.Open();
			        CString unitStr;
			        if (! unitSet.IsEOF())
				       unitStr.Format("%s",unitSet.m_Name);
			        else
				       unitStr = "";
			        unitSet.Close();
			        temp.Format("%s",unitStr);
		         } else
			        temp = "";
			      break;
 			case 21:  // subunit name
				if(!records->IsFieldNull(&records->m_SubunitID)){
			        CFireSubunitSet subunitSet(records->m_pDatabase);
			        CString strFilter;
                    strFilter.Format("[SubunitID] = %d AND [UnitID] = %d",records->m_SubunitID,records->m_UnitID);
			        subunitSet.m_strFilter = strFilter;
                    subunitSet.Open();
			        CString subunitStr;
			        if (! subunitSet.IsEOF())
				       subunitStr.Format("%s",subunitSet.m_Name);
			        else
				       subunitStr = "";
			        subunitSet.Close();
			        temp.Format("%s",subunitStr);
		         } else
			        temp = "";
                  break;
		    case 22://DOI cause
			
				if(!records->IsFieldNull(&records->m_StatisticalCause))
					temp.Format("%ld", util.TranslateCauseToDOI(records->m_StatisticalCause, records->m_AgencyID));
				else
					temp = "";
				break;
			default:
				temp = "";
			}
			buf += temp;
		}
		
		
            


		buf.Append("\n");


        fprintf(out,"%s",buf);
	
		count++;
		records->MoveNext();
		buf = "";
	}

	records->SetAbsolutePosition(retRec + 1);
	//records->SetBookmark( varRecordToReturnTo );

	fclose(out);
	chdir(theApp.dbDir);//theApp.workDir);
	theApp.SetUserDir(fd.GetPathName());
	msg.Format("Textfile data export Complete!\n%ld records written to\n%s... ", count, outFileName);
	AfxMessageBox(msg);
	return;

}



// StationExportDialog.cpp : implementation file
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
#include "StationExportDialog.h"
#include "SIGStationSet.h"

#include "FireAgencySet.h"
#include "FireRegionSet.h"
#include "CountySet.h"


#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern int curr_runID;


#define defaultFields 33
#define Fields  33

char *stationColNames[Fields] = 
{
	"Station ID",
	"Name",
	
	"Latitude",
	"Longitude",
	"Elevation",
	"Station Type",
	"Site #",
	"State",
	"County",
    "Agency",
	"Region",
	"Unit",

	"NFDRS fuel model",
	"Use 88?",
	"Climate class",
	"Slope class",
	"Aspect",
	"Position on slope",

	"Herb annual?",
	"Greenup date",
	"Freeze date",
	"Herb moisture",
	"Shrub moisture",
	"Initial KBDI",
	"Deciduous?",
	"Avg. Precip",
	"Initial 1000h",
	"FM 1 = 10?",

	"UseDormant",
	"DormantJulian",
  	"WRCC_ID",
	"RegSchdObs",
	"NESDIS_ID"
	
	

};



/////////////////////////////////////////////////////////////////////////////
// CStationExportDialog dialog

CStationExportDialog::CStationExportDialog(CStationdialog* pParent /*=NULL*/, CFireplusSet *_fpSet, CString *_SIG )
	: CDialog(CStationExportDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStationExportDialog)
	//}}AFX_DATA_INIT
	
	fpSet = _fpSet;
	SIG = _SIG;
	stnDlg = pParent;
}

CStationExportDialog::CStationExportDialog(CSIGSDialog* pParent /*=NULL*/, CFireplusSet *_fpSet, CString *_SIG )
	: CDialog(CStationExportDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStationExportDialog)
	//}}AFX_DATA_INIT
	
	fpSet = _fpSet;
	SIG = _SIG;
	stnDlg = NULL;
}


void CStationExportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStationExportDialog)
	DDX_Control(pDX, IDC_TIME1, time1);
	DDX_Control(pDX, IDC_SPACE, space);
	DDX_Control(pDX, IDC_COMMA, comma);
	DDX_Control(pDX, IDC_SELECTEDLIST, selectedList);
	DDX_Control(pDX, IDC_FIELDSLIST, fieldsList);
	DDX_Control(pDX, IDC_DATE1, date1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStationExportDialog, CDialog)
	//{{AFX_MSG_MAP(CStationExportDialog)
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
	ON_BN_CLICKED(IDC_EXPORT, OnOK)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationExportDialog message handlers

BOOL CStationExportDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	int loc;

	int f;
	for(f = 0; f < defaultFields; f++)
	{
		// add some defaults to selected list 
		loc = selectedList.AddString(stationColNames[f]);
		selectedList.SetItemData(loc, f);
	}

 	for (;f < Fields;f++)
    {
		// other fields - not selected by default 
		loc = fieldsList.AddString(stationColNames[f]);
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

void CStationExportDialog::OnDate1() 
{
	dt = Full;
}

void CStationExportDialog::OnDate2() 
{
	dt = Packed;
}

void CStationExportDialog::OnDate3() 
{
	dt = Reverse;
}

void CStationExportDialog::OnDate4() 
{
	dt = MD;
}

void CStationExportDialog::OnDate5() 
{
	dt = MDPacked;
}

void CStationExportDialog::OnTime1() 
{
	tt = FullTime;
}

void CStationExportDialog::OnTime2() 
{
	tt = PackedTime;
}

void CStationExportDialog::OnTop() 
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

void CStationExportDialog::OnUp() 
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

void CStationExportDialog::OnBottom() 
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

void CStationExportDialog::OnDown() 
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

void CStationExportDialog::OnDblclkFieldslist() 
{
	CString str;
	int cur = fieldsList.GetCurSel();
	fieldsList.GetText(cur, str);
	int val = fieldsList.GetItemData(cur);
	int loc = selectedList.AddString(str);
	selectedList.SetItemData(loc, val);
	fieldsList.DeleteString(cur);
	
	

	
}

void CStationExportDialog::OnDblclkSelectedlist() 
{
	CString str;
	int cur = selectedList.GetCurSel();
	selectedList.GetText(cur, str);
	int val = selectedList.GetItemData(cur);
	int loc = fieldsList.AddString(str);
	fieldsList.SetItemData(loc, val);
	selectedList.DeleteString(cur);
}

void CStationExportDialog::OnMoveleft() 
{
	if(selectedList.GetCurSel() != LB_ERR)
		OnDblclkSelectedlist();
}

void CStationExportDialog::OnMoveright() 
{
	if(fieldsList.GetCurSel() != LB_ERR)
		OnDblclkFieldslist();
}

void CStationExportDialog::OnSelectall() 
{
	while(fieldsList.GetCount() > 0)
	{
		fieldsList.SetCurSel(0);
		OnDblclkFieldslist();
	}
}

void CStationExportDialog::OnRemoveall() 
{
	while(selectedList.GetCount() > 0)
	{
		selectedList.SetCurSel(0);
		OnDblclkSelectedlist();
	}	
}

void CStationExportDialog::OnOK() 
{
	if(selectedList.GetCount() <= 0)
	{
		AfxMessageBox("ERROR: At least one field must be selected\nfor custom exports.");
		return;
	}
	
	


	CSIGStationSet records(fpSet->m_pDatabase);
	records.m_strSort = "[StationID]";
	
	

	if (SIG)
	{
		if (SIG->GetLength() > 1)
			records.m_strFilter.Format("[StationID] in (select StationID from ffpSIGCatalog where SIG='%s')",SIG->GetString()); 
	}
	else
	{
			records.m_strFilter.Format("[StationID] in (select DISTINCT StationID from WXObs)"); 

	}
    if (! records.IsOpen())
	   records.Open();

	// no station list
	if (records.IsEOF())
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: No Wx observations have been imported for any station.\n\n"));
		AfxMessageBox(strMsg);
		records.Close();
		return;

	}

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
	records.GetStatus(recStatus);
	long retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
	//CDBVariant varRecordToReturnTo;
	//records->GetBookmark( varRecordToReturnTo );

 

	//output header
	
	CString dStr;

	fprintf(out, "FireFamily Plus Station Metadata Export\n");
	
	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(out,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(out,"   using database: %s\n", records.m_pDatabase->GetDatabaseName());

	if (SIG)
    	if  (SIG->GetLength() > 1){
           CString headerStr;
		   headerStr.Format("   limited to: stations in SIG '%s'\n", SIG->GetString());
		   fprintf(out,"%s",headerStr);
	    }
	fprintf(out,"\n");
	

	


	 for(int f = 0; f < selectedList.GetCount(); f++)
		{
			if (f > 0)
  			   fprintf(out,"%s",delim);
			int loc = selectedList.GetItemData(f);

			switch(loc)
			{
			case 0:
				 fprintf(out,"Station ID");
				 break;
			case 1:
				 fprintf(out,"Name"); break;
            case 2:
				 fprintf(out,"Latitude"); break;
            case 3:
				 fprintf(out,"Longitude"); break;
			case 4:
				 fprintf(out,"Elevation"); break;
            case 5:
				 fprintf(out,"Station Type"); break;
			case 6: fprintf(out,"Site #");break;
			case 7: fprintf(out,"State");break;
            case 8:
				 fprintf(out,"County"); break;
			case 9:
				 fprintf(out,"Agency"); break;
			case 10:
				 fprintf(out,"Region"); break;
            case 11:
				 fprintf(out,"Unit"); break;
			case 12:
                 fprintf(out,"NFDRS fuel model"); break;
			case 13: fprintf(out,"Use 88?");break;
			case 14:
				 fprintf(out,"Climate class"); break;
			case 15: 
				 fprintf(out,"Slope class"); break;
			case 16:
				 fprintf(out,"Aspect"); break;
			case 17:
				 fprintf(out,"Position on slope"); break;
			case 18:
				 fprintf(out,"Herb annual?"); break;
			case 19:
				 fprintf(out,"Greenup date"); break;
			case 20:
				 fprintf(out,"Freeze date"); break;
			case 21:
				 fprintf(out,"Herb moisture"); break;
			case 22:
				 fprintf(out,"Shrub moisture"); break;
			case 23:
				 fprintf(out,"Initial KBDI"); break;
			case 24:
				 fprintf(out,"Deciduous?"); break;
			case 25:
				 fprintf(out,"Avg. precip"); break;
			case 26:
				 fprintf(out,"Initial 1000h"); break;
			case 27:
				 fprintf(out,"FM 1=10 ?"); break;
			case 28:
				 fprintf(out,"UseDormant?"); break;
			case 29:
				 fprintf(out,"DormantJulian"); break;
			case 30:
				 fprintf(out,"WRCC_ID"); break;
			case 31:
				 fprintf(out,"RegSchdObs"); break;
			case 32:
				 fprintf(out,"NESDIS_ID"); break;
			default: break;
			}
		}

    fprintf(out,"\n");

	records.MoveFirst();
	while(!records.IsEOF())
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
			case 0:  // station ID
				
			    temp.Format("%s",records.m_StationID);
				break;
			case 1: // name
				if(!records.IsFieldNull(&records.m_Name))
				   temp.Format("%s",records.m_Name);
				else
				   temp = "";
			    break;
			case 2: // Latitude
				if(!records.IsFieldNull(&records.m_LatDegrees))
					temp.Format("%10.5f", records.m_LatDegrees);
				else
					temp = "";
				break;
            case 3:// Longitude
				if(!records.IsFieldNull(&records.m_LonDegrees))
					temp.Format("%10.5f", records.m_LonDegrees);
				else
					temp = "";
				break;
			case 4: // elevation
				if(!records.IsFieldNull(&records.m_Elevation))
    				temp.Format("%d",records.m_Elevation);
				else
					temp = "";
			    break;
			case 5: // station type
                if(!records.IsFieldNull(&records.m_StnType))
					temp.Format("%d", records.m_StnType);
				else
					temp = "";
				break;
			case 6:  // site #
				 if(!records.IsFieldNull(&records.m_Site))
					temp.Format("%d", records.m_Site);
				else
					temp = "";
				break;
			
			case 7: // State
				if(!records.IsFieldNull(&records.m_State))
				   temp.Format("%s",records.m_State);
				else
				   temp = "";
			    break;

			case 8: // county   LOOKUP

              	if(!records.IsFieldNull(&records.m_County)){
			        CCountySet cSet(records.m_pDatabase);
			        CString strFilter;
                    strFilter.Format("[State] = '%s' AND [CountyID] = %d",records.m_State,records.m_County);
			        cSet.m_strFilter = strFilter;
                    cSet.Open();
			        CString cStr;
			        if (! cSet.IsEOF())
				       cStr.Format("%s",cSet.m_CountyName);
			        else
				       cStr = "";
			        cSet.Close();
			        temp.Format("%s",cStr);
		         } else
			        temp = "";
				 temp.Trim();
                 break;
			case 9: // Agency  LOOKUP
               	if(!records.IsFieldNull(&records.m_ObsAgy)){
					
			           CFireAgencySet agSet(records.m_pDatabase);
			           CString strFilter;
                       strFilter.Format("[AgencyID] = %d",records.m_ObsAgy);
			           agSet.m_strFilter = strFilter;
                       agSet.Open();
			           CString agStr;
			           if (! agSet.IsEOF())
				          agStr.Format("%s",agSet.m_Name);
			           else
				          agStr = "";
			           agSet.Close();
			           temp.Format("%s",agStr);
					
		         } else
			        temp = "";
				 temp.Trim();
				 break;
			case 10:  // region LOOKUP
			
				if(!records.IsFieldNull(&records.m_FSRegion)){

					// 2014 - here's a kludge fix for the USFS region numbering issue:
					//  there is no USFS region 7, so regions 8,9,10 have an index ID which
					//  is offset by 1

					long useRegion = records.m_FSRegion;
					if (records.m_ObsAgy == 1)
						if (useRegion > 6 && useRegion < 10)
							useRegion = useRegion - 1;

					
			           CFireRegionSet regSet(records.m_pDatabase);
			           CString strFilter;
                       strFilter.Format("[RegionID] = %d AND [AgencyID] = %d",useRegion,records.m_ObsAgy);
			           regSet.m_strFilter = strFilter;
                       regSet.Open();
			           CString regStr;
			           if (! regSet.IsEOF())
				          regStr.Format("%s",regSet.m_Name);
			           else
				          regStr = "";
			           regSet.Close();
			           temp.Format("%s",regStr);
					
		         } else
			        temp = "";
				 temp.Trim();
				 break;
			case 11:  // unit
				if(!records.IsFieldNull(&records.m_ObsUnit))
					temp.Format("%s", records.m_ObsUnit);
				else
					temp = "";
				
                temp.Trim();
				break;
			case 12: // Fuel Model
			    if(!records.IsFieldNull(&records.m_NFDRSFM))
					temp.Format("%s", records.m_NFDRSFM);
				else
					temp = "";

				break;
			case 13: // Use 88?
				if(!records.IsFieldNull(&records.m_Use88)){
					if (records.m_Use88)
						temp = "Y";
					else
					   temp = "N";
				} else
					temp = "";
				break;
            case 14: // climate class
                if(!records.IsFieldNull(&records.m_ClimateCls))
					temp.Format("%d", records.m_ClimateCls);
				else
					temp = "";
				break;
			case 15: // slope class
                if(!records.IsFieldNull(&records.m_SlopeCls))
					temp.Format("%d", records.m_SlopeCls);
				else
					temp = "";
				break;
			case 16: // aspect
                if(!records.IsFieldNull(&records.m_Aspect))
					temp.Format("%s", records.m_Aspect);
				else
					temp = "";
				break;

			case 17://position on slope
				if(!records.IsFieldNull(&records.m_PsnOnSlope))
					temp.Format("%s", records.m_PsnOnSlope);
				else
					temp = "";
				break;

			
			case 18: // herb annual?
				if(!records.IsFieldNull(&records.m_HerbAnnual)){
				   if (records.m_HerbAnnual)
						temp = "Y";
					else
					   temp = "N";
				} else
					temp = "";
				
				break;
			case 19:  // greenup date
			     if(!records.IsFieldNull(&records.m_GreenJulian))
				{
					switch(dt)
					{
				
					case Reverse:
					case MD:
					case Full:
						temp.Format("%02d/%02d", records.m_GreenJulian.GetMonth(),
							records.m_GreenJulian.GetDay());
						break;
					case MDPacked:
					case Packed:
						temp.Format("%02d%02d", records.m_GreenJulian.GetMonth(),
							records.m_GreenJulian.GetDay());
						break;
					}
				}
				else
					temp = "";
                 break;

 			case 20:  // freeze date
		        if(!records.IsFieldNull(&records.m_FreezeJulian))
				{
					switch(dt)
					{
			
					case MD:
					case Full:
					case Reverse:
						temp.Format("%02d/%02d", records.m_FreezeJulian.GetMonth(),
							records.m_FreezeJulian.GetDay());
						break;
					case MDPacked:
					case Packed:
						temp.Format("%02d%02d", records.m_FreezeJulian.GetMonth(),
							records.m_FreezeJulian.GetDay());
						break;
					}
				}
				else
					temp = "";
                 break;		

 			case 21:  // herb moisture
			    if(!records.IsFieldNull(&records.m_StartGreenHerb))
					temp.Format("%d", records.m_StartGreenHerb);
				else
					temp = "";
				break;
			case 22:  // shrub moisture
			    if(!records.IsFieldNull(&records.m_StartGreenShrub))
					temp.Format("%d", records.m_StartGreenShrub);
				else
					temp = "";
				break;

			case 23:  // initial KBDI
			    if(!records.IsFieldNull(&records.m_StartKBDI))
					temp.Format("%d", records.m_StartKBDI);
				else
					temp = "";
				break;
			case 24:  // deciduo
				if(!records.IsFieldNull(&records.m_Deciduous)){
				   if (records.m_Deciduous)
						temp = "Y";
					else
					   temp = "N";
				} else
					temp = "";
				
				break;
			case 25:  // avg precip
			    if(!records.IsFieldNull(&records.m_AvgPrecip))
					temp.Format("%6.2f", records.m_AvgPrecip);
				else
					temp = "";
				break;
			case 26:  // initial 1000h
			    if(!records.IsFieldNull(&records.m_Start1000))
					temp.Format("%6.2f", records.m_Start1000);
				else
					temp = "";
				break;
			case 27:  // FM 1=10
				if(!records.IsFieldNull(&records.m_FM1Eq10)){
				   if (records.m_FM1Eq10)
						temp = "Y";
					else
					   temp = "N";
				} else
					temp = "";
				
				break;
			case 28:  //UseDormant
				if(!records.IsFieldNull(&records.m_UseDormant)){
				   if (records.m_UseDormant)
						temp = "Y";
					else
					   temp = "N";
				} else
					temp = "";
			 	
				break;
			case 29:  //DormantJulian	
				if(!records.IsFieldNull(&records.m_DormantJulian)){
				   temp.Format("%d", records.m_DormantJulian);
				} else
					temp = "";
			 	
				break;
			case 30:
				
				if(!records.IsFieldNull(&records.m_WRCC_ID)){
				   temp.Format("%s", records.m_WRCC_ID);
				} else
					temp = "";
			 	
				break;
		    case 31:  //	RegSchedObs	
				if(!records.IsFieldNull(&records.m_RegSchdObs)){
				   temp.Format("%d", records.m_RegSchdObs);
				} else
					temp = "";
			 	
				break;
			case 32: //NESDIS_ID
				if(!records.IsFieldNull(&records.m_NESDIS_ID)){
				   temp.Format("%s", records.m_NESDIS_ID);
				} else
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
		records.MoveNext();
		buf = "";
	}

	records.Close();
	//records->SetBookmark( varRecordToReturnTo );

	fclose(out);
	chdir(theApp.dbDir);
	theApp.SetUserDir(fd.GetPathName());
	msg.Format("Textfile data export Complete!\n%ld records written to\n%s... ", count, outFileName);
	AfxMessageBox(msg);

	CDialog::OnOK();

	return;

}



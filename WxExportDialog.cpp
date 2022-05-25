// WxExportDialog.cpp : implementation file
//
 
#include "stdafx.h"
#include "fireplus.h"
#include "WxExportDialog.h"
#include "cdtlib.h"
#include <direct.h>
#include <math.h>
#include "SIGStationSet.h"
#include "CFW21ExportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double PotentialRadiation(int year, int month, int day,
            int hour, int minute, double lat, double lon, long elev, long slope, long aspect)
{
// calculates solar radiation (W/m2) using Collin Bevins model
     double cloudTransmittance=1.0;//-(double) cloud/100.0;
     double Rad, jdate, latitude= lat;
     double atmTransparency=0.7;
     double canopyTransmittance=1.0;
	long i, Tolerance=2;//pFlamMap->GetTolerance();

	 //if(date >= days)
	//	 month = 11;
     jdate=CDT_JulianDate(year, month, day, hour, minute, 0, 0);
     Rad=CDT_SolarRadiation(jdate, 0, latitude, 0.0, (double) slope, (double) aspect, (double) elev / 3.2808,
          atmTransparency, cloudTransmittance, canopyTransmittance);
	//printf("SimpleRadiation returning:  %f, hour = %f date= %ld,\n\thour= %f, cloud= %ld, elev= %ld,\n\tslope= %ld, aspect= %ld, cover= %ld\n", Rad * 1370.0, hour/100,
	//	  date, hour, cloud, elev, slope, aspect, cover);

	//TRACE3("SimpleRadiation(date = %ld, hour = %f, cloud = %ld, ", date, hour, cloud);
	//TRACE3("elev = %ld, slope = %ld, aspect = %ld, ", elev, slope, aspect);
	//TRACE2("cover = %ld) returns %f\n", cover, Rad*1370.0);
     return Rad*=1370.0; //W/m2
}

extern CFireplusApp theApp;
extern char *colNames[];
//extern char *colFormats[];
/////////////////////////////////////////////////////////////////////////////
// CWxExportDialog dialog
char *outFormats[prnFields] =
{
	"%6.6s",
	"%10.10s",
	"%5.5s",
	"%1.1s",
	"%1d",
	"%3d",
	"%3d",
	"%5.2f",
	"%2d",
	"%3d",
	"%1d",
	"%3d",
	"%3d",
	"%3d",
	"%3d",
	"%3d",
	"%1d",
	"%6.2f",
	"%3d",
	"%3d",
	"%1d",
	"%6d",
	"%3d",
	"%3d",
	"%5.2f",
	"%1d",
	"%1d",
	"%1d",
	"%6.2f",
	"%6.2f",
	"%6.2f",
	"%6.2f",
	"%6.2f",
};

CWxExportDialog::CWxExportDialog(CWnd* pParent, CWxSet *_records)
	: CDialog(CWxExportDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWxExportDialog)
	//}}AFX_DATA_INIT
	records = _records;
}

void CWxExportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWxExportDialog)
	DDX_Control(pDX, IDC_FWX, fwx);
	DDX_Control(pDX, IDC_FLAMMAPFARSITE_STREAM, flammap);
	DDX_Control(pDX, IDC_DELIMSTATIC, delimStatic);
	DDX_Control(pDX, IDC_MOVELEFT, moveLeft);
	DDX_Control(pDX, IDC_MOVERIGHT, moveRight);
	DDX_Control(pDX, IDC_REMOVEALL, removeAll);
	DDX_Control(pDX, IDC_SELECTALL, selectAll);
	DDX_Control(pDX, IDC_SELECTSTATIC, selectStatic);
	DDX_Control(pDX, IDC_SPACE, space);
	DDX_Control(pDX, IDC_TIMESTATIC, timeStatic);
	DDX_Control(pDX, IDC_TIME1, time1);
	DDX_Control(pDX, IDC_DATE1, date1);
	DDX_Control(pDX, IDC_SELECTEDLIST, selectedList);
	DDX_Control(pDX, IDC_FIELDSLIST, fieldsList);
	DDX_Control(pDX, IDC_DATESTATIC, dateStatic);
	DDX_Control(pDX, IDC_CUSTSTATIC, custStatic);
	DDX_Control(pDX, IDC_AVAILSTATIC, availStatic);
	DDX_Control(pDX, IDC_RADIO1, windsAvg);
	DDX_Control(pDX, IDC_RADIO2, windsGusts);

	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RADIO_HOURLY_PRECIP, m_btnHourlyPrecip);
	DDX_Control(pDX, IDC_RADIO_DAILY_PRECIP, m_btnDailyPrecip);
}

BEGIN_MESSAGE_MAP(CWxExportDialog, CDialog)
	//{{AFX_MSG_MAP(CWxExportDialog)
	ON_LBN_DBLCLK(IDC_SELECTEDLIST, OnDblclkSelectedlist)
	ON_LBN_DBLCLK(IDC_FIELDSLIST, OnDblclkFieldslist)
	ON_BN_CLICKED(IDC_MOVELEFT, OnMoveleft)
	ON_BN_CLICKED(IDC_MOVERIGHT, OnMoveright)
	ON_BN_CLICKED(IDC_REMOVEALL, OnRemoveall)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_CUSTOM, OnCustom)
	ON_BN_CLICKED(IDC_FW9, OnFw9)
	ON_BN_CLICKED(IDC_FWX, OnFw13)   // note 2014 change here

	ON_BN_CLICKED(IDC_DATE1, OnDate1)
	ON_BN_CLICKED(IDC_DATE2, OnDate2)
	ON_BN_CLICKED(IDC_DATE3, OnDate3)
	ON_BN_CLICKED(IDC_DATE4, OnDate4)
	ON_BN_CLICKED(IDC_DATE5, OnDate5)
	ON_BN_CLICKED(IDC_TIME1, OnTime1)
	ON_BN_CLICKED(IDC_TIME2, OnTime2)
	ON_BN_CLICKED(IDC_RADIO1,OnWindsAvg)
	ON_BN_CLICKED(IDC_RADIO2,OnWindsGusts)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	ON_BN_CLICKED(IDC_BOTTOM, OnBottom)
	ON_BN_CLICKED(IDC_FLAMMAPFARSITE_STREAM, &CWxExportDialog::OnBnClickedFlammapFARSITEStream)
	ON_BN_CLICKED(IDC_RADIO_HOURLY_PRECIP, &CWxExportDialog::OnBnClickedRadioHourlyPrecip)
	ON_BN_CLICKED(IDC_RADIO_DAILY_PRECIP, &CWxExportDialog::OnBnClickedRadioDailyPrecip)
	ON_BN_CLICKED(IDC_RADIO_FW21, &CWxExportDialog::OnBnClickedRadioFw21)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWxExportDialog message handlers

BOOL CWxExportDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	int loc;
	for(int f = 0; f < prnFields; f++)
	{
		loc = fieldsList.AddString(colNames[f]);
		fieldsList.SetItemData(loc, f);
	}
	upButton.AutoLoad(IDC_UP, this);
	downButton.AutoLoad(IDC_DOWN, this);
	topButton.AutoLoad(IDC_TOP, this);
	bottomButton.AutoLoad(IDC_BOTTOM, this);
	fwx.SetCheck(TRUE);
	dt = Full;
	date1.SetCheck(TRUE);
	tt = FullTime;
	time1.SetCheck(TRUE);
	space.SetCheck(TRUE);

	windsAvg.SetCheck(TRUE);
	windsGusts.SetCheck(FALSE);
	windsType = 0;

	fw13PrecipType = 0;

	m_btnHourlyPrecip.SetCheck(TRUE);
	SetAppearance();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWxExportDialog::SetAppearance()
{
	//enable/disable custom export options
	BOOL enable = FALSE;
	CButton *butt = (CButton *)GetDlgItem(IDC_CUSTOM);
	enable = butt->GetCheck();// == TRUE) ? TRUE : FALSE;
	availStatic.EnableWindow(enable);
	bottomButton.EnableWindow(enable);
	custStatic.EnableWindow(enable);
	dateStatic.EnableWindow(enable);
	delimStatic.EnableWindow(enable);
	downButton.EnableWindow(enable);
	fieldsList.EnableWindow(enable);
	moveLeft.EnableWindow(enable);
	moveRight.EnableWindow(enable);
	removeAll.EnableWindow(enable);
	selectAll.EnableWindow(enable);
	selectedList.EnableWindow(enable);
	selectStatic.EnableWindow(enable);
	timeStatic.EnableWindow(enable);
	topButton.EnableWindow(enable);
	upButton.EnableWindow(enable);
	date1.EnableWindow(enable);
	butt = (CButton *)GetDlgItem(IDC_DATE2);
	butt->EnableWindow(enable);
	butt = (CButton *)GetDlgItem(IDC_DATE3);
	butt->EnableWindow(enable);
	butt = (CButton *)GetDlgItem(IDC_DATE4);
	butt->EnableWindow(enable);
	butt = (CButton *)GetDlgItem(IDC_DATE5);
	butt->EnableWindow(enable);
	time1.EnableWindow(enable);
	butt = (CButton *)GetDlgItem(IDC_TIME2);
	butt->EnableWindow(enable);
	space.EnableWindow(enable);
	butt = (CButton *)GetDlgItem(IDC_COMMA);
	butt->EnableWindow(enable);
	butt = (CButton *)GetDlgItem(IDC_NONE);
	butt->EnableWindow(enable);
	if (flammap.GetCheck())
	{
	   windsAvg.EnableWindow(TRUE);
	   windsGusts.EnableWindow(TRUE);
	} else
	{
	   windsAvg.EnableWindow(FALSE);
	   windsGusts.EnableWindow(FALSE);
	}
	//now deal with precip options for FW13
	BOOL isFW13 = fwx.GetCheck();
	m_btnDailyPrecip.EnableWindow(isFW13);
	m_btnHourlyPrecip.EnableWindow(isFW13);
}

void CWxExportDialog::OnDblclkSelectedlist()
{
	CString str;
	int cur = selectedList.GetCurSel();
	selectedList.GetText(cur, str);
	int val = selectedList.GetItemData(cur);
	int loc = fieldsList.AddString(str);
	fieldsList.SetItemData(loc, val);
	selectedList.DeleteString(cur);
}

void CWxExportDialog::OnDblclkFieldslist()
{
	CString str;
	int cur = fieldsList.GetCurSel();
	fieldsList.GetText(cur, str);
	int val = fieldsList.GetItemData(cur);
	int loc = selectedList.AddString(str);
	selectedList.SetItemData(loc, val);
	fieldsList.DeleteString(cur);
}

void CWxExportDialog::OnMoveleft()
{
	if(selectedList.GetCurSel() != LB_ERR)
		OnDblclkSelectedlist();
}

void CWxExportDialog::OnMoveright()
{
	if(fieldsList.GetCurSel() != LB_ERR)
		OnDblclkFieldslist();
}

void CWxExportDialog::OnRemoveall()
{
	while(selectedList.GetCount() > 0)
	{
		selectedList.SetCurSel(0);
		OnDblclkSelectedlist();
	}
}

void CWxExportDialog::OnSelectall()
{
	while(fieldsList.GetCount() > 0)
	{
		fieldsList.SetCurSel(0);
		OnDblclkFieldslist();
	}
}

void CWxExportDialog::OnUp()
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

void CWxExportDialog::OnTop()
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

void CWxExportDialog::OnDown()
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

void CWxExportDialog::OnBottom()
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

void CWxExportDialog::OnCustom()
{
	SetAppearance();
}

void CWxExportDialog::OnFw9()
{
	SetAppearance();
}

void CWxExportDialog::OnFw13()
{
	SetAppearance();
}

void CWxExportDialog::OnFwx()
{
	SetAppearance();
}

void CWxExportDialog::OnWindsGusts()
{
	
}

void CWxExportDialog::OnWindsAvg()
{
	
}




void CWxExportDialog::OnOK()
{
	CButton *butt = (CButton *)GetDlgItem(IDC_CUSTOM);
	if(butt->GetCheck() == TRUE && selectedList.GetCount() <= 0)
	{
		AfxMessageBox("ERROR: At least one field must be selected\nfor custom exports.");
		return;
	}
	CButton *butt2 = (CButton *)GetDlgItem(IDC_FLAMMAPFARSITE_STREAM);
	CButton *fw21Button = (CButton *)GetDlgItem(IDC_RADIO_FW21);
	int status;
	if (butt->GetCheck() == TRUE)
		status = ExportCustom();
	else if (fwx.GetCheck() == TRUE)
		status = ExportFW13();  // 2014 change
	else if (butt2->GetCheck() == TRUE)
		status = ExportFlamMapFARSITEStream();
	else if (fw21Button->GetCheck() == TRUE)
		status = ExportFW21();
	else
		status = ExportFW9();
	if(status)
		CDialog::OnOK();
}

int CWxExportDialog::ExportFWX()
{
	if(IDOK != AfxMessageBox("Warning! FWX files only handle NFDRS weather\nobservations (ObsType = 'O'). All non-NFDRS\n"
		"type weather observations will not be exported.\nContinue?", MB_OKCANCEL))
		return 0;
	char buf[128], temp[64];
	long count = 0;
//	char obtype;
	CString msg;
	chdir(theApp.userDir);
	CFileDialog fd(FALSE, "fwx", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Fire Weather Files (*.fwx)|*.fwx|All Files (*.*)|*.*||", this);
	if(fd.DoModal() != IDOK)
		return 0;
	outFileName = fd.GetPathName();
	FILE *out = fopen(outFileName, "wt");
	if(!out)
	{
		msg.Format("ERROR: Unable to create output file:\n%s", outFileName);
		AfxMessageBox(msg);
		return 0;
	}
	CRecordsetStatus recStatus;
	records->GetStatus(recStatus);
	long retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
//	long retRec = records->GetAbsolutePosition();
	records->MoveFirst();
	while(!records->IsEOF())
	{
		sprintf(temp, "%1.1s", records->m_ObsType);
		if(temp[0] == 'O')
		{
			//nfdrs observation, write it to file in fwx format
			strcpy_s(buf, records->m_StationID);
			sprintf(temp, "%02d%02d%02d", records->m_ObsDate.GetYear() % 100,
				records->m_ObsDate.GetMonth(), records->m_ObsDate.GetDay());
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_SOW))
				sprintf(temp, "%d", records->m_SOW);
			else
				strcpy_s(temp, " ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_Temp))
				sprintf(temp, "%3d", records->m_Temp);
			else
				strcpy_s(temp, "   ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_RH))
				sprintf(temp, "%3d", max(records->m_RH, 1));
			else
				strcpy_s(temp, "   ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_GreenHerb))
				sprintf(temp, "%3d", records->m_GreenHerb);
			else
				strcpy_s(temp, "   ");
			strcat(buf, temp);
			strcat(buf, "     "); //veg condition and human risk
			if(!records->IsFieldNull(&records->m_WDir))
				sprintf(temp, "%d", records->m_WDir);
			else
				strcpy_s(temp, " ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_WS))
				sprintf(temp, "%3d", records->m_WS);
			else
				strcpy_s(temp, "   ");
			strcat(buf, temp);
			strcat(buf, " ");
			if(!records->IsFieldNull(&records->m_OMC10))
				sprintf(temp, "%3.0f", records->m_OMC10);
			else
				strcpy_s(temp, "   ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_GreenShrub))
				sprintf(temp, "%3d", records->m_GreenShrub);
			else
				strcpy_s(temp, "   ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_TmpMax))
				sprintf(temp, "%3d", records->m_TmpMax);
			else
				strcpy_s(temp, "   ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_TmpMin))
				sprintf(temp, "%3d", records->m_TmpMin);
			else
				strcpy_s(temp, "   ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_RHMax))
				sprintf(temp, "%3d", records->m_RHMax);
			else
				strcpy_s(temp, "   ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_RHMin))
				sprintf(temp, "%3d", max(records->m_RHMin, 1));
			else
				strcpy_s(temp, "   ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_Season))
				sprintf(temp, "%d", records->m_Season);
			else
				strcpy_s(temp, " ");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_PPTDUR))
				sprintf(temp, "%2d", records->m_PPTDUR);
			else
				strcpy_s(temp, " 0");
			strcat(buf, temp);
			if(!records->IsFieldNull(&records->m_PPTAMT))
			{
				double amt = records->m_PPTAMT;// * 100.0);
				if(records->m_PPTAMT > 0.0 && amt < 0.0099999)
					sprintf(temp, "000T");
				else
					sprintf(temp, "%04.0f", amt * 100.0);
			}
			else
				strcpy_s(temp, "0000");
			strcat(buf, temp);
			strcat(buf, "  12                   ");
			fprintf(out, "%s\n", buf);
			count++;
		}
		records->MoveNext();
	}

	records->SetAbsolutePosition(retRec + 1);
	fclose(out);
	chdir(theApp.dbDir);
	theApp.SetUserDir(fd.GetPathName());
	msg.Format("Export Complete!\n%ld records written to\n%s", count, outFileName);
	AfxMessageBox(msg);
	return 1;
}

int CWxExportDialog::ExportFlamMapFARSITEStream()
{
	char buf[128], temp[64];
	long count = 0;
//	char obtype;
	CString msg;

	CButton *butt = (CButton *)GetDlgItem(IDC_RADIO1);
	if (butt->GetCheck())
		windsType = 0;  // avg
	else
		windsType = 1;  // gusts

	chdir(theApp.userDir);
	CFileDialog fd(FALSE, "wxs", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Flammap / Farsite Weather Files (*.wxs)|*.wxs|All Files (*.*)|*.*||", this);
	if(fd.DoModal() != IDOK)
		return 0;
	outFileName = fd.GetPathName();
	FILE *out = fopen(outFileName, "wt");
	if(!out)
	{
		msg.Format("ERROR: Unable to create output file:\n%s", outFileName);
		AfxMessageBox(msg);
		return 0;
	}
	CRecordsetStatus recStatus;
	records->GetStatus(recStatus);
	long retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
	double stnLat = 0, stnLon = 0;
	long stnSlope = 0, stnAspect = 0, stnElev = 0;
	CSIGStationSet staSet(records->m_pDatabase);
	staSet.m_strFilter.Format("[StationID] = '%6s'", records->m_StationID);
	staSet.Open();
	if(!staSet.IsEOF())
	{
		if(!staSet.IsFieldNull(&staSet.m_LatDegrees))
			stnLat = staSet.m_LatDegrees;
		if(!staSet.IsFieldNull(&staSet.m_LatDegrees))
			stnElev = (long)staSet.m_Elevation;
		if(!staSet.IsFieldNull(&staSet.m_LonDegrees))
			stnLon = staSet.m_LonDegrees;
		//	stnSlope = (long)staSet.m_LatDegrees;
		//if(!staSet.IsFieldNull(&staSet.m_LatDegrees))
			//stnAspect = (long)staSet.m_LatDegrees;
	}

	fprintf(out,"RAWS_ELEVATION: %ld\n", stnElev);
    if (windsType == 0)
	   fprintf(out,"RAWS_WINDS: Ave\n");
	else
	   fprintf(out,"RAWS_WINDS: Gust\n");

	fprintf(out,"RAWS_UNITS: English\n\n");

	fprintf(out, "Year  Mth  Day   Time    Temp     RH    HrlyPcp   WindSpd  WindDir CloudCov\n");
    //				  2010   12      30   0100      10           20             1.2                 40            50         60
	records->MoveFirst();
	while(!records->IsEOF())
	{
		if(records->IsFieldNull(&records->m_ObsDate)
			|| records->IsFieldNull(&records->m_Temp)
			|| records->IsFieldNull(&records->m_RH)
			|| records->IsFieldNull(&records->m_HourlyPrecip)
			|| records->IsFieldNull(&records->m_WS)
			|| records->IsFieldNull(&records->m_WAzimuth)
			)
		{
			records->MoveNext();
			continue;
		}
		int m = records->m_ObsDate.GetMonth() - 1;
		
		long ws = 0, wd = 0;

		if (windsType == 0)
		{
			ws = records->m_WS;
			wd = records->m_WAzimuth;
		} else 
		{
			if (records->IsFieldNull(&records->m_GustSpd) ||  records->IsFieldNull(&records->m_GustDir)  || records->m_GustSpd > 100 || records->m_GustDir > 360)
			{
				ws = -9999;
				wd = -9999;
			} else 
			{
				ws = records->m_GustSpd;
				wd = records->m_GustDir;
			}
		}

		
		sprintf(buf, "%4d   %2d   %2d   %04d    %4d   %4d   %8.2f   %6d  %6d",
			records->m_ObsDate.GetYear(),
			records->m_ObsDate.GetMonth(),
			records->m_ObsDate.GetDay(),
			records->m_ObsDate.GetHour() * 100 + records->m_ObsDate.GetMinute(),
			records->m_Temp,
			max(records->m_RH, 1),
			records->m_HourlyPrecip,
			ws,
			wd
			);
		//compute cloud cover
		int cc = 0;
		if(records->IsFieldNull(&records->m_SolarRadiation))
		{
			if(records->m_HourlyPrecip > 0)
				cc = 100;
		}
		else
		{
			if(records->m_HourlyPrecip > 0.0)
				cc = 100;
			else
			{
				double potSolRad = PotentialRadiation(records->m_ObsDate.GetYear(),
					records->m_ObsDate.GetMonth(),
					records->m_ObsDate.GetDay(),
					records->m_ObsDate.GetHour(),
					records->m_ObsDate.GetMinute(),
					stnLat,
					stnLon,
					stnElev,
					stnSlope,
					stnAspect);
				double pctSolRad = 100;
				if(potSolRad > 100)
					pctSolRad = (double)records->m_SolarRadiation / potSolRad * 100.0;
				cc = 100 - pctSolRad;
				cc = max(0, cc);
				cc = min(100, cc);
			//	TRACE3("Potential = %.0f, Observed = %d, cc = %d\n",
			//		potSolRad, records->m_SolarRadiation, cc);
			}
		}
		sprintf(temp, "  %4d", cc);
		strcat(buf, temp);
		fprintf(out, "%s\n", buf);
		count++;
		records->MoveNext();
	}

	records->SetAbsolutePosition(retRec + 1);
	fclose(out);
	chdir(theApp.dbDir);
	theApp.SetUserDir(fd.GetPathName());
	msg.Format("Export Complete!\n%ld records written to\n%s", count, outFileName);
	AfxMessageBox(msg);
	return 1;
}

int CWxExportDialog::ExportFW21()
{
	//first, ensure we have timezone offsets so we can convert to UTC


	CRecordsetStatus recStatus;
	records->GetStatus(recStatus);
	long retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
	CFW21ExportDlg FW21Dlg;
	FW21Dlg.records = records;
	FW21Dlg.DoModal();
	records->SetAbsolutePosition(retRec + 1);
	chdir(theApp.dbDir);
	return 1;
}

int CWxExportDialog::ExportFW13()
{
	char buf[128], temp[64];
	long count = 0;
	char obtype;
	//int wDir;
	CString msg;
	chdir(theApp.userDir);
	CFileDialog fd(FALSE, "fw13", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Fire Weather Files (*.fw13)|*.fw13|All Files (*.*)|*.*||", this);
	if(fd.DoModal() != IDOK)
		return 0;
	outFileName = fd.GetPathName();
	FILE *out = fopen(outFileName, "wt");
	if(!out)
	{
		msg.Format("ERROR: Unable to create output file:\n%s", outFileName);
		AfxMessageBox(msg);
		return 0;
	}
	CRecordsetStatus recStatus;
	records->GetStatus(recStatus);
	long retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
//	long retRec = records->GetAbsolutePosition();
	records->MoveFirst();
	while(!records->IsEOF())
	{
		strcpy_s(buf, "W13");
		strcat(buf, records->m_StationID);
		sprintf(temp, "%d%02d%02d%02d%02d", records->m_ObsDate.GetYear(), records->m_ObsDate.GetMonth(),
			records->m_ObsDate.GetDay(), records->m_ObsDate.GetHour(), records->m_ObsDate.GetMinute());
		strcat(buf, temp);
		sprintf(temp, "%1.1s", records->m_ObsType);
		obtype = temp[0];
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_SOW))
			sprintf(temp, "%1d", records->m_SOW);
		else
			strcpy_s(temp, " ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_Temp))
			sprintf(temp, "%3d", records->m_Temp);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_RH))
			sprintf(temp, "%3d", max(records->m_RH, 1));
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_WAzimuth))
			sprintf(temp, "%3d", records->m_WAzimuth);
		else if(!records->IsFieldNull(&records->m_WDir))
		{
			int tdir = records->m_WDir;
			if(obtype == 'R')//convert raws to azimuth
			{
				if(tdir > 1)
					tdir = (tdir - 1) * 45;
				else if(tdir == 1)
					tdir = 360;
			}
			sprintf(temp, "%3d", tdir);
		}
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_WS))
			sprintf(temp, "%3d", records->m_WS);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_OMC10))
			sprintf(temp, "%2.0f", records->m_OMC10);
		else
			strcpy_s(temp, "  ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_TmpMax))
			sprintf(temp, "%3d", records->m_TmpMax);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_TmpMin))
			sprintf(temp, "%3d", records->m_TmpMin);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_RHMax))
			sprintf(temp, "%3d", records->m_RHMax);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_RHMin))
			sprintf(temp, "%3d", max(records->m_RHMin, 1));
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		//cols 49-50, pcp duration
		if (fw13PrecipType == 1)//24hr precip
		{
			if (!records->IsFieldNull(&records->m_PPTDUR))
				sprintf(temp, "%2d", records->m_PPTDUR);
			else
				strcpy_s(temp, " 0");
			strcat(buf, temp);
			if (!records->IsFieldNull(&records->m_PPTAMT))
				sprintf(temp, "%5.0f", records->m_PPTAMT * 1000.0);
			else
				strcpy_s(temp, "    0");
			strcat(buf, temp);
		}
		else //hourly, put 0 or 1
		{
			if (!records->IsFieldNull(&records->m_HourlyPrecip))
			{
				if(records->m_HourlyPrecip > 0.0)
					strcpy_s(temp, " 1");
				else
					strcpy_s(temp, " 0");
			}
			else
				strcpy_s(temp, " 0");
			strcat(buf, temp);
			if (!records->IsFieldNull(&records->m_HourlyPrecip))
				sprintf(temp, "%5.0f", records->m_HourlyPrecip * 1000.0);
			else
				strcpy_s(temp, "    0");
			strcat(buf, temp);
		}
		if(!records->IsFieldNull(&records->m_WetFlag))
		{
			if(records->m_WetFlag == 1)
				strcat(buf, "Y");//column 56 wet flag always = N
			else
				strcat(buf, "N");//column 56 wet flag always = N
		}
		else
			strcat(buf, " ");//column 56 wet flag always = N
		if(!records->IsFieldNull(&records->m_GreenHerb))
			sprintf(temp, "%2d", records->m_GreenHerb);
		else
			strcpy_s(temp, "  ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_GreenShrub))
			sprintf(temp, "%2d", records->m_GreenShrub);
		else
			strcpy_s(temp, "  ");
		strcat(buf, temp);
		strcat(buf, "2");//col 61, measurement types(RH, U.S. units)
		if (fw13PrecipType == 0)
			strcat(buf, "3");
		else
			strcat(buf, "1");
		//grrr westfork sample doesnt even have this column(season), line return
		if(!records->IsFieldNull(&records->m_Season)) // 63
			sprintf(temp, "%1d", records->m_Season);
		else
			strcpy_s(temp, " ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_SolarRadiation))  //64-67
		{
			sprintf(temp, "%4d", records->m_SolarRadiation);
		//else
		//	strcpy_s(temp, "    ");
			strcat(buf, temp);
		//strcat(buf, "    ");//solar radiation is blank
		}
		else
			strcat(buf, "    ");//solar radiation is blank

		if(!records->IsFieldNull(&records->m_GustDir))
			sprintf(temp, "%3d", records->m_GustDir);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);

		if(!records->IsFieldNull(&records->m_GustSpd))
			sprintf(temp, "%3d", records->m_GustSpd);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);

		if(!records->IsFieldNull(&records->m_SnowFlag))
		{
		   if(records->m_SnowFlag == 1)
				strcat(buf, "Y");
			else
				strcat(buf, "N");
		} else
            strcat(buf, " ");

		fprintf(out, "%s\n", buf);
		count++;
		records->MoveNext();
	}

	records->SetAbsolutePosition(retRec + 1);
	fclose(out);
	chdir(theApp.dbDir);
	theApp.SetUserDir(fd.GetPathName());
	msg.Format("FW13 Export Complete!\n%ld records written to\n%s", count, outFileName);
	AfxMessageBox(msg);
	return 1;
}


int CWxExportDialog::ExportFW9()
{
	char buf[128], temp[64];
	long count = 0;
	char obtype;
	//int wDir;
	CString msg;
	chdir(theApp.userDir);
	CFileDialog fd(FALSE, "fw9", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Fire Weather Files (*.fw9)|*.fw9|All Files (*.*)|*.*||", this);
	if(fd.DoModal() != IDOK)
		return 0;
	outFileName = fd.GetPathName();
	FILE *out = fopen(outFileName, "wt");
	if(!out)
	{
		msg.Format("ERROR: Unable to create output file:\n%s", outFileName);
		AfxMessageBox(msg);
		return 0;
	}
	CRecordsetStatus recStatus;
	records->GetStatus(recStatus);
	long retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
//	long retRec = records->GetAbsolutePosition();
	records->MoveFirst();
	while(!records->IsEOF())
	{
		strcpy_s(buf, "W98");
		strcat(buf, records->m_StationID);
		sprintf(temp, "%d%02d%02d%02d%02d", records->m_ObsDate.GetYear(), records->m_ObsDate.GetMonth(),
			records->m_ObsDate.GetDay(), records->m_ObsDate.GetHour(), records->m_ObsDate.GetMinute());
		strcat(buf, temp);
		sprintf(temp, "%1.1s", records->m_ObsType);
		obtype = temp[0];
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_SOW))
			sprintf(temp, "%1d", records->m_SOW);
		else
			strcpy_s(temp, " ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_Temp))
			sprintf(temp, "%3d", records->m_Temp);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_RH))
			sprintf(temp, "%3d", max(records->m_RH, 1));
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_WAzimuth))
			sprintf(temp, "%3d", records->m_WAzimuth);
		else if(!records->IsFieldNull(&records->m_WDir))
		{
			int tdir = records->m_WDir;
			if(obtype == 'R')//convert raws to azimuth
			{
				if(tdir > 1)
					tdir = (tdir - 1) * 45;
				else if(tdir == 1)
					tdir = 360;
			}
			sprintf(temp, "%3d", tdir);
		}
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_WS))
			sprintf(temp, "%3d", records->m_WS);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_OMC10))
			sprintf(temp, "%2.0f", records->m_OMC10);
		else
			strcpy_s(temp, "  ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_TmpMax))
			sprintf(temp, "%3d", records->m_TmpMax);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_TmpMin))
			sprintf(temp, "%3d", records->m_TmpMin);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_RHMax))
			sprintf(temp, "%3d", records->m_RHMax);
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_RHMin))
			sprintf(temp, "%3d", max(records->m_RHMin, 1));
		else
			strcpy_s(temp, "   ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_PPTDUR))
			sprintf(temp, "%2d", records->m_PPTDUR);
		else
			strcpy_s(temp, " 0");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_PPTAMT))
			sprintf(temp, "%5.0f", records->m_PPTAMT * 1000.0);
		else
			strcpy_s(temp, "    0");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_WetFlag))
		{
			if(records->m_WetFlag == 1)
				strcat(buf, "Y");//column 56 wet flag always = N
			else
				strcat(buf, "N");//column 56 wet flag always = N
		}
		else
			strcat(buf, " ");//column 56 wet flag always = N
		if(!records->IsFieldNull(&records->m_GreenHerb))
			sprintf(temp, "%2d", records->m_GreenHerb);
		else
			strcpy_s(temp, "  ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_GreenShrub))
			sprintf(temp, "%2d", records->m_GreenShrub);
		else
			strcpy_s(temp, "  ");
		strcat(buf, temp);
		strcat(buf, "21");//cols 61 & 62, measurement types(RH, U.S. units)
		//grrr westfork sample doesnt even have this column(season), line return
		if(!records->IsFieldNull(&records->m_Season)) // 63
			sprintf(temp, "%1d", records->m_Season);
		else
			strcpy_s(temp, " ");
		strcat(buf, temp);
		if(!records->IsFieldNull(&records->m_SolarRadiation))  //64-67
		{
			sprintf(temp, "%4d", records->m_SolarRadiation);
		//else
		//	strcpy_s(temp, "    ");
			strcat(buf, temp);
		//strcat(buf, "    ");//solar radiation is blank
		}
		else
			strcat(buf, "    ");//solar radiation is blank

		fprintf(out, "%s\n", buf);
		count++;
		records->MoveNext();
	}

	records->SetAbsolutePosition(retRec + 1);
	fclose(out);
	chdir(theApp.dbDir);
	theApp.SetUserDir(fd.GetPathName());
	msg.Format("FW9 Export Complete!\n%ld records written to\n%s", count, outFileName);
	AfxMessageBox(msg);
	return 1;
}

int CWxExportDialog::ExportCustom()
{
	CString buf, temp;
	long count = 0;
//	char obtype;
	//int wDir;
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
		return 0;
	outFileName = fd.GetPathName();
	FILE *out = fopen(outFileName, "wt");
	if(!out)
	{
		msg.Format("ERROR: Unable to create output file:\n%s", outFileName);
		AfxMessageBox(msg);
		return 0;
	}
	CRecordsetStatus recStatus;
	records->GetStatus(recStatus);
	long retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
//	long retRec = records->GetAbsolutePosition();
	records->MoveFirst();
	while(!records->IsEOF())
	{
		for(int f = 0; f < selectedList.GetCount(); f++)
		{
			if(f > 0)
				buf += delim;
			CString temp, fmt;
			int loc = selectedList.GetItemData(f);
			fmt.Format("%s", outFormats[loc]);
			switch(loc)
			{
			case 0:
				temp.Format(fmt, records->m_StationID);
				break;
			case 1:
				if(!records->IsFieldNull(&records->m_ObsDate))
				{
					switch(dt)
					{
					case Full:
						temp.Format("%02d/%02d/%4d", records->m_ObsDate.GetMonth(),
							records->m_ObsDate.GetDay(), records->m_ObsDate.GetYear());
						break;
					case Packed:
						temp.Format("%02d%02d%4d", records->m_ObsDate.GetMonth(),
							records->m_ObsDate.GetDay(), records->m_ObsDate.GetYear());
						break;
					case Reverse:
						temp.Format("%4d%02d%02d", records->m_ObsDate.GetYear(), records->m_ObsDate.GetMonth(),
							records->m_ObsDate.GetDay());
						break;
					case MD:
						temp.Format("%02d/%02d", records->m_ObsDate.GetMonth(),
							records->m_ObsDate.GetDay());
						break;
					case MDPacked:
						temp.Format("%02d%02d", records->m_ObsDate.GetMonth(),
							records->m_ObsDate.GetDay());
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
				break;
			case 2:
				if(!records->IsFieldNull(&records->m_ObsDate))
				{
					switch(tt)
					{
					case FullTime:
						temp.Format("%02d:%02d", records->m_ObsDate.GetHour(),
							records->m_ObsDate.GetMinute());
						break;
					case PackedTime:
						temp.Format("%02d%02d", records->m_ObsDate.GetHour(),
							records->m_ObsDate.GetMinute());
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
				break;
			case 3:
				if(!records->IsFieldNull(&records->m_ObsType))
					temp.Format(fmt, records->m_ObsType);
				else
					temp.Format(fmt, " ");
				break;
			case 4:
				if(!records->IsFieldNull(&records->m_SOW))
					temp.Format(fmt, records->m_SOW);
				else
					temp = " ";
				break;
			case 5:
				if(!records->IsFieldNull(&records->m_Temp))
					temp.Format(fmt, records->m_Temp);
				else
					temp = "   ";
				break;
			case 6:
				if(!records->IsFieldNull(&records->m_RH))
					temp.Format(fmt, max(records->m_RH, 1));
				else
					temp = "   ";
				break;
			case 7:
				if(!records->IsFieldNull(&records->m_PPTAMT))
					temp.Format(fmt, records->m_PPTAMT);
				else
					temp = "     ";
				break;
			case 8:
				if(!records->IsFieldNull(&records->m_PPTDUR))
					temp.Format(fmt, records->m_PPTDUR);
				else
					temp = "  ";
				break;
			case 9:
				if(!records->IsFieldNull(&records->m_WS))
					temp.Format(fmt, records->m_WS);
				else
					temp = "   ";
				break;
			case 10:
				if(!records->IsFieldNull(&records->m_WDir))
					temp.Format("%d", records->m_WDir);
				else
					temp = " ";
				break;
			case 11:
				if(!records->IsFieldNull(&records->m_WAzimuth))
					temp.Format(fmt, records->m_WAzimuth);
				else
					temp = "   ";
				break;
			case 12:
				if(!records->IsFieldNull(&records->m_TmpMax))
					temp.Format(fmt, records->m_TmpMax);
				else
					temp = "   ";
				break;
			case 13:
				if(!records->IsFieldNull(&records->m_TmpMin))
					temp.Format(fmt, records->m_TmpMin);
				else
					temp = "   ";
				break;
			case 14:
				if(!records->IsFieldNull(&records->m_RHMax))
					temp.Format(fmt, records->m_RHMax);
				else
					temp = "   ";
				break;
			case 15:
				if(!records->IsFieldNull(&records->m_RHMin))
					temp.Format(fmt, max(records->m_RHMin, 1));
				else
					temp = "   ";
				break;
			case 16:
				if(!records->IsFieldNull(&records->m_Season))
					temp.Format(fmt, records->m_Season);
				else
					temp = " ";
				break;
			case 17:
				if(!records->IsFieldNull(&records->m_OMC10))
					temp.Format(fmt, records->m_OMC10);
				else
					temp = "      ";
				break;
			case 18:
				if(!records->IsFieldNull(&records->m_GreenHerb))
					temp.Format(fmt, records->m_GreenHerb);
				else
					temp = "    ";
				break;
			case 19:
				if(!records->IsFieldNull(&records->m_GreenShrub))
					temp.Format(fmt, records->m_GreenShrub);
				else
					temp = "    ";
				break;
			case 20:
				if(!records->IsFieldNull(&records->m_WetFlag))
					temp.Format(fmt, records->m_WetFlag);
				else
					temp = "       ";
				break;
			case 21:
				if(!records->IsFieldNull(&records->m_SolarRadiation))
					temp.Format(fmt, records->m_SolarRadiation);
				else
					temp = "      ";
				break;
			case 22:
				if(!records->IsFieldNull(&records->m_GustSpd))
					temp.Format(fmt, records->m_GustSpd);
				else
					temp = "     ";
				break;
			case 23:
				if(!records->IsFieldNull(&records->m_GustDir))
					temp.Format(fmt, records->m_GustDir);
				else
					temp = "     ";
				break;
			case 24:
				if(!records->IsFieldNull(&records->m_HourlyPrecip))
					temp.Format(fmt, records->m_HourlyPrecip);
				else
					temp = "      ";
				break;
			case 25:
				if(!records->IsFieldNull(&records->m_SR_SOW))
					temp.Format(fmt, records->m_SR_SOW);
				else
					temp = "      ";
				break;
			case 26:
				if (!records->IsFieldNull(&records->m_SR_WetFlag))
					temp.Format(fmt, records->m_SR_WetFlag);
				else
					temp = "      ";
				break;
			case 27:
				if (!records->IsFieldNull(&records->m_SnowFlag))
					temp.Format(fmt, records->m_SnowFlag);
				else
					temp = "      ";
				break;
			case 28:
				if (!records->IsFieldNull(&records->m_FM1))
					temp.Format(fmt, records->m_FM1);
				else
					temp = "      ";
				break;
			case 29:
				if (!records->IsFieldNull(&records->m_FM10))
					temp.Format(fmt, records->m_FM10);
				else
					temp = "      ";
				break;
			case 30:
				if (!records->IsFieldNull(&records->m_FM100))
					temp.Format(fmt, records->m_FM100);
				else
					temp = "      ";
				break;
			case 31:
				if (!records->IsFieldNull(&records->m_FM1000))
					temp.Format(fmt, records->m_FM1000);
				else
					temp = "      ";
				break;
			case 32:
				if (!records->IsFieldNull(&records->m_FuelTemperature))
					temp.Format(fmt, records->m_FM1);
				else
					temp = "      ";
				break;
			default:
				temp = "";
			}
			buf += temp;
		}
		fprintf(out, "%s\n", buf);
		count++;
		records->MoveNext();
		buf = "";
	}

	records->SetAbsolutePosition(retRec + 1);
	fclose(out);
	chdir(theApp.dbDir);
	theApp.SetUserDir(fd.GetPathName());
	msg.Format("Export Complete!\n%ld records written to\n%s", count, outFileName);
	AfxMessageBox(msg);
	return 1;
}

void CWxExportDialog::OnDate1()
{
	dt = Full;
}

void CWxExportDialog::OnDate2()
{
	dt = Packed;
}

void CWxExportDialog::OnDate3()
{
	dt = Reverse;
}

void CWxExportDialog::OnDate4()
{
	dt = MD;
}

void CWxExportDialog::OnDate5()
{
	dt = MDPacked;
}

void CWxExportDialog::OnTime1()
{
	tt = FullTime;
}

void CWxExportDialog::OnTime2()
{
	tt = PackedTime;
}

void CWxExportDialog::OnBnClickedFlammapFARSITEStream()
{
	// TODO: Add your control notification handler code here
	SetAppearance();
	
}

void CWxExportDialog::OnBnClickedRadioHourlyPrecip()
{
	fw13PrecipType = 0;
}


void CWxExportDialog::OnBnClickedRadioDailyPrecip()
{
	fw13PrecipType = 1;
}


void CWxExportDialog::OnBnClickedRadioFw21()
{
	// TODO: Add your control notification handler code here
}

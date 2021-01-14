// FARSITEDialog.cpp : implementation file
//
 
#include "stdafx.h"
#include <afxrich.h>
#include "richdoc.h"
#include "fireplus.h"
#include "FARSITEDialog.h"
#include "SIGStationSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
extern double PotentialRadiation(int year, int month, int day,
            int hour, int minute, double lat, double lon, long elev, long slope, long aspect);
/////////////////////////////////////////////////////////////////////////////
// CFARSITEDialog dialog


CFARSITEDialog::CFARSITEDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet /*= NULL*/)
	: CDialog(CFARSITEDialog::IDD, pParent), wxSet(_fpSet->m_pDatabase)
{
	//{{AFX_DATA_INIT(CFARSITEDialog)
	bWnd = FALSE;
	//bWtr3 = FALSE;
	bWtr4 = FALSE;
	bWxs = FALSE;
	wndFileName = _T("");
	//wtr3FileName = _T("");
	wtr4FileName = _T("");
	wxsFileName = _T("");
	//}}AFX_DATA_INIT
	wx3 = wx4 = winds = wxs = NULL;
	windsType = 0;

	fpSet = _fpSet;
}


void CFARSITEDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFARSITEDialog)
	DDX_Control(pDX, IDC_STATIC3, wndStatic);
	DDX_Control(pDX, IDC_STATIC2, wtr4Static);
	//DDX_Control(pDX, IDC_STATIC1, wtr3Static);
	DDX_Control(pDX, IDC_WTR4FILEBUTTON, wtr4Button);
	//DDX_Control(pDX, IDC_WTR3FILEBUTTON, wtr3Button);
	DDX_Control(pDX, IDC_WNDFILEBUTTON, wndButton);
	DDX_Control(pDX, IDC_WTR4EDIT, wtr4Edit);
	DDX_Control(pDX, IDC_WXSFILEBUTTON, wxsButton);
	//DDX_Control(pDX, IDC_WTR3EDIT, wtr3Edit);
	DDX_Control(pDX, IDC_WNDEDIT, wndEdit);
	DDX_Check(pDX, IDC_WNDCHECK, bWnd);

	DDX_Check(pDX, IDC_FLAMMAPFARSITE_STREAM, bWxs);
	DDX_Control(pDX, IDC_RADIO1, windsAveButton);
	DDX_Control(pDX, IDC_RADIO2, windsGustButton);

	//DDX_Check(pDX, IDC_WTR3CHECK, bWtr3);
	DDX_Check(pDX, IDC_WTR4CHECK, bWtr4);
	DDX_Text(pDX, IDC_WNDEDIT, wndFileName);
	//DDX_Text(pDX, IDC_WTR3EDIT, wtr3FileName);
	DDX_Text(pDX, IDC_WTR4EDIT, wtr4FileName);
	DDX_Control(pDX, IDC_WXSEDIT, wxsEdit);
	DDX_Text(pDX, IDC_WXSEDIT, wxsFileName);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFARSITEDialog, CDialog)
	//{{AFX_MSG_MAP(CFARSITEDialog)
	ON_BN_CLICKED(IDC_WTR4CHECK, OnWtr4check)
	//ON_BN_CLICKED(IDC_WTR3CHECK, OnWtr3check)
	ON_BN_CLICKED(IDC_WNDCHECK, OnWndcheck)
	ON_BN_CLICKED(IDC_WNDFILEBUTTON, OnWndfilebutton)
	//ON_BN_CLICKED(IDC_WTR3FILEBUTTON, OnWtr3filebutton)
	ON_BN_CLICKED(IDC_WTR4FILEBUTTON, OnWtr4filebutton)
	ON_BN_CLICKED(IDC_WXSFILEBUTTON, OnWxsfilebutton)

	ON_BN_CLICKED(IDC_FLAMMAPFARSITE_STREAM, OnWxscheck)
	ON_BN_CLICKED(IDC_RADIO1, OnWxsAvecheck)
	ON_BN_CLICKED(IDC_RADIO2, OnWxsGustcheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFARSITEDialog message handlers

BOOL CFARSITEDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	EnableSelections();
	windsType = 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFARSITEDialog::OnOK() 
{
	UpdateData();
	// make sure something selected
	if(!bWtr4 && !bWnd && !bWxs)
	{
		AfxMessageBox("ERROR: At least one file type must be selected.");
		return;
	}
	if((bWtr4 && wtr4FileName.GetLength() == 0)
		|| (bWnd && wndFileName.GetLength() == 0)
		|| (bWxs && wxsFileName.GetLength() == 0))
	{
		AfxMessageBox("ERROR: A required file name is blank.");
		return;
	}
	if((bWnd && bWtr4 && wndFileName.CompareNoCase(wtr4FileName) == 0))
	{
		AfxMessageBox("ERROR: Identical names selected for output files.");
		return;
	}



	CWaitCursor wait;
	
	if(bWtr4)
	{
		wx4 = fopen(wtr4FileName, "wt");
		if(!wx4)
		{
			CString msg;
			msg.Format("ERROR:  Can not create output file:\n%s", wtr4FileName);
			AfxMessageBox(msg);
			if(wx3)
			{
				fclose(wx3);
				wx3 = NULL;
			}
			return;
		}
	}
	if(bWnd)
	{
		winds = fopen(wndFileName, "wt");
		if(!winds)
		{
			CString msg;
			msg.Format("ERROR:  Can not create output file:\n%s", wndFileName);
			AfxMessageBox(msg);
			if(wx3)
			{
				fclose(wx3);
				wx3 = NULL;
			}
			if(wx4)
			{
				fclose(wx4);
				wx4 = NULL;
			}
			return;
		}
	}

	if(bWxs)
	{
		wxs = fopen(wxsFileName, "wt");
		if(!wxs)
		{
			CString msg;
			msg.Format("ERROR:  Can not create output file:\n%s", wxsFileName);
			AfxMessageBox(msg);
			if(wxs)
			{
				fclose(wxs);
				wxs = NULL;
			}
			return;
		}
	}


	GenerateFiles();
	if(wx3)
		fclose(wx3);
	if(wx4)
		fclose(wx4);
	if(winds)
		fclose(winds);
	if(wxs)
		fclose(wxs);

	//now displayed generated files
	//now display the file in a child window
	POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	while(curTemplatePos != NULL)
	{
		CDocTemplate* curTemplate = 
			theApp.GetNextDocTemplate(curTemplatePos);
		CString str;
		curTemplate->GetDocString(str, CDocTemplate::docName);
		if(str == _T("TextFile"))
		{
			
			if(bWtr4)
			{
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(wtr4FileName);
				pDoc->isTemp = false;
				pDoc->SetTitle(wtr4FileName);
			//	pDoc->SetPathName(wtr4FileName, FALSE);
			}
			if(bWnd)
			{
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(wndFileName);
				pDoc->isTemp = false;
				pDoc->SetTitle(wndFileName);
			}


			if(bWxs)
			{
				CRichDoc *pDoc = (CRichDoc *)	curTemplate->OpenDocumentFile(wxsFileName);
				pDoc->isTemp = false;
				pDoc->SetTitle(wxsFileName);
			}
		}
	}
	CDialog::OnOK();
}

void CFARSITEDialog::EnableSelections()
{
	//wtr3Static.EnableWindow(bWtr3);
	//wtr3Button.EnableWindow(bWtr3);
	//wtr3Edit.EnableWindow(bWtr3);
	wtr4Static.EnableWindow(bWtr4);
	wtr4Button.EnableWindow(bWtr4);
	wtr4Edit.EnableWindow(bWtr4);
	wxsEdit.EnableWindow(bWxs);
	wndStatic.EnableWindow(bWnd);
	wndButton.EnableWindow(bWnd);
	wndEdit.EnableWindow(bWnd);


	wxsButton.EnableWindow(bWxs);
	windsAveButton.EnableWindow(bWxs);
	windsGustButton.EnableWindow(bWxs);
	if (bWxs)
	{
         
	      windsAveButton.SetCheck(! windsType);
		  windsGustButton.SetCheck(windsType);
		  
	}
	
}

void CFARSITEDialog::OnWtr4check() 
{
	UpdateData();
	EnableSelections();
}

//void CFARSITEDialog::OnWtr3check() 
//{
//	UpdateData();
//	EnableSelections();
//}

void CFARSITEDialog::OnWndcheck() 
{
	UpdateData();
	EnableSelections();
}

void CFARSITEDialog::OnWxsAvecheck()
{
	windsType = ! windsAveButton.GetCheck();
}

void CFARSITEDialog::OnWxsGustcheck()
{
	windsType = windsGustButton.GetCheck();
}

void CFARSITEDialog::OnWxscheck() 
{
	UpdateData();
	EnableSelections();
}

void CFARSITEDialog::OnWndfilebutton() 
{
	CFileDialog fd(FALSE, "wtr", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"FARSITE Wind Files (*.wnd)|*.wnd|All Files (*.*)|*.*||", this);
	if(fd.DoModal() == IDOK)
	{
		wndFileName = fd.GetPathName();
		UpdateData(FALSE);
	}
	
}
/*
void CFARSITEDialog::OnWtr3filebutton() 
{
	CFileDialog fd(FALSE, "wtr", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"FARSITE Weather Files (*.wtr)|*.wtr|All Files (*.*)|*.*||", this);
	if(fd.DoModal() == IDOK)
	{
		wtr3FileName = fd.GetPathName();
		UpdateData(FALSE);
	}
}
*/
void CFARSITEDialog::OnWtr4filebutton() 
{
	CFileDialog fd(FALSE, "wtr", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"FARSITE Weather Files (*.wtr)|*.wtr|All Files (*.*)|*.*||", this);
	if(fd.DoModal() == IDOK)
	{
		wtr4FileName = fd.GetPathName();
		UpdateData(FALSE);
	}
}

void CFARSITEDialog::OnWxsfilebutton() 
{
	CFileDialog fd(FALSE, "wxs", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Flammap / FARSITE Weather Stream (*.wxs)|*.wxs|All Files (*.*)|*.*||", this);
	if(fd.DoModal() == IDOK)
	{
		wxsFileName = fd.GetPathName();
		UpdateData(FALSE);
	}
}


void CFARSITEDialog::GenerateFiles()
{
	CString query = "", temp, staQuery;
	COleDateTimeSpan span(1, 0, 0, 0);
	fpSet->BuildBaseQuery(query, "ObsDate", false, false);
	if(query.GetLength() > 0)
		temp.Format(" and [StationID] = '%6.6s'", fpSet->m_SIG_Station);
	else
		temp.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	wxSet.m_strSort = _T("[ObsDate]");
	wxSet.m_strFilter.Format("(%s) %s", query, temp);
	wxSet.Open();
	if(wxSet.IsBOF() || wxSet.IsEOF())
	{
		CString strMsg;
		strMsg.Format(
			_T("ERROR: No matching weather observation records.\n Redefine Working Set Query. \n\n"));
		AfxMessageBox(strMsg);
		return;
	}
		//set up SIGStationSet
	CSIGStationSet staSet(fpSet->m_pDatabase);
	staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
	staSet.m_strFilter = _T(staQuery);
	staSet.Open();
	elev = staSet.m_Elevation;
	slope = 0;  
	aspect = 0;
	lat = staSet.m_LatDegrees;
	lon = staSet.m_LonDegrees;

	pcp = lastPcp = 0.0;
	GetLastPcp();
	bool init = true, newDay = true;
	COleDateTime yesterday, lastPcpTime;
	if (wxs) // write header
	{
		fprintf(wxs,"RAWS_ELEVATION: %ld\n", elev);
		if (windsType == 0)
			fprintf(wxs,"RAWS_WINDS: Ave\n");
		else
			fprintf(wxs,"RAWS_WINDS: Gust\n");

		fprintf(wxs,"RAWS_UNITS: English\n\n");
		fprintf(wxs, "Year  Mth  Day   Time    Temp     RH    HrlyPcp   WindSpd  WindDir CloudCov\n");
	}

	while(!wxSet.IsEOF())
	{
		if(wxSet.IsFieldNull(&wxSet.m_Temp) 
			|| wxSet.IsFieldNull(&wxSet.m_RH)
			|| wxSet.IsFieldNull(&wxSet.m_ObsDate))
		{
			wxSet.MoveNext();
			continue;
		}
		if(!init && curDay >= wxSet.m_ObsDate)
		{//duplicates allowed in database!!!!(GRRRRRRRRRRRR)
			wxSet.MoveNext();
			continue;
		}
		if(init)
		{
			curDay = wxSet.m_ObsDate;
		}

		if (wxs)
			WriteWxs();

		//fill in missing data w/same as last day of data
		while(DayOfYear(curDay) != DayOfYear(wxSet.m_ObsDate))
		{
			if(wx3)
				WriteWx3();
			if(wx4)
				WriteWx4();
			curDay += span;
			newDay = true;
		}
		curDay = wxSet.m_ObsDate;
		if(winds)
			WriteWinds();
		if(newDay)
		{
			minHr = maxHr = wxSet.m_ObsDate.GetHour() * 100 + wxSet.m_ObsDate.GetMinute();
			maxT = minT = wxSet.m_Temp;
			maxRH = minRH = min(wxSet.m_RH, 99);
			maxRH = max(maxRH, 1);
			minRH = max(minRH, 1);
			pcp = 0.0;
			pcpStart = wxSet.m_ObsDate + span;
			pcpEnd = wxSet.m_ObsDate - span;
			if(!wxSet.IsFieldNull(&wxSet.m_PPTAMT))
			{
				if(wxSet.m_PPTAMT > lastPcp)
				{
					//CTime tTime(wxSet.m_ObsDate.GetYear(), wxSet.m_ObsDate.GetMonth(), wxSet.m_ObsDate.GetDay(),
					//	0, 0, 0);
					pcpStart = wxSet.m_ObsDate;
					//pcpStart.SetTime(0, 0, 0);//lastPcpTime;
					pcpEnd = /*lastPcpTime =*/ wxSet.m_ObsDate;
					pcp = wxSet.m_PPTAMT - lastPcp;
				}
				lastPcp = wxSet.m_PPTAMT;
			}
		}
		else
		{
			if(wxSet.m_Temp < minT)
			{
				minT = wxSet.m_Temp;
				minHr = wxSet.m_ObsDate.GetHour() * 100 + wxSet.m_ObsDate.GetMinute();
			}
			if(wxSet.m_Temp > maxT)
			{
				maxT = wxSet.m_Temp;
				maxHr = wxSet.m_ObsDate.GetHour() * 100 + wxSet.m_ObsDate.GetMinute();
			}
			maxRH = max(maxRH, min(wxSet.m_RH, 99));
			minRH = min(minRH, min(wxSet.m_RH, 99));
			if(!wxSet.IsFieldNull(&wxSet.m_PPTAMT))
			{
				if(wxSet.m_PPTAMT > lastPcp)
				{
					if(pcp == 0)
						pcpStart = lastPcpTime;
					pcp += wxSet.m_PPTAMT - lastPcp;
					pcpEnd = /*lastPcpTime =*/ wxSet.m_ObsDate;
				}
				lastPcp = wxSet.m_PPTAMT;
			}
		}
		lastPcpTime = wxSet.m_ObsDate;
		init = false;
		wxSet.MoveNext();
		if(DayOfYear(wxSet.m_ObsDate) != DayOfYear(curDay))
			newDay = true;
		else
			newDay = false;
	}
	if(wx3)
		WriteWx3();
	if(wx4)
		WriteWx4();
	

	wxSet.Close();
	staSet.Close();
}

void CFARSITEDialog::WriteWx3()
{
	if(wx3)
		fprintf(wx3, "%d %d %.0f %d %d %d %d %d %d %d\n",
			curDay.GetMonth(), curDay.GetDay(), pcp * 100.0, 
			minHr, maxHr, minT, maxT, maxRH, minRH, elev);
}

void CFARSITEDialog::WriteWxs()
{

	char buf[128], temp[64];

	if(wxs)
	{
	
	    long ws = 0, wd = 0;

		if (windsType == 0)
		{
			ws = (wxSet.IsFieldNull(&wxSet.m_WS) ? -9999 : wxSet.m_WS);
			wd = (wxSet.IsFieldNull(&wxSet.m_WAzimuth) ? -9999 : wxSet.m_WAzimuth);
		} else 
		{
			ws = (wxSet.IsFieldNull(&wxSet.m_GustSpd) ? -9999 : wxSet.m_GustSpd);
			wd = (wxSet.IsFieldNull(&wxSet.m_GustDir) ? -9999 : wxSet.m_GustDir);
		}

	fprintf(wxs, "%4d   %2d   %2d   %04d    %4d   %4d   %8.2f   %6d  %6d",
		//curDay.GetYear(),
		//curDay.GetMonth(),
		//curDay.GetDay(),
		//curDay.GetHour() * 100,
		wxSet.m_ObsDate.GetYear(),
		wxSet.m_ObsDate.GetMonth(),
		wxSet.m_ObsDate.GetDay(),
		wxSet.m_ObsDate.GetHour() * 100,
		(wxSet.IsFieldNull(&wxSet.m_Temp) ? -9999 : wxSet.m_Temp),
			(wxSet.IsFieldNull(&wxSet.m_RH) ? -9999 : max(wxSet.m_RH, 1)),
			(wxSet.IsFieldNull(&wxSet.m_HourlyPrecip) ? -9999 : wxSet.m_HourlyPrecip),
			ws,
			wd
			);

		//compute cloud cover
		int cc = 0;
		if(wxSet.IsFieldNull(&wxSet.m_SolarRadiation))
		{
			if(wxSet.m_HourlyPrecip > 0)
				cc = 100;
		}
		else
		{
			if(wxSet.m_HourlyPrecip > 0.0)
				cc = 100;
			else
			{
				double potSolRad = PotentialRadiation(wxSet.m_ObsDate.GetYear(),
					wxSet.m_ObsDate.GetMonth(),
					wxSet.m_ObsDate.GetDay(),
					wxSet.m_ObsDate.GetHour(),
					wxSet.m_ObsDate.GetMinute(),
					lat,
					lon,
					elev,
					slope,
					aspect);

				double pctSolRad = 100;
				if(potSolRad > 100)
					pctSolRad = (double)wxSet.m_SolarRadiation / potSolRad * 100.0;
				cc = 100 - pctSolRad;
				cc = max(0, cc);
				cc = min(100, cc);
				//TRACE3("Potential = %.0f, Observed = %d, cc = %d\n",
				//	potSolRad, records->m_SolarRadiation, cc);
			}
		}
		sprintf(temp, "  %4d", cc);
	
		fprintf(wxs, "%s\n", temp);

	}

}


void CFARSITEDialog::WriteWx4()
{
	if(wx4)
	{
		if(pcp > 0.0)
		{
			if(pcpStart > pcpEnd)
			{
				COleDateTimeSpan fifteenMins(0, 0, 15, 0);
				COleDateTime tTime(pcpStart.GetYear(), pcpStart.GetMonth(), pcpStart.GetDay(), 0, 0, 0);
				pcpStart = tTime;//.SetTime(0, 0, 0);
				if(pcpStart + fifteenMins > pcpEnd)
					pcpEnd = pcpStart + fifteenMins;
			}
			fprintf(wx4, "%d %d %.0f %d %d %d %d %d %d %d %d %d\n",
			curDay.GetMonth(), curDay.GetDay(), pcp * 100.0, 
			minHr, maxHr, minT, maxT, maxRH, minRH, elev, 
			pcpStart.GetHour() * 100 + pcpStart.GetMinute(), 
			pcpEnd.GetHour() * 100 + pcpEnd.GetMinute());
		}
		else
			fprintf(wx4, "%d %d %.0f %d %d %d %d %d %d %d\n",
			curDay.GetMonth(), curDay.GetDay(), pcp * 100.0, 
			minHr, maxHr, minT, maxT, maxRH, minRH, elev);
	}
}

void CFARSITEDialog::WriteWinds()
{
	if(winds)
	{
		double stnLat = 0, stnLon = 0; 
		long stnSlope = 0, stnAspect = 0, stnElev = 0;	
		CSIGStationSet staSet(wxSet.m_pDatabase);
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", wxSet.m_StationID);
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
		int wdir;
		if(!wxSet.IsFieldNull(&wxSet.m_WAzimuth))
			wdir = wxSet.m_WAzimuth;
		else if(!wxSet.IsFieldNull(&wxSet.m_WDir))
		{
			switch(wxSet.m_WDir)
			{
			case 0:
			case 1:
				wdir = 0;
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
				wdir = (wxSet.m_WDir - 1)* 45;
				break;
			default:
				wdir = 0;
			}
		}
		else
			wdir = 0;
		//compute cloud cover
		int cc = 0;
		if(wxSet.IsFieldNull(&wxSet.m_SolarRadiation))
		{
			if(wxSet.m_HourlyPrecip > 0)
				cc = 100;
		}
		else
		{
			if(wxSet.m_HourlyPrecip > 0.0)
				cc = 100;
			else
			{
				double potSolRad = PotentialRadiation(wxSet.m_ObsDate.GetYear(),
					wxSet.m_ObsDate.GetMonth(),
					wxSet.m_ObsDate.GetDay(),
					wxSet.m_ObsDate.GetHour(),
					wxSet.m_ObsDate.GetMinute(),
					stnLat,
					stnLon,
					stnElev,
					stnSlope,
					stnAspect);
				double pctSolRad = 100;
				if(potSolRad > 100)
					pctSolRad = (double)wxSet.m_SolarRadiation / potSolRad * 100.0;
				cc = 100 - pctSolRad;
				cc = max(0, cc);
				cc = min(100, cc);
				//TRACE3("Potential = %.0f, Observed = %d, cc = %d\n",
//					potSolRad, wxSet.m_SolarRadiation, cc);
			}
		}
		fprintf(winds, "%d %d %d %d %d %d\n", 
			wxSet.m_ObsDate.GetMonth(), 
			wxSet.m_ObsDate.GetDay(),
			wxSet.m_ObsDate.GetHour() * 100 + wxSet.m_ObsDate.GetMinute(), 
			wxSet.IsFieldNull(&wxSet.m_WS) ? 0 : wxSet.m_WS, 
			wdir,
			cc);
	}
}

void CFARSITEDialog::GetLastPcp()
{
	lastPcp = 0.0;
	CWxSet tSet(fpSet->m_pDatabase);
	tSet.m_strFilter.Format("[StationID] = '%6.6s' and [ObsDate] < #%d/%d/%d#",
		fpSet->m_SIG_Station, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay, fpSet->m_StartYear);
	tSet.m_strSort = "[ObsDate]";
	tSet.Open();
	if(!tSet.IsEOF())
	{
		tSet.MoveLast();
		if(!tSet.IsFieldNull(&tSet.m_PPTAMT))
			lastPcp = tSet.m_PPTAMT;
	}
	tSet.Close();
}

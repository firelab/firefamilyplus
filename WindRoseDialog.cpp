// WindRoseDialog.cpp : implementation file
// 
#include "stdafx.h"
#include "fireplus.h"
#include "wxset.h"
#include "stationinsigset.h"
#include "runsset.h"
#include "runsave.h"
#include "reportoptionsset.h"
#include <afxrich.h>
#include "richView.h"
#include "richDoc.h"
#include "mainfrm.h"
#include "WindRoseDoc.h"
#include "WindRoseView.h"
#include "WindRoseFrame.h"
#include "WindRoseDialog.h"
#include "SigStationSet.h"
#include "StationInSIGSet.h"
#include ".\windrosedialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern int curr_runID;
extern int daysInMonth[];
extern CFireplusApp theApp;
extern char *abbrevs[];
extern char *varFormats[];
extern int MonthChtoInt(CString chMonth);


/////////////////////////////////////////////////////////////////////////////
// CWindRoseDialog dialog


CWindRoseDialog::CWindRoseDialog(CWnd* pParent /*=NULL*/, CFireplusSet *_fpSet /*=NULL*/)
	: CDialog(CWindRoseDialog::IDD, pParent)
{
	fpSet = _fpSet;


	incGraph = true;
	incTable = true;
	incWinds = true;
    CRunSave saveRun(fpSet);
	CString value1;
	// populate dialog checkboxes based on previous run setting
    int loadID = 0;
	int prevSave = saveRun.RestoreRunOption(loadID,REPORT_WEATHER_WINDS,0,"wind_0_4",&value1,fpSet);
	if (prevSave){
		wind_0_4 = atoi(value1);
        
		saveRun.RestoreRunOption(loadID,REPORT_WEATHER_WINDS,0,"wind_4_8",&value1,fpSet);
        wind_4_8 = atoi(value1);

		saveRun.RestoreRunOption(loadID,REPORT_WEATHER_WINDS,0,"wind_8_12",&value1,fpSet);
        wind_8_12 = atoi(value1);

		saveRun.RestoreRunOption(loadID,REPORT_WEATHER_WINDS,0,"wind_12_16",&value1,fpSet);
        wind_12_16 = atoi(value1);

		saveRun.RestoreRunOption(loadID,REPORT_WEATHER_WINDS,0,"wind_16_20",&value1,fpSet);
        wind_16_20 = atoi(value1);

		saveRun.RestoreRunOption(loadID,REPORT_WEATHER_WINDS,0,"wind_20_0",&value1,fpSet);
        wind_20_0 = atoi(value1);

		saveRun.RestoreRunOption(loadID,REPORT_WEATHER_WINDS,0,"splitGraphs",&value1,fpSet);
        splitGraphs = atoi(value1);

		saveRun.RestoreRunOption(loadID,REPORT_WEATHER_WINDS,0,"windsType",&value1,fpSet);
        windsType = atoi(value1);

	} else {
	   wind_0_4 = true;
	   wind_4_8 = true;
	   wind_8_12 = true;
	   wind_12_16 = true;
	   wind_16_20 = true;
	   wind_20_0 = true;
	   splitGraphs = true;
	   windsType = 0;

	}
	
	//{{AFX_DATA_INIT(CWindRoseDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}



void CWindRoseDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWindRoseDialog)
	//DDX_Check(pDX, IDC_WIND_ROSE_GRAPH, incGraph);
    //DDX_Check(pDX, IDC_WIND_ROSE_PERCENTAGE, incTable);
	//DDX_Check(pDX, IDC_WIND_ROSE_WINDS, incWinds);

	DDX_Check(pDX, IDC_WINDS_0_4, wind_0_4);
    DDX_Check(pDX, IDC_WINDS_4_8, wind_4_8);
	DDX_Check(pDX, IDC_WINDS_8_12, wind_8_12);
	DDX_Check(pDX, IDC_WINDS_12_16, wind_12_16);
	DDX_Check(pDX, IDC_WINDS_16_20, wind_16_20);
	DDX_Check(pDX, IDC_WINDS_20_0, wind_20_0);

	DDX_Check(pDX, IDC_WINDROSE_SPLIT, splitGraphs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWindRoseDialog, CDialog)
	//{{AFX_MSG_MAP(CWindRoseDialog)
	
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_WINDS_4_8, OnBnClickedWinds48)
	ON_BN_CLICKED(IDC_WINDS_0_4, OnBnClickedWinds04)

	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedGust)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedAvg)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedBoth)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWindRoseDialog message handlers


void CWindRoseDialog::OnBnClickedGust()
{
	windsType = 1;
}

void CWindRoseDialog::OnBnClickedAvg()
{
	windsType = 0;
}

void CWindRoseDialog::OnBnClickedBoth()
{
	windsType = 2;
}


BOOL CWindRoseDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	EnableToolTips(TRUE);

	if (windsType==0)
		((CButton *)(GetDlgItem(IDC_RADIO1)))->SetCheck(TRUE);
	else if (windsType==1)
		((CButton *)(GetDlgItem(IDC_RADIO2)))->SetCheck(TRUE);
	else if (windsType==2)
		((CButton *)(GetDlgItem(IDC_RADIO3)))->SetCheck(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CWindRoseDialog::OnOK()
{
    UpdateData();

	if (! incGraph && !incTable && !incWinds){
		CDialog::OnOK();
		return;
	}

	// added for batch
	// single run mode

	CString newDesc;
	CRunSave saveRun(fpSet);
	saveRun.SaveGeneral(0, "Winds" , fpSet,&newDesc);

	if (newDesc.GetLength() < 2)
		newDesc.Format("%s",fpSet->m_SIG_Station);

	saveRun.SaveClimateOptions(curr_runID, fpSet);
	// removed 8/2006 saveRun.SaveWxStation(curr_runID, fpSet);
	saveRun.SaveFireOptions(curr_runID, fpSet);
	CString value1;


	int saveID = curr_runID;

	// save initial dialog values
	value1.Format("%1d", wind_0_4);
	saveRun.SaveRunOption(saveID,REPORT_WEATHER_WINDS,0,"wind_0_4",value1, fpSet);
    value1.Format("%1d", wind_4_8);
	saveRun.SaveRunOption(saveID,REPORT_WEATHER_WINDS,0,"wind_4_8",value1, fpSet);
    value1.Format("%1d", wind_8_12);
	saveRun.SaveRunOption(saveID,REPORT_WEATHER_WINDS,0,"wind_8_12",value1, fpSet);
    value1.Format("%1d", wind_12_16);
	saveRun.SaveRunOption(saveID,REPORT_WEATHER_WINDS,0,"wind_12_16",value1, fpSet);
    value1.Format("%1d", wind_16_20);
	saveRun.SaveRunOption(saveID,REPORT_WEATHER_WINDS,0,"wind_16_20",value1, fpSet);
    value1.Format("%1d", wind_20_0);
	saveRun.SaveRunOption(saveID,REPORT_WEATHER_WINDS,0,"wind_20_0",value1, fpSet);

	value1.Format("%1d", splitGraphs);
	saveRun.SaveRunOption(saveID,REPORT_WEATHER_WINDS,0,"splitGraphs",value1, fpSet);

	value1.Format("%1d", windsType);
	saveRun.SaveRunOption(saveID,REPORT_WEATHER_WINDS,0,"windsType",value1, fpSet);


	// end added for batch
	
	CWaitCursor wait;
	bool isSIG = false;
	long dirSpd[366][10][7];
	double speeds[366][10];



    int WRSpd[WRDIRS+1][WRSPEEDS+1][WRHOURS+1];
	double WRspeeds[WRDIRS+1][WRHOURS+1];


	int d;
	
	for(d = 0; d < 366; d++)
	{   
		int i=0;

		// old ffp3 bins
		for(i = 0; i < 10; i++)
		{
			speeds[d][i] = 0.0;
			for(int j = 0; j < 7; j++)
				dirSpd[d][i][j] = 0L;
		}

		
	}

	long recs = 0; // old ffp3
	long WRrecs[WRHOURS+1]; // new windrose

	// new windrose bins
  
	for (int k=0;k<=WRHOURS;k++){
	   WRrecs[k] = 0;
       for (int i=0;i<WRDIRS+1;i++){
            WRspeeds[i][k] = 0.0;
			for (int j=0;j<WRSPEEDS+1;j++)
				WRSpd[i][j][k] = 0L;
	   }
   	}

	CString query, tStr;
	fpSet->BuildBaseQuery(query, "ObsDate", false);
	//m_ctlSIG_Station.GetWindowText(tStr);
	tStr = fpSet->m_SIG_Station;
	tStr.TrimRight();
	CStationInSIGSet sSet(fpSet->m_pDatabase);
	char sigName[64];
	if(tStr.GetLength() > 6)
	{
		isSIG = true;
		query += " and (";
		strcpy(sigName, tStr);
		sSet.m_strFilter.Format("[SIG] = '%s'", &sigName[6]);
		sSet.Open();
		int count = 0;
		while(!sSet.IsEOF())
		{
			if(!count)
				tStr.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			else
				tStr.Format(" or [StationID] = '%6.6s'", sSet.m_StationID);
			query += tStr;
			count++;
			sSet.MoveNext();
		}
		query += ")";
		//sSet.Close();
	}
	else
	{
		CString sta;
		sta.Format(" and [StationID] = '%6.6s'", tStr);
		query += sta;
	}


    EnableWindow(FALSE);


	/*CCancelDialog *cd = NULL;
	bool noCancel = false;


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
	     cd->SetStatusString("Processing Working Set");
		cd->SetTitle("Reading Weather Data");
	  }
	}*/


	


	

	CWxSet wxSet(fpSet->m_pDatabase);
	wxSet.m_strFilter = query;
	wxSet.m_strSort = "[ObsDate]";
	wxSet.Open();

	// counters / flags to determine missing data
	
    int days = 0, lastDay = -1;
    int dir16day = 0, dir16count = 0;


	// new 2014: windsType 0: average   1:  gust    2: both


	int numModes = 0;
	int mode[2];

#define MODE_AVE	0
#define MODE_GUST	1

	mode[0] = MODE_AVE;    // first pass for speeds
	mode[1] = MODE_GUST;    // second pass for gusts

	if (windsType == 2)
	{
		numModes = 2;   // two passes for 'both'
	}
	else numModes = 1;

	if (windsType == 1)
		mode[0] = MODE_GUST;      // first&only pass for gusts

	

	int iHourBin = 0;   // hourly bin
	while(!wxSet.IsEOF())//  && (!cd || !cd->Abort() ))
	{
		if(!wxSet.IsFieldNull(&wxSet.m_WS))
		{
			//first things first... make sure record in filter
			int iHour = wxSet.m_ObsDate.GetHour();
			iHourBin = (int ) (iHour / 4);
			d = DayOfYear(wxSet.m_ObsDate) - 1;
			if (d != lastDay)// && lastDay != -1)
			{
				dir16count += dir16day;
				lastDay = d;
				dir16day = 0;
				days ++;
			}
			if(!dir16day)// && wxSet.m_ObsType != "O")
				dir16day = 1;
			bool recIsNeeded = false;
			switch(iHourBin)
			{
			case 0:
				if(wind_0_4)
					recIsNeeded = true;
				break;
			case 1:
				if(wind_4_8)
					recIsNeeded = true;
				break;
			case 2:
				if(wind_8_12)
					recIsNeeded = true;
				break;
			case 3:
				if(wind_12_16)
					recIsNeeded = true;
				break;
			case 4:
				if(wind_16_20)
					recIsNeeded = true;
				break;
			case 5:
				if(wind_20_0)
					recIsNeeded = true;
				break;
			}
			if(!recIsNeeded)
			{
				//if (cd)
				//   cd->Increment();
				wxSet.MoveNext();
				continue;
			}
			for (int iMode=0; iMode < numModes; iMode++)
			{
				int WRdir = -1;
				int dir = -1;
				if (mode[iMode]==MODE_AVE && !wxSet.IsFieldNull(&wxSet.m_WDir))
					dir = wxSet.m_WDir;
				else if (mode[iMode]==MODE_GUST && !wxSet.IsFieldNull(&wxSet.m_GustDir))
					dir = WRdir = (int) ((wxSet.m_GustDir + 11.25) / 22.5);   // gusts are already 0-360 degrees

				

				if (mode[iMode] == MODE_AVE)
				{
					if(!wxSet.IsFieldNull(&wxSet.m_WAzimuth))
					{
						if ((wxSet.m_WAzimuth >= 0) && (wxSet.m_WAzimuth <= 360))
							WRdir = (int) ((wxSet.m_WAzimuth + 11.25) / 22.5);
						else
							WRdir = (wxSet.m_WDir * 2 - 2);
					}

					if(WRdir == -1 && dir == -1)
					{
						//if (cd)
						//    cd->Increment();
						//wxSet.MoveNext();
						continue;
					}
					/***********************************************************
					Ask Luke about records with dir = 0
					****************************************************/
					else if(WRdir == -1)
					{
						WRdir = (wxSet.m_WDir * 2 - 2);
						if(WRdir < 0)
							WRdir = 0;
					}
				} else {  // gusts
					if (WRdir < 0 )
					{
						//if (cd)
						//   cd->Increment();
						continue;
					}
					
				}

				int ws = -1;
				
				if (mode[iMode]==MODE_AVE)
					ws = wxSet.m_WS;
				else
					ws = wxSet.m_GustSpd;

				if(ws > 0)
				{
					if((mode[iMode]==MODE_AVE && !wxSet.IsFieldNull(&wxSet.m_WDir)) ||
					   (mode[iMode]==MODE_GUST && !wxSet.IsFieldNull(&wxSet.m_GustDir)))
					{
						//int dir = wxSet.m_WDir;
						bool isCalm = false;

						int wsLoc = 0;  // old FFP3 speed bin
						int WRLoc = 0;  // new windrose speed bin
						// old ffp3 
						if(dir > 0)
						{
							if(ws <= 3 && (dir <= 0 || dir >= 9))
							{
								dir = 9;
								wsLoc = 0;
							}
							else if((dir > 0 || dir < 9))
							{
								if(ws < 4)
									wsLoc = 0;
								else if(ws < 8)
									wsLoc = 1;
								else if(ws < 13)
									wsLoc = 2;
								else if(ws < 19)
									wsLoc = 3;
								else if(ws < 24)
									wsLoc = 4;
								else
									wsLoc = 5;
								dirSpd[d][dir - 1][wsLoc]++;
								dirSpd[d][dir - 1][6]++;
								dirSpd[d][9][wsLoc]++;
								dirSpd[d][9][6]++;
								speeds[d][dir - 1] += ws;
								speeds[d][9] += ws;
								recs++;
							}
							//TRACE3("%d %d %d\n", d, dir, wsLoc);
						}
					}
				}
				else //calm
				{
					dirSpd[d][8][0]++;
					dirSpd[d][8][6]++;
					dirSpd[d][9][0]++;
					dirSpd[d][9][6]++;
					recs++;
				}
				// new windrose speed bins
				//int WRdir = 0;

				/*if ((wxSet.m_WAzimuth >= 0) && (wxSet.m_WAzimuth <= 360))
				WRdir = (int) ((wxSet.m_WAzimuth + 11.25) / 22.5);
				else
				WRdir = (wxSet.m_WDir * 2 - 2);*/
				// is it a "base 16" dir rather than "base 8" ??
				if (WRdir > 0)
					//if (((int) (WRdir/2) * 2)  != WRdir)
					//{
					//if (!dir16day)
					// {
					//   dir16day = TRUE;
					//   dir16count ++;
					//}

					//}

					if (WRdir >= WRDIRS)
						WRdir = 0;

				int WRLoc = 0;

				if (ws < 1)
					WRLoc = 0;
				else if (ws < 4)
					WRLoc = 1;
				else if (ws < 8)
					WRLoc = 2;
				else if (ws < 13)
					WRLoc = 3;
				else if (ws < 19)
					WRLoc = 4;
				else if (ws < 25)
					WRLoc = 5;
				else if (ws < 32)
					WRLoc = 6;
				else if (ws < 39)
					WRLoc = 7;
				else if (ws < 47)
					WRLoc = 8;
				else
					WRLoc = 9;
				//if(WRdir < 0 || WRdir > 16)
				//	AfxMessageBox("heh");
				//TRACE1("%d\n", WRdir);
				if(WRdir >= 0)
				{
					WRSpd[WRdir][WRLoc][iHourBin]++ ;  // counter
					WRSpd[WRdir][WRSPEEDS][iHourBin] ++;   // catchall - all speeds for this dir
					WRspeeds[WRdir][iHourBin] += ws;  // total
					WRrecs[iHourBin]++;

					// catchall
					WRSpd[WRdir][WRLoc][WRHOURS]++ ;  // counter
					WRSpd[WRdir][WRSPEEDS][WRHOURS] ++;   // catchall - all speeds for this dir
					WRspeeds[WRdir][WRHOURS] += ws;  // total
					WRrecs[WRHOURS]++;
				}
			}

		}
		lastDay = d;
		//if (cd)
		//   cd->Increment();
		wxSet.MoveNext();
	}
	wxSet.Close();
	dir16count += dir16day;
	/*if (cd)
	   cd->SetStatusString("Analyzing Winds Data");
	if(cd && cd->Abort())
	{
		cd->DestroyWindow();
		EnableWindow(TRUE);
		return;
	}*/
	char *fName = GetTempFileName();
	FILE *stream = fopen(fName, "wt");

    char *fName2 = GetTempFileName();
	

	FILE *stream2 = fopen(fName2, "wt");

	CString tempStr;

	tempStr.Format("%s - Frequency Table\n\n",newDesc);
	fprintf(stream2, "%s",tempStr);

    tempStr.Format("%s - Wind Speed vs. Direction Report\n\n",newDesc);
	fprintf(stream, "%s",tempStr);

	// added for batch
	// (code interspersed)
	CReportOptionsSet reportSet(fpSet->m_pDatabase);
	reportSet.Open();

	CString dStr;


	
	COleDateTime dateTime = COleDateTime::GetCurrentTime();

	dStr = dateTime.Format("%m/%d/%Y   at  %I:%M:%S %p");
	fprintf(stream,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());
	fprintf(stream,"\n");

	fprintf(stream2,"   printed on: %s  (from run # %d)\n",  dStr, curr_runID);
	fprintf(stream2,"   using database: %s\n", fpSet->m_pDatabase->GetDatabaseName());
	fprintf(stream2,"\n");

 

	CString headerStr;

	// add report headers (as needed)
	reportSet.listAll(&headerStr,fpSet);
	fprintf(stream,"%s",headerStr);
	fprintf(stream2,"%s",headerStr);
	reportSet.Close();

	float totPercRow = 0,
	 	  totPercCalm = 0,
		  totPercCol[WRDIRS + 1];

		
    float maxPerc[WRHOURS+1];
    double percs[WRDIRS+1][WRSPEEDS+1][WRHOURS+1];
	for (int k=0;k<=WRHOURS;k++){
		// only disply individual bins if split checkbox = true 
       maxPerc[k] = 0;

	   if (! splitGraphs)
			if (k < WRHOURS)
				continue;
	   fprintf(stream2,"\n\n");

	   switch (k) {
		  case 0:
             if (! wind_0_4)
			     continue;
		     else
			     fprintf(stream2,"                          Hourly Period: 0000 - 0300 hours\n\n");
             break;
          case 1:
             if (! wind_4_8)
			     continue;
		     else
			     fprintf(stream2,"                          Hourly Period: 0400 - 0700 hours\n\n");
             break;
		  case 2:
             if (! wind_8_12)
			     continue;
		     else
			     fprintf(stream2,"                          Hourly Period: 0800 - 1100 hours\n\n");
             break;
		 case 3:
             if (! wind_12_16)
			     continue;
		     else
			     fprintf(stream2,"                          Hourly Period: 1200 - 1500 hours (and daily observations)\n\n");
             break;
		  case 4:
             if (! wind_16_20)
			     continue;
		     else
			     fprintf(stream2,"                          Hourly Period: 1600 - 1900 hours\n\n");
             break;		  
		  case 5:
             if (! wind_20_0)
			     continue;
		     else
			     fprintf(stream2,"                          Hourly Period: 2000 - 2300 hours\n\n");
             break;
		  case 6:
			  if (wind_0_4 && wind_4_8 && wind_8_12 && wind_12_16 && wind_16_20 && wind_20_0){
				 fprintf(stream2,"                             Total of all hourly periods\n\n");
			  } else {
				 fprintf(stream2,"                          Combined Hours: ");
				 if (wind_0_4)
					 fprintf(stream2,"0-3, ");
 				 if (wind_4_8)
					 fprintf(stream2,"4-7, ");
 				 if (wind_8_12)
					 fprintf(stream2,"8-11, ");
 				 if (wind_12_16)
					 fprintf(stream2,"12-15, ");
 				 if (wind_16_20)
					 fprintf(stream2,"16-19, ");
 				 if (wind_20_0)
					 fprintf(stream2,"20-23");
				 fprintf(stream2,"\n\n");
						  

			   }
			   
			 break;
	   }

       fprintf(stream2,"  MPH      ____________________________________ Direction _______________________________\n"); 
       fprintf(stream2," Range     N    NNE  NE   ENE  E    ESE  SE   SSE  S    SSW  SW   WSW  W    WNW  NW   NNW  Total\n");
     
       for (int j=0;j< WRDIRS + 1;j++)
		  totPercCol[j] = 0.0;
 

	   for (int i=1;i<WRSPEEDS;i++){
          totPercRow = 0.0;

		  switch(i){
		   case 1:
			   fprintf(stream2,"  1-4    "); break;
		   case 2:
			   fprintf(stream2,"  4-8    "); break;
		   case 3:
			   fprintf(stream2,"  8-13   "); break;
		   case 4:
			   fprintf(stream2," 13-19   "); break;
		   case 5:
			   fprintf(stream2," 19-25   "); break;
		   case 6:
			   fprintf(stream2," 25-32   "); break;
		   case 7:
			   fprintf(stream2," 32-39   "); break;
		   case 8:
			   fprintf(stream2," 39-47   "); break;
		   case 9:
			   fprintf(stream2," 47 +    "); break;

		  }
		  for (int j=0;j<WRDIRS;j++){
			if (WRrecs[k] > 0){
               fprintf(stream2,"%5.1f",(float) (WRSpd[j][i][k]) / (float) (WRrecs[k]) * 100.0);
			   totPercRow += (float) (WRSpd[j][i][k]) / (float) (WRrecs[k]) * 100.0;
               totPercCol[j] += (float) (WRSpd[j][i][k]) / (float) (WRrecs[k]) * 100.0;
               totPercCol[WRDIRS] += (float) (WRSpd[j][i][k]) / (float) (WRrecs[k]) * 100.0;
			   percs[j][i][k] = (double) (WRSpd[j][i][k]) / (double) (WRrecs[k]) * 100.0;
             
			} else {
			   fprintf(stream2,"%5.1f", 0.0);
			   percs[j][i][k] = 0.0;
			}
		  }

		

		  fprintf(stream2," %5.1f",totPercRow);
		  fprintf(stream2,"\n");


	   }

       fprintf(stream2,"\nTotal (%%)");
	   for (int j=0;j<WRDIRS;j++){
           fprintf(stream2,"%5.1f",totPercCol[j]);

	   }
       fprintf(stream2," %5.1f",totPercCol[WRDIRS]);
       fprintf(stream2,"\n");

   	   fprintf(stream2,"\nCalm (<1) ");
	   for (int j=0;j<WRDIRS;j++){
		  if (totPercCol[j] > maxPerc[k])
			     maxPerc[k] = totPercCol[j];
		  if (WRrecs[k] > 0)
    		    totPercCalm += (float) (WRSpd[j][0][k]) / (float) (WRrecs[k]) * 100.0;
		  else
			    totPercCalm = 100.0;
		  fprintf(stream2,"     ");
	   }

        fprintf(stream2,"%5.1f",100.0 - totPercCol[WRDIRS]   );
        fprintf(stream2,"\n");

	    fprintf(stream2,"\nAve Speed");
	    float aveCol = 0,
		    totSpd = 0;

	    for (int j=0;j<WRDIRS;j++){
		   if (WRSpd[j][WRSPEEDS][k] > 0)
   		      aveCol = WRspeeds[j][k] / WRSpd[j][WRSPEEDS][k];
		   else
		      aveCol = 0.0;
		   totSpd += WRspeeds[j][k];
		   fprintf(stream2,"%5.1f", aveCol);
	    }
	   if (WRrecs[k] > 0)
          fprintf(stream2," %5.1f",totSpd / WRrecs[k]);
	   else
          fprintf(stream2," %5.1f",0.0);
       fprintf(stream2,"\n (MPH)\n");

	   if (windsType == 0)
		   fprintf(stream2,"\nWind type: 10-minute average\n");
	   else if (windsType == 1)
		    fprintf(stream2,"\nWind type: Gusts\n");
	   else if (windsType == 2)
		    fprintf(stream2,"\nWind type: Both\n");
       if (splitGraphs && k < WRHOURS)
  	      fprintf(stream2, "\n%ld observation records used\n", WRrecs[k]);
	 
	}
     


	// end added for batch
//calculate and dump data
	//first, calculate bins
	int pLen = atoi(fpSet->m_PeriodLength), numPeriods = 0;
	COleDateTimeSpan span( pLen - 1, 0, 0, 0 );
	COleDateTime d1, d2, d3;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	//ensure don't cross month boundary
	while(d1.GetMonth() != d2.GetMonth())
		d2 -= 1;
	while(1)
	{
		//new period coming into loop
		numPeriods++;
		if(MonthChtoInt(fpSet->m_EndMonth) == d1.GetMonth()
			&& fpSet->m_EndDay >= d1.GetDay()
			&& fpSet->m_EndDay <= d2.GetDay())
			break;
		d3 = d2 + span;
		d3 += 1;
		if(d2.GetMonth() != d3.GetMonth())//does next period cross a month boundary?
		{
			if(d3.GetDay() <= pLen / 2)//will create a new period, otherwise will combine
				numPeriods++;
			if(MonthChtoInt(fpSet->m_EndMonth) == d1.GetMonth()
				&& fpSet->m_EndDay >= d1.GetDay()
				&& fpSet->m_EndDay <= daysInMonth[d1.GetMonth() - 1])
				break;
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d2 = d1 + span;
		}
		else
		{
			d1 = d2;
			d1 += 1;
			d2 = d1 + span;
		}
		//ensure don't cross month boundary
		while(d1.GetMonth() != d2.GetMonth())
			d2 -= 1;
	}
	//ok, got numPeriods. Allocate date arrays and fill values by repeating loop
	COleDateTime *sDates = new COleDateTime[numPeriods];
	COleDateTime *eDates = new COleDateTime[numPeriods];
	int dLoc = 0;
	d1.SetDate(BASISYEAR, MonthChtoInt(fpSet->m_StartMonth), fpSet->m_StartDay);
	d2 = d1 + span;
	//safety
	while(d1.GetMonth() != d2.GetMonth())
		d2 -= 1;
	while(1)
	{
		//new period coming into loop
		sDates[dLoc].SetDate(1996, d1.GetMonth(), d1.GetDay());
		eDates[dLoc++].SetDate(1996, d1.GetMonth(), d2.GetDay());
		if(dLoc >= numPeriods)
			break;
		d3 = d2 + span;
		d3 += 1;
		if(d2.GetMonth() != d3.GetMonth()) //will next period cross a month boundary???
		{
			if(d3.GetDay() <= pLen / 2)
			{
				//extra group to fill
				sDates[dLoc].SetDate(fpSet->m_StartYear, d1.GetMonth(), d2.GetDay() + 1);
				eDates[dLoc++].SetDate(fpSet->m_EndYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
				if(dLoc >= numPeriods)
					break;
			}
			else
			{//combine into previous group
				eDates[dLoc - 1].SetDate(fpSet->m_EndYear, d1.GetMonth(), daysInMonth[d1.GetMonth() - 1]);
			}
			//reset locators to beginning of next month
			d1.SetDate(BASISYEAR, d3.GetMonth(), 1);
			d2 = d1 + span;
		}
		else
		{//won't cross boundary, increment normally
			d1 = d2;
			d1 += 1;
			d2 = d1 + span;
		}
		//ensure same month
		while(d1.GetMonth() != d2.GetMonth())
			d2 -= 1;
	}
	int cnts[10][7];
	double spds[10];

	

	for(int p = 0; p < numPeriods; p++)
	{
		int i;
		for(i = 0; i < 10; i++)
		{
			for(int s = 0; s < 7; s++)
				cnts[i][s] = 0;
			spds[i] = 0.0;
		}
		for(d = DayOfYear(sDates[p]) - 1; d < DayOfYear(eDates[p]); d++)
		{//combine data 
			for( i = 0; i < 10; i++)
			{
				for(int s = 0; s < 7; s++)
					cnts[i][s] += dirSpd[d][i][s];
				spds[i] += speeds[d][i];
			}
		}
		//dump data
		fprintf(stream, 
			"%d-Day Period Beginning %d/%d\n", pLen, sDates[p].GetMonth(),
			sDates[p].GetDay());
		char typeStr[20];
		if (windsType==1)
			sprintf(typeStr,"Gusts");
		else if (windsType==2)
			sprintf(typeStr,"Both");
		else
			sprintf(typeStr,"Average");
		fprintf(stream, 
			"                                           Wind Speed(%s), MPH\n"
			"          0 - 3        4 - 7        8 - 12      13 - 18      19 - 24       >24          TOTAL     AVG\n"
			"Dir      N   PCT      N   PCT      N   PCT      N   PCT      N   PCT      N   PCT      N   PCT   SPEED\n"
			"------------------------------------------------------------------------------------------------------\n",typeStr);
		double n = cnts[9][6];
		for(d = 0; d < 10; d++)
		{
			switch(d)
			{
			case 0:
				fprintf(stream, " NE");
				break;
			case 1:
				fprintf(stream, "  E");
				break;
			case 2:
				fprintf(stream, " SE");
				break;
			case 3:
				fprintf(stream, "  S");
				break;
			case 4:
				fprintf(stream, " SW");
				break;
			case 5:
				fprintf(stream, "  W");
				break;
			case 6:
				fprintf(stream, " NW");
				break;
			case 7:
				fprintf(stream, "  N");
				break;
			case 8:
				fprintf(stream, "CLM");
				break;
			case 9:
				fprintf(stream, "TOT");
				break;
			}//END SWITCH D (DIRECTION CATEGORY)
			for(int s = 0; s < 7; s++)
			{
				if(cnts[d][s] > 0)
					fprintf(stream, "   %4d %5.1f", 
						cnts[d][s], n > 0 ? ((double)cnts[d][s]) / n * 100.0 : 0.0);
				else//leave blank(no zeros printed
					fprintf(stream, "             ");
			}
			if(cnts[d][6] > 0)
				fprintf(stream, "  %5.1f\n",
					spds[d] / cnts[d][6]);
			else
				fprintf(stream, "\n");
			if(d == 8)
				fprintf(stream,
					"-----------------------------------------------------------------------------------------------------\n");
		}
		fprintf(stream, "\n");
	}
	/*fprintf(stream, "\n%ld observations used out of a possible %d (%d%%)\n", recs, days*24, (int) ((float)(recs)/(float) (days * 24) * 100   ));
	fprintf(stream2, "\n%ld total observations used out of a possible %d (%d%%)\n", WRrecs[WRHOURS], days*24, (int) ((float)(WRrecs[WRHOURS])/(float) (days * 24) * 100   ));

	fprintf(stream,"%d days (%d w/ complete wind dirs - %d%%)\n",days,dir16count, (int) ((float) (dir16count)/(float) (days)*100));
	fprintf(stream2,"%d days (%d w/ complete wind dirs - %d%%)\n",days,dir16count,(int) ((float) (dir16count)/(float) (days)*100));
	*/

	int possObs = days * 24;

	// if both gusts and average winds are used, there
	// are double the number of possible observations
	if (windsType == 2)
		possObs = possObs * 2;

	fprintf(stream, "\n%ld observations used out of a possible %d (%d%%)\n", recs, possObs, 
		(days > 0) ? (int) ((float)(recs)/(float) (possObs) * 100   ) : 0);
	fprintf(stream2, "\n%ld total observations used out of a possible %d (%d%%)\n", WRrecs[WRHOURS], possObs, 
		(days > 0) ? (int) ((float)(WRrecs[WRHOURS])/(float) (possObs) * 100   ) : 0);

	fprintf(stream,"%d days (%d w/ complete wind dirs - %d%%)\n",days,dir16count, 
		(days > 0) ? (int) ((float) (dir16count)/(float) (days)*100) : 0);
	fprintf(stream2,"%d days (%d w/ complete wind dirs - %d%%)\n",days,dir16count,
		(days > 0) ? (int) ((float) (dir16count)/(float) (days)*100) : 0);

	
	if(isSIG)
	{//list stations included in SIG
		sSet.Close();
	}
	fclose(stream);
	fclose(stream2);
	delete[] sDates;
	delete[] eDates;
	EnableWindow(TRUE);
	//if (cd)
	//	cd->DestroyWindow();


	if (incGraph){
	   for (int k=0;k<=WRHOURS;k++){	

          if (! splitGraphs)
			   if (k < WRHOURS)
				   continue;

	      switch (k) {
		     case 0:
                if (! wind_0_4)
			       continue;
                break;
             case 1:
                if (! wind_4_8)
  			        continue;
				break;
		     case 2:
                 if (! wind_8_12)
			         continue;
		         break;
		     case 3:
                 if (! wind_12_16)
			        continue;
                 break;
		     case 4:
                if (! wind_16_20)
			        continue;
                break;		  
		     case 5:
                if (! wind_20_0)
			       continue;
		        break;
		  }

	      //now display the wind rose graph in a child window
	      POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	      while(curTemplatePos != NULL)
	      {
		      CDocTemplate* curTemplate = 
			     theApp.GetNextDocTemplate(curTemplatePos);
		      CString str;
		      curTemplate->GetDocString(str, CDocTemplate::docName);
		      if(str == _T("WindRose"))
		      {
			
			     CWindRoseDoc *pDoc = (CWindRoseDoc *)curTemplate->CreateNewDocument();
			     //set WindRoseDoc members
			     //pDoc->dbName = pocketSet->m_pDatabase->GetDatabaseName();
         		
			     for (int i=0;i<WRDIRS;i++){
                    for (int j=0;j<WRSPEEDS;j++)
					     pDoc->percs[i][j] = percs[i][j][k];
				 
			     }
			     pDoc->recs = WRrecs[k];
                 pDoc->maxPerc = maxPerc[k];
			     pDoc->days = days;
			     pDoc->days16 = dir16count;

	              // create header strings for CWindRoseDoc
			      // can't use normal report headers - they are too big
			      // and will not display correctly on graph
                  CString str2,stream;

	              CSIGStationSet staSet(fpSet->m_pDatabase);
	             
	

	              if (fpSet->m_SIG_Station.GetLength() > 6){   // SIG
		             char sig[64];
		             strcpy(sig, fpSet->m_SIG_Station);
		             CString temp;
		             CStationInSIGSet sSet(fpSet->m_pDatabase);
		
		             temp.Format("[SIG] = '%-20.20s'", &sig[6]);
		             sSet.m_strFilter = _T(temp);
		             sSet.Open();
				     pDoc->staStr.Format("Stations: "); 
				     int staCount = 0;
		             while(!sSet.IsEOF())
					 {
			            CString staQuery;
			            staQuery.Format("[StationID] = '%6.6s'", sSet.m_StationID);
			            staSet.m_strFilter = _T(staQuery);
			            staSet.Open();
		
			          
			          
			            str2.Format("%s-%s", staSet.m_StationID, staSet.m_Name);
			            if (staCount > 0)
				  		    pDoc->staStr.Append(", ");
					    staCount ++;

					    pDoc->staStr.Append(str2);
			
			            staSet.Close();
			            sSet.MoveNext();
		             }
		             sSet.Close();
		
	
	               } 
				  else 
				  { // single station
		             CString staQuery;
		             staQuery.Format("[StationID] = '%6.6s'", fpSet->m_SIG_Station);
		             staSet.m_strFilter = _T(staQuery);
		             staSet.Open();
		
		             pDoc->staStr.Format("Station: "); 
				     staSet.m_StationID.Trim();

		             if (! staSet.m_StationID.IsEmpty())
		                str2.Format("%s %s", staSet.m_StationID, staSet.m_Name);
				     else
					    str2.Format("%s",fpSet->m_SIG_Station);

		             pDoc->staStr.Append(str2);
		         
	

		             staSet.Close();
		
	              }
			      pDoc->perStr.Format("Annual filter dates:  %s %d  thru  %s %d", fpSet->m_StartMonth, fpSet->m_StartDay, fpSet->m_EndMonth, fpSet->m_EndDay);

			      pDoc->dateStr.Format("Data years:  %d - %d", fpSet->m_StartYear, fpSet->m_EndYear);

			     CWindRoseFrame* pFrame = (CWindRoseFrame*)curTemplate->CreateNewFrame(pDoc, NULL);
				
			     curTemplate->InitialUpdateFrame(pFrame, pDoc);
			     CString tTitle;
				 switch(k){
				   case 0:
					   pDoc->hourStr.Format("Hours: 0000-0300");
					   tTitle.Format("%s - Wind Rose - 0000-0300 hrs",fpSet->m_SIG_Station);
					   break;
				   case 1:
					   pDoc->hourStr.Format("Hours: 0400-0700");
					   tTitle.Format("%s - Wind Rose - 0400-0700 hrs",fpSet->m_SIG_Station);
					   break;
				   case 2:
					   pDoc->hourStr.Format("Hours: 0800-1100");
					   tTitle.Format("%s - Wind Rose - 0800-1100 hrs",fpSet->m_SIG_Station);
					   break;
				   case 3:
					   pDoc->hourStr.Format("Hours: 1200-1500 + daily obs.");
					   tTitle.Format("%s - Wind Rose - 1200-1500 hrs",fpSet->m_SIG_Station);
					   break;
				   case 4:
					   pDoc->hourStr.Format("Hours: 1600-1900");
					   tTitle.Format("%s - Wind Rose - 1600-1900 hrs",fpSet->m_SIG_Station);
					   break;
				   case 5:
					   pDoc->hourStr.Format("Hours: 2000-2300");
					   tTitle.Format("%s - Wind Rose - 2000-2300 hrs",fpSet->m_SIG_Station);
					   break;
				   case 6:
					   if (wind_0_4 && wind_4_8 && wind_8_12 && wind_12_16 && wind_16_20 && wind_20_0){
					      pDoc->hourStr.Format("All hours combined");
					      tTitle.Format("%s - Wind Rose - All hours combined",fpSet->m_SIG_Station);
					   } else {
						   pDoc->hourStr.Format("Hrs: ");
						   if (wind_0_4)
							   pDoc->hourStr.Append("0-3,");
                           if (wind_4_8)
							   pDoc->hourStr.Append("4-7,");
						   if (wind_8_12)
							   pDoc->hourStr.Append("8-11,");
						   if (wind_12_16)
							   pDoc->hourStr.Append("12-15,");
						   if (wind_16_20)
							   pDoc->hourStr.Append("16-19,");
						   if (wind_20_0)
							   pDoc->hourStr.Append("20-23");

						   tTitle.Format("%s - Wind Rose - Combined hourly periods", fpSet->m_SIG_Station);
						  

					   }

					   break;

				 }
			 
			     pDoc->SetTitle(tTitle);

				 if (windsType == 2)
					 pDoc->extraStr.Format("Winds: Both");
				 else if (windsType == 0)
					 pDoc->extraStr.Format("Winds: Ave.");
				 else if (windsType == 1)
					 pDoc->extraStr.Format("Winds: Gusts");
				 
			     break;
		      }
	       }
	   }
	  
	}
	if (splitGraphs){
	    CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	    mainFrame->MDICascade(MDITILE_SKIPDISABLED);
    }
    if (incWinds){
	   //now display the file in a child window
	   POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	   while(curTemplatePos != NULL)
	   {
	   	   CDocTemplate* curTemplate = 
			theApp.GetNextDocTemplate(curTemplatePos);
		   CString str3;
		   curTemplate->GetDocString(str3, CDocTemplate::docName);
		   if(str3 == _T("TextFile"))
		   {
			   CRichDoc *pDoc = (CRichDoc *)curTemplate->OpenDocumentFile(fName);
			   pDoc->isTemp = true;
			   free(fName);
			   CString tTitle;
			   tTitle.Format("%s - Wind Speed vs Wind Direction", fpSet->m_SIG_Station);
			   pDoc->SetTitle(tTitle);
			
		   }
	   }

	}


	if (incTable){
	   //now display the file in a child window
	   POSITION curTemplatePos = theApp.GetFirstDocTemplatePosition();

	   while(curTemplatePos != NULL)
	   {
	   	   CDocTemplate* curTemplate = 
			theApp.GetNextDocTemplate(curTemplatePos);
		   CString str4;
		   curTemplate->GetDocString(str4, CDocTemplate::docName);
		   if(str4 == _T("TextFile"))
		   {
			   CRichDoc *pDoc2 = (CRichDoc *)curTemplate->OpenDocumentFile(fName2);
			   pDoc2->isTemp = true;
			   free(fName2);
			   CString tTitle;
			   tTitle.Format("%s - Wind Frequency Table", fpSet->m_SIG_Station);
			   pDoc2->SetTitle(tTitle);
			
		   }
	   }

	}
	//EnableWindow(TRUE);
   CDialog::OnOK();
}


void CWindRoseDialog::OnBnClickedWinds48()
{
	// TODO: Add your control notification handler code here
}

void CWindRoseDialog::OnBnClickedWinds04()
{
	// TODO: Add your control notification handler code here
}

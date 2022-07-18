// CFW21ImportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "afxdialogex.h"
#include "CFW21ImportDlg.h"
#include "wxSet.h"
#include "SIGStationSet.h"
#include "fireplusDoc.h"
#include <direct.h>
#include "fw21.h"
#include "ImportDoneDialog.h"
//#include <deque>

// CFW21ImportDlg dialog
extern CFireplusApp theApp;

IMPLEMENT_DYNAMIC(CFW21ImportDlg, CDialogEx)

CFW21ImportDlg::CFW21ImportDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FW21_IMPORT, pParent)
{
	pDoc = NULL;
	m_strFW21File = "";
}

CFW21ImportDlg::~CFW21ImportDlg()
{
}

void CFW21ImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_STATION, m_comboStation);
	DDX_Control(pDX, IDC_EDIT_FW21_FILE, m_editFW21File);
}


BEGIN_MESSAGE_MAP(CFW21ImportDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_FW21_FILE, &CFW21ImportDlg::OnBnClickedButtonFw21File)
	ON_BN_CLICKED(ID_IMPORT_FW21, &CFW21ImportDlg::OnBnClickedImportFw21)
END_MESSAGE_MAP()


// CFW21ImportDlg message handlers


void CFW21ImportDlg::OnBnClickedButtonFw21File()
{
	chdir(theApp.userDir);
	CFileDialog	dlg(TRUE, _T("fw9"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
		OFN_PATHMUSTEXIST,
		_T("Fire Weather 21 Files (*.fw21;*.csv)|*.fw21;*.csv|All Files (*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK)
	{
		m_strFW21File = dlg.GetPathName();
		PathSetDlgItemPath(m_hWnd, IDC_EDIT_FW21_FILE, m_strFW21File);
		theApp.SetUserDir(dlg.GetPathName());
	}
	chdir(theApp.dbDir);
}

struct FW21ImportData
{
	int nErrors;
	bool bOverwrite;
	FILE* errLog;
	string importFileName;
	string stationID;
	CFireplusDoc* pDoc;
	int count;
};

bool ImportFW21RunProc(const CUPDUPDATA* pCUPDUPData)
{
	FW21ImportData *pData = (FW21ImportData *)pCUPDUPData->GetAppData();
	if (pData->pDoc->HasTempWx())
	{
		try
		{
			CString strSql = "DROP TABLE [ffpTempWx]";
			pData->pDoc->m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	CString progText;
	progText.Format("Reading %s", pData->importFileName.c_str());
	pCUPDUPData->SetProgress(progText);
	CFW21Data fw21Data;
	int fw21Status = fw21Data.LoadFile(pData->importFileName.c_str());
	if (fw21Status != 0)
	{
		CString msg;
		msg.Format("Error loading %s as FW21 file\n", pData->importFileName.c_str());
		fprintf(pData->errLog, msg);
		AfxMessageBox(msg);

	}
	int regHour = 13;
	CSIGStationSet stnSet(pData->pDoc->m_pDB);
	stnSet.Open();
	// look for station record
	stnSet.m_strFilter.Format("[StationID] = '%6.6s'", pData->stationID.c_str());
	stnSet.Requery();
	if (!stnSet.IsEOF())
	{
		regHour = stnSet.m_RegSchdObs;
	}
	stnSet.Close();
	pData->pDoc->CreateTempWx();
	CWxSet wxObs(pData->pDoc->m_pDB);
	wxObs.m_strSort = "[ObsDate]";
	wxObs.Open();
	CWxSet twxObs(pData->pDoc->m_pDB);
	twxObs.m_strSort = "[ObsDate]";
	twxObs.Open(CRecordset::dynaset, "ffpTempWx");
	int transStatus = twxObs.m_pDatabase->BeginTrans();

	//std::deque<double> temp24;
	//std::deque<double> rh24;
	//std::deque<double> pcp24;
	const double SECS_PER_DAY = 86400.0;
	for (size_t r = 0; r < fw21Data.GetNumRecs(); r++)
	{
		progText.Format("Processing FW21 record %d", (int)r);
		pCUPDUPData->SetProgress(progText);
		pCUPDUPData->SetProgress((double)r / (double)fw21Data.GetNumRecs() * 100.0);
		FW21Record rec = fw21Data.GetRec(r), rec2;
		tm trgTime = rec.GetDateTime();
		time_t trgTimet = mktime(&trgTime), thisTimet;
		double tMin = rec.GetTemp(), tMax = rec.GetTemp(), rhMin = rec.GetRH(), rhMax = rec.GetRH(), pcp = rec.GetPrecip();
		if (pcp < 0.0)
			pcp = 0.0;
		int pcpDur = 0;
		if (pcp > 0.0)
			pcpDur++;
		int checkRec = r > 0 ? r - 1 : 0;
		//need 24 values for tmpMin, tmpMax, RHMin, RHMax, and precip
		while (checkRec >= 0)
		{
			rec2 = fw21Data.GetRec(checkRec);
			tm thisTime = rec2.GetDateTime();
			thisTimet = mktime(&thisTime);
			if (difftime(trgTimet, thisTimet) >= SECS_PER_DAY)
				break;
			double thisTemp = rec2.GetTemp(), thisRH = rec2.GetRH(), thisPcp = rec2.GetPrecip();
			if (thisTemp != dNODATA)
			{
				if (thisTemp < tMin || tMin == dNODATA)
					tMin = thisTemp;
				if (thisTemp > tMax || tMax == dNODATA)
					tMax = thisTemp;
			}
			if (thisRH != dNODATA)
			{
				if (thisRH < rhMin || rhMin == dNODATA)
					rhMin = thisRH;
				if (thisRH > rhMax || rhMax == dNODATA)
					rhMax = thisRH;
			}
			if (thisPcp != dNODATA)// || pcp == dNODATA)
				pcp += thisPcp;
			if (thisPcp > 0.0)
				pcpDur++;

			checkRec--;
		}
		//ok, now place record in twxObs
		twxObs.AddNew();
		twxObs.m_StationID = pData->stationID.c_str();
		tm dateTime = rec.GetDateTime();
		int y = dateTime.tm_year + 1900;
		int m = dateTime.tm_mon + 1;
		int d = dateTime.tm_mday;
		int hr = dateTime.tm_hour;
		int mn = dateTime.tm_min;
		twxObs.m_ObsDate.SetDateTime(y, m, d, hr, 0, 0);
		// if obs minute was in latter part of hour, round up an hour
		if (mn > 30) {
			COleDateTimeSpan ts(0, 1, 0, 0);
			twxObs.m_ObsDate += ts;
		}
		twxObs.m_Hour = twxObs.m_ObsDate.GetHour();
		twxObs.m_ObsType = "R";
		twxObs.SetFieldNull(&twxObs.m_SOW);
		twxObs.m_Temp = rec.GetTemp();
		twxObs.m_RH = rec.GetRH();
		twxObs.m_WS = rec.GetWindSpeed();
		int tdir = twxObs.m_WAzimuth = rec.GetWindAzimuth();		
		if (tdir > 337 || tdir < 23)
			tdir = 8;
		else if (tdir > 22 && tdir < 68)
			tdir = 1;
		else if (tdir > 67 && tdir < 113)
			tdir = 2;
		else if (tdir > 112 && tdir < 158)
			tdir = 3;
		else if (tdir > 157 && tdir < 203)
			tdir = 4;
		else if (tdir > 202 && tdir < 248)
			tdir = 5;
		else if (tdir > 247 && tdir < 293)
			tdir = 6;
		else if (tdir > 292 && tdir < 338)
			tdir = 7;
		//}
		twxObs.m_WDir = tdir;
		if (rec.GetGustSpeed() != dNODATA)
			twxObs.m_GustSpd = rec.GetGustSpeed();
		else
			twxObs.SetFieldNull(&twxObs.m_GustSpd);
		if (rec.GetGustAzimuth() != iNODATA)
			twxObs.m_GustDir = rec.GetGustAzimuth();
		else
			twxObs.SetFieldNull(&twxObs.m_GustDir);
		twxObs.SetFieldNull(&twxObs.m_OMC10);
		twxObs.m_TmpMax = tMax;
		twxObs.m_TmpMin = tMin;
		twxObs.m_RHMax = rhMax;
		twxObs.m_RHMin = rhMin;
		twxObs.m_HourlyPrecip = rec.GetPrecip();
		twxObs.m_PPTAMT = pcp;
		twxObs.m_PPTDUR = pcpDur;
		twxObs.m_WetFlag = 0;
		twxObs.SetFieldNull(&twxObs.m_GreenHerb);
		twxObs.SetFieldNull(&twxObs.m_GreenShrub);
		twxObs.SetFieldNull(&twxObs.m_Season);
		twxObs.m_SolarRadiation = rec.GetSolarRadiation();// atoi(tSlr);
		if(rec.GetSnowFlag() != iNODATA)
			twxObs.m_SnowFlag = rec.GetSnowFlag();
		else
			twxObs.SetFieldNull(&twxObs.m_SnowFlag);

		twxObs.Update();
	}
	twxObs.m_pDatabase->CommitTrans(); //so we can requery

	CString strSQLa;
	strSQLa.Format("[StationID] = '%6.6s'", pData->stationID.c_str());
	CString strSQLb;
	strSQLb.Format("[StationID] = '%6.6s'", pData->stationID.c_str());
	wxObs.m_strFilter = strSQLa;
	twxObs.m_strFilter = strSQLb;
	BOOL wxRequery = wxObs.Requery();
	BOOL tWxRequery = twxObs.Requery();

	//ok, put into wxObs appending or editing
	COleDateTime lastDay;
	//CTime lastDay;
	pCUPDUPData->SetProgress(0.0);
	long edits = 0, appends = 0, wetFlagEdits = 0, nDuplicates = 0, errors = 0, count = 0;
	lastDay.SetDateTime(1900, 1, 1, 1, 1, 1);//minimum start date
	twxObs.m_pDatabase->BeginTrans();
	while (!twxObs.IsEOF() && !pCUPDUPData->ShouldTerminate())// && (!cd || !cd->Abort()))
	{
		bool tryEdit = false;
		bool thisEdit = false;
		count++;
		progText.Format("Updating WxObs %ld", count);
		pCUPDUPData->SetProgress(progText);
		pCUPDUPData->SetProgress((double)count / (double)fw21Data.GetNumRecs() * 100.0);

		//if (cd)
		 //  cd->Increment();
		COleDateTime tTime(twxObs.m_ObsDate.GetYear(), twxObs.m_ObsDate.GetMonth(),
			twxObs.m_ObsDate.GetDay(), twxObs.m_ObsDate.GetHour(), twxObs.m_ObsDate.GetMinute(),
			twxObs.m_ObsDate.GetSecond());

		while (!twxObs.IsEOF()
			&& tTime <= lastDay)//may be dups in twxObs
		{
			nDuplicates++;
			errors++;


			fprintf(pData->errLog, "ERROR: StationID %s Date: %s duplicate record rejected (time duplicate)\n",
				twxObs.m_StationID.GetBuffer(), twxObs.m_ObsDate.Format("%m/%d/%y %H:%M").GetBuffer());
			twxObs.MoveNext();
			tTime.SetDateTime(twxObs.m_ObsDate.GetYear(), twxObs.m_ObsDate.GetMonth(),
				twxObs.m_ObsDate.GetDay(), twxObs.m_ObsDate.GetHour(), twxObs.m_ObsDate.GetMinute(),
				twxObs.m_ObsDate.GetSecond());
		}
		if (twxObs.IsEOF())
			break;
		lastDay.SetDateTime(twxObs.m_ObsDate.GetYear(), twxObs.m_ObsDate.GetMonth(),
			twxObs.m_ObsDate.GetDay(), twxObs.m_ObsDate.GetHour(), twxObs.m_ObsDate.GetMinute(),
			twxObs.m_ObsDate.GetSecond());
		//= twxObs.m_ObsDate;
		while (!wxObs.IsEOF()
			&& wxObs.m_ObsDate < twxObs.m_ObsDate)
		{
			wxObs.MoveNext();
		}

		if (!wxObs.IsEOF() && !wxObs.IsBOF()
			&& wxObs.m_ObsDate == twxObs.m_ObsDate
			&& wxObs.m_StationID.Compare(pData->stationID.c_str()) == 0)//twxObs.m_StationID)
		{


			tryEdit = true;
			//edits++;
			//if(wxObs.Is
			//twxObs.MoveNext();
			//continue;
			wxObs.Edit();

			//ATLTRACE("Calling Edit for stationID: %s, %d, %d, %d, %d\n", twxObs.m_StationID, twxObs.m_ObsDate.GetYear(), twxObs.m_ObsDate.GetMonth(), twxObs.m_ObsDate.GetDay(), twxObs.m_ObsDate.GetHour());

		}
		else
		{
			wxObs.AddNew();
			appends++;

			wxObs.SetFieldNull(&wxObs.m_ObsType);
			wxObs.SetFieldNull(&wxObs.m_UserWX);
			wxObs.SetFieldNull(&wxObs.m_SOW);
			wxObs.SetFieldNull(&wxObs.m_Temp);
			wxObs.SetFieldNull(&wxObs.m_RH);
			wxObs.SetFieldNull(&wxObs.m_WDir);
			wxObs.SetFieldNull(&wxObs.m_WS);
			wxObs.SetFieldNull(&wxObs.m_OMC10);
			wxObs.SetFieldNull(&wxObs.m_OMCWood);
			wxObs.SetFieldNull(&wxObs.m_OMCWoodDate);
			wxObs.SetFieldNull(&wxObs.m_TmpMax);
			wxObs.SetFieldNull(&wxObs.m_TmpMin);
			wxObs.SetFieldNull(&wxObs.m_RHMax);
			wxObs.SetFieldNull(&wxObs.m_RHMin);
			wxObs.SetFieldNull(&wxObs.m_PPTDUR);
			wxObs.SetFieldNull(&wxObs.m_PPTAMT);
			wxObs.SetFieldNull(&wxObs.m_Season);
			wxObs.SetFieldNull(&wxObs.m_GreenHerb);
			wxObs.SetFieldNull(&wxObs.m_GreenShrub);
			wxObs.SetFieldNull(&wxObs.m_MC1);
			wxObs.SetFieldNull(&wxObs.m_MC10);
			wxObs.SetFieldNull(&wxObs.m_MC100);
			wxObs.SetFieldNull(&wxObs.m_MC1000);
			wxObs.SetFieldNull(&wxObs.m_X1000);
			wxObs.SetFieldNull(&wxObs.m_MCHerb);
			wxObs.SetFieldNull(&wxObs.m_MCWood);
			wxObs.SetFieldNull(&wxObs.m_GREN);
			wxObs.SetFieldNull(&wxObs.m_HStage);
			wxObs.SetFieldNull(&wxObs.m_ROS);
			wxObs.SetFieldNull(&wxObs.m_ERC);
			wxObs.SetFieldNull(&wxObs.m_BI);
			wxObs.SetFieldNull(&wxObs.m_FIL);
			wxObs.SetFieldNull(&wxObs.m_WAzimuth);
			wxObs.SetFieldNull(&wxObs.m_SolarRadiation);
			wxObs.SetFieldNull(&wxObs.m_WetFlag);
			wxObs.SetFieldNull(&wxObs.m_GustDir);
			wxObs.SetFieldNull(&wxObs.m_GustSpd);
			wxObs.SetFieldNull(&wxObs.m_SnowFlag);
			wxObs.SetFieldNull(&wxObs.m_HourlyPrecip);
			wxObs.SetFieldNull(&wxObs.m_DailyObs);
		}

		// force stationID to the WIMS ID used for import
		// (twxObs WRCC ID may correlate to wxObs WIMS ID)

		wxObs.m_StationID.Format("%6.6s", pData->stationID.c_str());

		bool replaceAll = pData->bOverwrite;

		wxObs.m_ObsDate = twxObs.m_ObsDate;
		wxObs.m_Hour = wxObs.m_ObsDate.GetHour();

		// if existing record is NULL, then we can always overwrite
		if (!twxObs.IsFieldNull(&twxObs.m_ObsType) && wxObs.IsFieldNull(&wxObs.m_ObsType))
			wxObs.m_ObsType = twxObs.m_ObsType;
		else
			// 2014 change: a type O is allowed to overwrite a type R (or a NULL ObsType, etc.)
			if (twxObs.m_ObsType.Compare("O") == 0 && wxObs.m_ObsType.Compare("O"))
			{
				replaceAll = true;
				wxObs.m_ObsType = twxObs.m_ObsType;
			}

		if (!twxObs.IsFieldNull(&twxObs.m_UserWX) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_UserWX)))
		{
			wxObs.m_UserWX = twxObs.m_UserWX;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_SOW) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_SOW)))
		{
			wxObs.m_SOW = twxObs.m_SOW;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_Temp) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_Temp)))
		{
			wxObs.m_Temp = twxObs.m_Temp;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_RH) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_RH)))
		{
			wxObs.m_RH = max(twxObs.m_RH, 1);
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_WDir) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_WDir)))
		{
			wxObs.m_WDir = twxObs.m_WDir;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_WS) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_WS)))
		{
			wxObs.m_WS = twxObs.m_WS;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_OMC10) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_OMC10)))
		{
			wxObs.m_OMC10 = twxObs.m_OMC10;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_OMCWood) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_OMCWood)))
		{
			wxObs.m_OMCWood = twxObs.m_OMCWood;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_OMCWoodDate) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_OMCWoodDate)))
		{
			wxObs.m_OMCWoodDate = twxObs.m_OMCWoodDate;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_TmpMax) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_TmpMax)))
		{
			wxObs.m_TmpMax = twxObs.m_TmpMax;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_TmpMin) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_TmpMin)))
		{
			wxObs.m_TmpMin = twxObs.m_TmpMin;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_RHMax) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_RHMax)))
		{
			wxObs.m_RHMax = twxObs.m_RHMax;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_RHMin) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_RHMin)))
		{
			wxObs.m_RHMin = max(twxObs.m_RHMin, 1);
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_PPTDUR) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_PPTDUR)))
		{
			wxObs.m_PPTDUR = twxObs.m_PPTDUR;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_PPTAMT) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_PPTAMT)))
		{
			wxObs.m_PPTAMT = twxObs.m_PPTAMT;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_Season) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_Season)))
		{
			wxObs.m_Season = twxObs.m_Season;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_GreenHerb) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_GreenHerb)))
		{
			wxObs.m_GreenHerb = twxObs.m_GreenHerb;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_GreenShrub) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_GreenShrub)))
		{
			wxObs.m_GreenShrub = twxObs.m_GreenShrub;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_MC1) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_MC1)))
		{
			wxObs.m_MC1 = twxObs.m_MC1;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_MC10) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_MC10)))
		{
			wxObs.m_MC10 = twxObs.m_MC10;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_MC100) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_MC100)))
		{
			wxObs.m_MC100 = twxObs.m_MC100;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_MC1000) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_MC1000)))
		{
			wxObs.m_MC1000 = twxObs.m_MC1000;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_X1000) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_X1000)))
		{
			wxObs.m_X1000 = twxObs.m_X1000;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_MCHerb) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_MCHerb)))
		{
			wxObs.m_MCHerb = twxObs.m_MCHerb;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_MCWood) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_MCWood)))
		{
			wxObs.m_MCWood = twxObs.m_MCWood;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_GREN) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_GREN)))
		{
			wxObs.m_GREN = twxObs.m_GREN;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_HStage) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_HStage)))
		{
			wxObs.m_HStage = twxObs.m_HStage;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_ROS) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_ROS)))
		{
			wxObs.m_ROS = twxObs.m_ROS;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_ERC) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_ERC)))
		{
			wxObs.m_ERC = twxObs.m_ERC;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_BI) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_BI)))
		{
			wxObs.m_BI = twxObs.m_BI;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_FIL) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_FIL)))
		{
			wxObs.m_FIL = twxObs.m_FIL;
			thisEdit = true;
		}

		if (!twxObs.IsFieldNull(&twxObs.m_WAzimuth) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_WAzimuth)))
		{
			wxObs.m_WAzimuth = twxObs.m_WAzimuth;
			thisEdit = true;
		}
		// new FFP4 fields here     5/2006
		if (!twxObs.IsFieldNull(&twxObs.m_SolarRadiation) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_SolarRadiation)))
		{
			wxObs.m_SolarRadiation = twxObs.m_SolarRadiation;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_WetFlag) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_WetFlag)))
		{
			wxObs.m_WetFlag = twxObs.m_WetFlag;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_GustDir) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_GustDir)))
		{
			wxObs.m_GustDir = twxObs.m_GustDir;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_GustSpd) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_GustSpd)))
		{
			wxObs.m_GustSpd = twxObs.m_GustSpd;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_SnowFlag) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_SnowFlag)))
		{
			wxObs.m_SnowFlag = twxObs.m_SnowFlag;
			thisEdit = true;
		}
		if (!twxObs.IsFieldNull(&twxObs.m_HourlyPrecip) && (replaceAll || wxObs.IsFieldNull(&wxObs.m_HourlyPrecip)))
		{
			wxObs.m_HourlyPrecip = twxObs.m_HourlyPrecip;
			thisEdit = true;
		}

		wxObs.m_DailyObs = 0;
		if (wxObs.m_ObsType.CompareNoCase("O") == 0)
			wxObs.m_DailyObs = 1;
		else
			if ((wxObs.m_ObsType.CompareNoCase("R") == 0)
				&& (wxObs.m_Hour == regHour))
				//&& (!wxObs.IsFieldNull(&wxObs.m_SOW)))
				wxObs.m_DailyObs = 1;
		try
		{
			wxObs.Update();
			//wxRequery = wxObs.Requery();
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
		if (tryEdit && thisEdit)
			edits++;
		//LogWxDataWarnings(errLog, &twxObs);
		pData->count++;
		twxObs.MoveNext();
	}
	pCUPDUPData->AllowCancel(FALSE);
	pCUPDUPData->SetProgress("Setting DailyObs...");
	if (!pCUPDUPData->ShouldTerminate())
	{
		twxObs.m_pDatabase->CommitTrans(); //so we can requery	
		wxObs.m_strSort.Format("[ObsDate]");
		wxObs.Requery();
		if (!wxObs.IsEOF())
		{
			//COleDateTime startPrecip;

			//int success = wxObs.CalcHourlyPrecip();
			//if (success < 0)
			//	fprintf(errLog, "Station: %s, unable to calculate hourly precip\n", sta);
			//else
			//	fprintf(errLog, "Station: %s, \n\tHourly precip started on %s\n",
			//	sta, startPrecip.Format("%m/%d/%Y %H:%M"));
			//wxObs.MoveFirst();
			wxObs.setDailyObs(regHour);
			wxObs.CalcSOW();
			wxObs.Requery();
			fprintf(pData->errLog, "%d records processed\n%d records updated\n%d records appended\n%d errors\n",
				count, edits, appends, errors);
			pData->nErrors = errors;
		}
	}
	else
		twxObs.m_pDatabase->Rollback();
	pCUPDUPData->AllowCancel(TRUE);
	return true;
}

void CFW21ImportDlg::OnBnClickedImportFw21()
{
	if (m_comboStation.GetCurSel() < 0)
	{
		AfxMessageBox("A Station must be selected");
		return;
	}
	if (m_strFW21File.GetLength() <= 0)
	{
		AfxMessageBox("An import data file must be selected");
		return;
	}
	FW21ImportData importData;
	CString overwriteMsg = "FireFamily Plus can overwrite existing records or disregard import data for existing records. Note: Existing data fields that are NULL will always be updated if import data is not NULL.\n\n Should existing records be overwritten?";
	int option = AfxMessageBox(overwriteMsg, MB_YESNOCANCEL);
	if (option == IDCANCEL)
		return;
	if (option == IDYES)
		importData.bOverwrite = true;
	else
		importData.bOverwrite = false;
	char* logFileName = GetTempFileName();
	FILE* logFile = fopen(logFileName, "wt");
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	fprintf(logFile, "FireFamily Plus FW21 (CSV) Weather File Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S").GetBuffer());

	importData.nErrors = 0;
	importData.count = 0;
	CString tStation;
	m_comboStation.GetWindowTextA(tStation);
	importData.stationID = tStation.Left(6);
	importData.importFileName = m_strFW21File;
	importData.errLog = logFile;
	importData.pDoc = pDoc;
	CUPDialog cupDlg(GetSafeHwnd(), ImportFW21RunProc, &importData);
	INT_PTR ret = cupDlg.DoModal();
	CString strMsg;
	if (ret == IDOK)
	{
		fprintf(logFile, "\nImport Completed: %s\n%d errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S").GetBuffer(), importData.nErrors);
		strMsg.Format("Import Complete.\n%d errors were encountered.\nRecords processed: %ld\n", importData.nErrors, importData.count);
	}
	else
	{
		fprintf(logFile, "\n****** Import Cancelled by user. **********\n");
		strMsg.Format("Import cancelled. Some data may have been imported.\nPlease check the log file for complete information.");
	}
	fclose(logFile);
	CImportDoneDialog idd(this);
	idd.statusStr = strMsg;
	idd.logFileName = logFileName;
	delete[] logFileName;
	idd.DoModal();
}


BOOL CFW21ImportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (pDoc)
	{
		//populate stations combo
		CSIGStationSet staSet(pDoc->m_pDB);
		staSet.m_strSort = "[StationID]";
		staSet.Open();
		while (!staSet.IsEOF())
		{
			CString str;
			str.Format("%6.6s - %s", staSet.m_StationID.GetBuffer(), staSet.m_Name.GetBuffer());
			m_comboStation.AddString(str);
			staSet.MoveNext();
		}
		staSet.Close();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

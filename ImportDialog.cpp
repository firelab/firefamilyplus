// ImportDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "wxSet.h"
#include "SIGStationSet.h"
#include "FireRegionSet.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"
//#include "FireSet.h"
#include "FireAgencySet.h"
#include "ImportDialog.h"
#include "CustomFireSet.h"
//#include "CuctomFireDialog.h"
#include <math.h>

#include "ImportDoneDialog.h"
#include "SelStationDialog.h"
#include "ImportWRCCDialog.h"

#include <direct.h>
#include "countySet.h"
#include "stateSet.h"
#include "fireplusView.h"
#include "fireplusDoc.h"
#include "utils.h"
#include <cderr.h>
#include <iostream>
#include <fstream>
#include <sstream>


#include "GenericWxImportDialog.h"
#include "GenericFireImportDialog.h"

#include "DOIunitSet.h"
#include "CFW21ImportDlg.h"
#include "csv_readrow.h"
//#include "updialog2.h"
#include <unordered_map>
#include <set>

using namespace std;  // needed for stream stuff.... std::<whatever> won't work :(

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PIVOT 30
#define MF_BUFSIZE 32768

extern CFireplusApp theApp;
extern char *monthNames[];

const CString staHeader = "WLSTINV1";

CString errStrings[] =
{
	"Blank station id",
	"Invalid date",
	"Unrecognized Region ID",
	"Unrecognized Unit ID",
	"Unrecognized Unit ID (District)",
	"Invalid Hour in RAWS record",
	"Invalid Minutes in RAWS record.",
	"-9999 found in record",
	"Change in Precipitation Measurement code mid stream",
	"Invalid or missing Precipitation Measurement Code"
};

int MonthAbrevtoInt(CString chMonth)
{
	for(int i = 0; i < 12; i++)
	{
		if(chMonth.CompareNoCase(monthNames[i]) == 0)
			return i + 1;
	}
	return 0;
}

char *trim(char *src)
{
	for(int i = strlen(src) - 1; i >= 0; i--)
	{
		if(src[i] == ' ')
			src[i] = 0;
		else
			return src;
	}
	return src;
}

char *stripCRLF(char *src)
{
	for(int i = strlen(src) - 1; i >= 0; i--)
	{
		if(src[i] == '\r' || src[i] == '\n')
			src[i] = 0;
		else
			return src;
	}
	return src;
}
//RH conversion utility routines
double satvap(double t)

{//return saturationvapor pressure, baset on temperature t(degrees K)
	if(t != 35.86)
		return exp(1.81 + (t * 17.27 - 4717.31) / (t - 35.86));
	else
		return 0.0;
}

double fTok(double f)
{
	//convert fahrenheit to kelvin
	return (f - 32.0) / 1.8 + 273.16;
}

double stationPressure(int elevation, int idNum)
{
	int ev = elevation;
	if(idNum > 50000)
		ev += 200;
	return 1013.09 / exp((double)ev / 25000.0);
}

int rhFromWb(double td, double tw, double pp)
{
	double corr = (0.00066 * (1.0 + (0.00115 * (tw - 273.16))) * pp * (td - tw));
	double val = max(1.0, min(100.0, ((satvap(tw) - corr) / satvap(td)) * 100.0));
	double rem = val - floor(val);
	int ret  = (int)floor(val);
	return ret + ((rem >= 0.5) ? 1 : 0);
}

int rhFromDp(double dry, double dew)
{
	double val = 100.0 * (exp(-7482.6 / (dew + 398.36) + 15.674)
		/ exp(-7482.6 / (dry + 398.36) + 15.674));
	double rem = val - floor(val);
	int ret  = (int)floor(val);
	return ret + ((rem >= 0.5) ? 1 : 0);
}

short RH(int RHType, int in, int db)
{
	int ret;
	switch(RHType)
	{
	case 1://wet bulb
		ret = rhFromWb(fTok(db), fTok((double)in), 900.0);//station press not corrected for elevation
		break;
	case 3:
		ret = rhFromDp((double) db, (double)in);
		break;
	default:
		ret = in;
	}
	return ret;
}

struct ImportData
{
	int nErrors;
	bool bOverwrite;
	FILE *errLog;
	vector<string> importFileNames;
	CImportDialog *pImportDialog;
};

bool ImportFW13RunProc(const CUPDUPDATA* pCUPDUPData)
{
	ImportData *pData = (ImportData *)pCUPDUPData->GetAppData();
	pData->pImportDialog->pCUPDUPData = (CUPDUPDATA*)pCUPDUPData;
	pCUPDUPData->ShowProgressBar(false);
	pCUPDUPData->SetProgress(_T("Preparing import.."), 0);
	pData->nErrors = 0;
	vector<string>::iterator it;
	CString fName;
	char captionStr[256];
	for (it = pData->importFileNames.begin(); it != pData->importFileNames.end(); ++it)
	{
		CString fileName = (*it).c_str();
		//fName = fileName;
		int loc = fileName.ReverseFind('\\');
		fName = fileName.Right(fileName.GetLength() - loc);
		sprintf(captionStr, "Importing %s", fName);
		pCUPDUPData->SetDialogCaption(captionStr);
		pData->nErrors += pData->pImportDialog->NewImportFW9(fileName, pData->errLog, pData->bOverwrite);
	}
	return true;
}

INT_PTR CImportDialog::RunFW13Import(vector<string> importFileNames, FILE *errLog, int *nErrors)
{
	ImportData importData;
	importData.importFileNames = importFileNames;
	importData.errLog = errLog;
	importData.nErrors = 0;
	importData.pImportDialog = this;
	CString overwriteMsg = "FireFamily Plus can overwrite existing records or disregard import data for existing records. Note: Existing data fields that are NULL will always be updated if import data is not NULL.\n\n Should existing records be overwritten?";
	int option = AfxMessageBox(overwriteMsg, MB_YESNOCANCEL);
	if (option == IDCANCEL)
		return IDCANCEL;
	if (option == IDYES)
		importData.bOverwrite = true;
	else
		importData.bOverwrite = false;
	CUPDialog cupDlg(GetSafeHwnd(), ImportFW13RunProc, &importData);
	INT_PTR ret = cupDlg.DoModal();
	*nErrors = importData.nErrors;
	return ret;
}

bool ImportFWXRunProc(const CUPDUPDATA* pCUPDUPData)
{
	ImportData *pData = (ImportData *)pCUPDUPData->GetAppData();
	pData->pImportDialog->pCUPDUPData = (CUPDUPDATA*)pCUPDUPData;
	pCUPDUPData->ShowProgressBar(false);
	pCUPDUPData->SetProgress(_T("Preparing import.."), 0);
	pData->nErrors = 0;
	vector<string>::iterator it;
	CString fName;
	char captionStr[256];
	for (it = pData->importFileNames.begin(); it != pData->importFileNames.end(); ++it)
	{
		CString fileName = (*it).c_str();
		//fName = fileName;
		int loc = fileName.ReverseFind('\\');
		fName = fileName.Right(fileName.GetLength() - loc);
		sprintf(captionStr, "Importing %s", fName);
		pCUPDUPData->SetDialogCaption(captionStr);
		pData->nErrors += pData->pImportDialog->NewImportFWX(fileName, pData->errLog);
	}
	return true;
}

INT_PTR CImportDialog::RunFWXImport(vector<string> importFileNames, FILE *errLog, int *nErrors)
{
	ImportData importData;
	importData.importFileNames = importFileNames;
	importData.errLog = errLog;
	importData.nErrors = 0;
	importData.pImportDialog = this;
	CUPDialog cupDlg(GetSafeHwnd(), ImportFWXRunProc, &importData);
	INT_PTR ret = cupDlg.DoModal();
	*nErrors = importData.nErrors;
	return ret;
}
/////////////////////////////////////////////////////////////////////////////
// CImportDialog dialog

CImportDialog::CImportDialog(CWnd* pParent /*=NULL*/, CDatabase *_pDB)
: CDialog(CImportDialog::IDD, pParent),
regs(_pDB), units(_pDB), subs(_pDB), firesSet(_pDB)
{
	//{{AFX_DATA_INIT(CImportDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	regs.Open();
	units.Open();
	subs.Open();
	firesSet.Open();
	pDoc = ((CFireplusView *)pParent)->GetDocument();
	m_GACC_Agency_ID = 7;//this is the default, but will be checked/changed in OnInitDialog()
}

void CImportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CImportDialog)
	DDX_Control(pDX, IDC_RAW, rawButton);
	//DDX_Control(pDX, IDC_FPL, fplButton);
	DDX_Control(pDX, IDC_COMBO1, agencies);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_GENERICFIRE, m_genericFireButton);
	DDX_Control(pDX, IDC_NASF_CSV, m_nasfFireButton);
	DDX_Control(pDX, IDC_WFMI, m_WFMIbutton);
	DDX_Control(pDX, IDC_INFORMS_CSV, m_btnINFORMS);
}

BEGIN_MESSAGE_MAP(CImportDialog, CDialog)
	//{{AFX_MSG_MAP(CImportDialog)
	//ON_BN_CLICKED(IDC_FPL, OnFpl)
	ON_BN_CLICKED(IDC_FW9, OnFw9)
	ON_BN_CLICKED(IDC_FWX, OnFwx)
	ON_BN_CLICKED(IDC_RAW, OnRaw)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnSelchangeAgency)
	ON_BN_CLICKED(IDC_STATIONS, OnStations)
	ON_BN_CLICKED(IDC_FWSTXT, OnFwstxt)
	ON_BN_CLICKED(IDC_GWI,OnGenericWx)
	ON_BN_CLICKED(IDC_WFMI,OnWFMI)
	ON_BN_CLICKED(IDC_GENERICFIRE, OnGenericFires)
	ON_BN_CLICKED(IDC_NASF_CSV, OnNASFFires)
	//ON_BN_CLICKED(IDC_WRCCRAWS,OnWRCCRAWS)
	//ON_BN_CLICKED(IDC_WFMI, OnWFMICodes)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_FW21, &CImportDialog::OnBnClickedFw21)
	ON_BN_CLICKED(IDC_INFORMS_CSV, &CImportDialog::OnBnClickedInformsCsv)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImportDialog message handlers

/*void CImportDialog::OnFpl()
{
// TODO: Add your control notification handler code here
long agencyID = -1;
if(agencies.GetCurSel() != CB_ERR)
agencyID = agencies.GetItemData(agencies.GetCurSel());
else
{//report no agency selected
CString strMsg = "An Agency must be selected in order to import fire data.";
AfxMessageBox(strMsg);
return;
}
if(agencyID > 5)
{
ImportCustom(agencyID);
return;
}
chdir(theApp.userDir);
CFileDialog	dlg (TRUE, _T("fpl"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
_T("Fire Record Files (*.fpl)|*.fpl|All Files (*.*)|*.*||"), this);
char *mfBuf = new char[MF_BUFSIZE];
mfBuf[0] = 0;
dlg.m_ofn.lpstrFile = mfBuf;
dlg.m_ofn.nMaxFile = MF_BUFSIZE;
int res = dlg.DoModal();
nNewSubunits = nNewUnits = 0;
if (res == IDOK)
{
CWaitCursor wait;
long errors = 0;
// Get the position of the first filename
char *logFileName = GetTempFileName();
FILE * logFile = fopen(logFileName, "wt");
COleDateTime dateTime = COleDateTime::GetCurrentTime();
fprintf(logFile, "FireFamily Plus FPL Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
POSITION filenamePosition = dlg.GetStartPosition();
CString fileName;

// Open each file selected
int tErrors;
while ( filenamePosition != NULL )
{
fileName = dlg.GetNextPathName( filenamePosition );
tErrors = ImportFPL(fileName, logFile, agencyID);
if(tErrors >= 0)
errors += tErrors;
else
{//user cancelled
errors = -1;
break;
}
}
dateTime = COleDateTime::GetCurrentTime();
CString strMsg;
if(errors >= 0)
{
fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
strMsg.Format("Import Complete.\n%ld errors were encountered.", errors);
if(nNewUnits > 0)
{
CString nuStr;
nuStr.Format("\n%ld new Fire Units were created.", nNewUnits);
strMsg = strMsg + nuStr;
}
if(nNewSubunits > 0)
{
CString nuStr;
nuStr.Format("\n%ld new Fire SubUnits were created.", nNewSubunits);
strMsg = strMsg + nuStr;
}
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
theApp.SetUserDir(dlg.GetPathName());
}
else if(res == IDCANCEL)
{
int err = CommDlgExtendedError();
if(err == FNERR_BUFFERTOOSMALL)
{
AfxMessageBox("File Name buffer overrun, select fewer files.");
}
}
delete[] mfBuf;
chdir(theApp.workDir);
}*/

void CImportDialog::OnGenericWx()
{
	chdir(theApp.userDir);

	CFileDialog	dlg (TRUE, _T("GenericWx"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
		OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
		_T("Generic Weather Import Text Files (*.txt;*.csv)|*.txt;*.csv|All Files (*.*)|*.*||"), this);
	/*CFileDialog	dlg (TRUE, _T("GenericWx"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
	OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
	_T("Generic Fire Import Text Files (*.txt;*.csv)|*.txt;*.csv|All Files (*.*)|*.*||"), this);*/
	char *mfBuf = new char[MF_BUFSIZE];
	mfBuf[0] = 0;
	dlg.m_ofn.lpstrFile = mfBuf;
	dlg.m_ofn.nMaxFile = MF_BUFSIZE;
	int res = dlg.DoModal();

	if (res == IDOK)
	{
		CWaitCursor wait;
		long errors = 0;
		// Get the position of the first filename
		char *logFileName = GetTempFileName();
		FILE * logFile = fopen(logFileName, "wt");
		COleDateTime dateTime = COleDateTime::GetCurrentTime();
		fprintf(logFile, "FireFamily Plus Generic Weather File Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
		POSITION filenamePosition = dlg.GetStartPosition();
		CString fileName;

		// Open each file selected
		int tErrors;
		while ( filenamePosition != NULL )
		{
			fileName = dlg.GetNextPathName( filenamePosition );
			//tErrors = ImportGenericWx(fileName, logFile);
			CGenericWxImportDialog gwd(this,NULL,firesSet.m_pDatabase);
			gwd.fileName.Format("%s",fileName);
			gwd.errLog = logFile;
			gwd.pDoc = pDoc;
			res = gwd.DoModal();
			tErrors = gwd.errors;

			if(tErrors >= 0)
				errors += tErrors;
			else
			{//user cancelled
				errors = -1;
				break;
			}
		}
		//if(errors >= 0)
		//{
		if (res == IDCANCEL){
			fclose(logFile);
			delete[] logFileName;
		} else {
			dateTime = COleDateTime::GetCurrentTime();
			CString strMsg;
			if(errors >= 0)
			{
				fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
				strMsg.Format("Import Complete.\n%ld errors were encountered.", errors);
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
			theApp.SetUserDir(dlg.GetPathName());
		}
	}
	else if(res == IDCANCEL)
	{
		int err = CommDlgExtendedError();
		if(err == FNERR_BUFFERTOOSMALL)
		{
			AfxMessageBox("File Name buffer overrun, select fewer files.");
		}
	}
	delete[] mfBuf;
	chdir(theApp.dbDir);
}


// NASF CSV import:
// new 2014



int CImportDialog::ImportNASFFire(CString fileName, FILE *errLog)
{

	
	long defaultRegion = -1, defaultUnit = -1, defaultSubunit = -1;
	

    long errors = 0;
	long dupRejects = 0;
	
	
	fprintf(errLog, "**************************************************\nImport File Name: %s\n", fileName);
	 EnableWindow(FALSE);
 /*  CCancelDialog *cd = NULL;

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


	
	if(pDoc->HasTempFire())
		{
			try
			{
				pDoc->m_pDB->ExecuteSQL("DROP TABLE [ffpTempFire]");
			}
			catch(CDBException* e)
			{
				e->Delete();
			}
			//pDoc->m_pDB->DeleteTableDef("ffpTempFire");
		}
	pDoc->CreateTempFire();
	
	
	ifstream stream;

	stream.open(fileName);

	if (! stream.good())
		return -1;


	long updates = 0, rejects = 0;
    int lastRegionID = 0;
	int lastFireID = 0;
    
    char line[MAX_INPUT_LINE];
    char temp[MAX_INPUT_LINE];

    CString temp2, temp3;
    stringstream stream2; 
    
    
	char delim;

	delim = ',';//default to commma
	

	


    CList<CString, CString> staList;

	CFiresSet fires(pDoc->m_pDB);
	fires.m_strSort = "[FireNumber]";
	fires.Open();

	CFiresSet tfires(pDoc->m_pDB);
	tfires.m_strSort = "[FireNumber]";
	tfires.Open(CRecordset::dynaset, "ffpTempFire");

	int iDiscoveryDate = -1,
		iDiscoveryTime = -1,
		//iFirstAttackDate = -1,
		//iFirstAttackTime = -1,
		iRegionID =-1,
		iUnitID = -1,
		iNASFID = -1,   // NASF reporting unit
		iSubunitID = -1,
		iFireNumber = -1,
		iState = -1,
	    iStateID = -1,  // State FIPS
		iCounty = -1,
	    iCountyID = -1,  // County FIPS
		iTotalAcres = -1,
		iFireName = -1,
		iStatisticalCause = -1,
		iGeneralCause = -1,
		iSpecificCause = -1,
		//iClassPeople = -1,
		//iCauseNarr = -1,
		iTownship = -1,
		iRange = -1,
		iSection = -1,
		iSubSection= -1,
		iLatitude= -1,
		iLongitude= -1,
		iSlope= -1,
		iElevation= -1,
		iAspect= -1,
		//iFuelModel= -1,
		iFireType= -1,
		iContainDate= -1,
		iContainTime= -1,
		iStrategyMetDate= -1,
		iStrategyMetTime= -1,
		iFireOutDate= -1,
		iFireOutTime= -1;

           


	// first line contains field names
	int iField = 0;
	
	
	int lineNo = 1;

	stream.getline(line,MAX_INPUT_LINE); 
	string line2 = line;
	vector<string> vColNames = csv_read_row(line2, ',');;
	while (stream.good())
	{
		iFireNumber = getColIndex("Local Incident ID", vColNames);
		if (iFireNumber >= 0)
			break;
		stream.getline(line, MAX_INPUT_LINE);
		line2 = line;
		vColNames = csv_read_row(line2, ',');
		lineNo++;

	}
	iFireNumber = getColIndex("Local Incident ID", vColNames);
	iDiscoveryDate = getColIndex("Fire Discovery Date", vColNames);
	if(iDiscoveryDate < 0)
		iDiscoveryDate = getColIndex("Fire Date", vColNames);
	iFireName = getColIndex("Incident Name", vColNames);
	iDiscoveryTime = getColIndex("Fire Discovery Time", vColNames);
	iContainDate = getColIndex("Fire Containment Date", vColNames);
	if(iContainDate < 0)
		iContainDate = getColIndex("Containment Date", vColNames);
	iContainTime = getColIndex("Fire Containment Time", vColNames);
	if(iContainTime < 0)
		iContainTime = getColIndex("Containment Time", vColNames);
	iNASFID = getColIndex("Fire Reporting Agency Unit ID", vColNames);
	iSubunitID = getColIndex("District", vColNames);
	iLatitude = getColIndex("Latitude", vColNames);
	iLongitude = getColIndex("Longitude", vColNames);
	iStatisticalCause = getColIndex("Cause Code", vColNames);
	if(iStatisticalCause < 0)
		iStatisticalCause = getColIndex("Statistical Cause Code", vColNames);
	iTotalAcres = getColIndex("Total Acres Burned", vColNames);
	if(iTotalAcres < 0)
		iTotalAcres = getColIndex("Final Fire Acre Quantity", vColNames);
	iState = getColIndex("State", vColNames);
	iStateID = getColIndex("State FIPS", vColNames);
	iCounty = getColIndex("County", vColNames);
	iCountyID = getColIndex("County FIPS", vColNames);

	if (iFireNumber < 0 || iDiscoveryDate < 0 || iFireName < 0 || iDiscoveryTime < 0
		|| iContainDate < 0 || iContainTime < 0 || iNASFID < 0 || iSubunitID < 0
		|| iLatitude < 0 || iLongitude < 0 || iStatisticalCause < 0 || iTotalAcres < 0)
	{
		if (iFireNumber < 0)
			fprintf(errLog, "Error, field Local Incident ID not found in header\n");
		if (iDiscoveryDate < 0)
			fprintf(errLog, "Error, field Fire Discovery Date nor Fire Date not found in header\n");
		if (iDiscoveryTime < 0)
			fprintf(errLog, "Error, field Fire Discovery Time not found in header\n");
		if (iContainDate < 0)
			fprintf(errLog, "Error, field Fire Containment Date nor Containment Date not found in header\n");
		if (iContainTime < 0)
			fprintf(errLog, "Error, field Fire Containment Time nor Containment Time not found in header\n");
		if (iNASFID < 0)
			fprintf(errLog, "Error, field Fire Reporting Agency Unit ID not found in header\n");
		if (iSubunitID < 0)
			fprintf(errLog, "Error, field District not found in header\n");
		if (iLatitude < 0)
			fprintf(errLog, "Error, field Latitude not found in header\n");
		if (iLongitude < 0)
			fprintf(errLog, "Error, field Longitude not found in header\n");
		if (iStatisticalCause < 0)
			fprintf(errLog, "Error, field Cause Code nor Statistical Cause Code not found in header\n");
		if (iTotalAcres < 0)
			fprintf(errLog, "Error, field Total Acres Burned nor Final Fire Acre Quantity not found in header\n");
		if (iState < 0)
			fprintf(errLog, "Error, field State not found in header\n");
		if (iStateID < 0)
			fprintf(errLog, "Error, field State FIPS not found in header\n");
		if (iCounty < 0)
			fprintf(errLog, "Error, field County not found in header\n");
		if (iCountyID < 0)
			fprintf(errLog, "Error, field County FIPS not found in header\n");
		stream.close();
		return -2;
	}
	int maxID = iFireNumber;
	maxID = max(maxID, iDiscoveryDate);
	maxID = max(maxID, iDiscoveryTime);
	maxID = max(maxID, iContainDate);
	maxID = max(maxID, iContainTime);
	maxID = max(maxID, iNASFID);
	maxID = max(maxID, iSubunitID);
	maxID = max(maxID, iLatitude);
	maxID = max(maxID, iLongitude);
	maxID = max(maxID, iStatisticalCause);
	maxID = max(maxID, iTotalAcres);
	maxID = max(maxID, iState);
	maxID = max(maxID, iStateID);
	maxID = max(maxID, iCounty);
	maxID = max(maxID, iCountyID);
	string strLine;
	vector<string> vFields;
	while (stream.good())
	{
		stream.getline(line, MAX_INPUT_LINE);
		lineNo++;
		strLine = line;
		vFields = csv_read_row(strLine, ',');
		if (vFields.size() <= maxID)
		{
			fprintf(errLog, "Warning, line %d has too few fields, skipping record\n", lineNo);
			continue;
		}
		tfires.AddNew();
		COleDateTime dt;
		dt.ParseDateTime(vFields[iDiscoveryDate].c_str());
		if(dt.GetStatus() != COleDateTime::valid)
		{
			fprintf(errLog, "\tRecord Number %d, Discovery date is not valid.\n", lineNo);
			rejects++;
			continue;
		}
		int hour = 0, minute = 0;
		if (vFields[iDiscoveryTime].size() > 0)
		{
			if (vFields[iDiscoveryTime].size() <= 2)//hour only
			{
				hour = atoi(vFields[iDiscoveryTime].c_str());
			}
			else if (vFields[iDiscoveryTime].size() == 3 || vFields[iDiscoveryTime].size() > 5) {
				hour = 0;
			}
			else {   // normal format
				// HHMM
				char tdate[3];
				tdate[0] = vFields[iDiscoveryTime][0];
				tdate[1] = vFields[iDiscoveryTime][1];
				tdate[2] = '\0';
				hour = atoi(tdate);

				tdate[0] = vFields[iDiscoveryTime][2];
				tdate[1] = vFields[iDiscoveryTime][3];
				tdate[2] = '\0';
				minute = atoi(tdate);
			}
		}
		if (hour < 0 || hour > 23) {
			hour = 13;
		}
		if (minute < 0 || minute > 59) {
			minute = 0;
		}
		tfires.m_Discovery.SetDateTime(dt.GetYear(), dt.GetMonth(), dt.GetDay(), hour, minute, 0);
		COleDateTime today;
		today = COleDateTime::GetCurrentTime();
		if (tfires.m_Discovery > today)
		{
			fprintf(errLog, "\tRecord Number %ld, Discovery Date/Time is not valid (future): %d/%d/%04d\n", lineNo, dt.GetMonth(), dt.GetDay(), dt.GetYear());
			rejects++;

			continue;
		}
		if (strlen(vFields[iTotalAcres].c_str()) > 0)
			tfires.m_TotalAcres = atof(vFields[iTotalAcres].c_str());
		else
		{//error, Total Acres is required
			fprintf(errLog, "\tRecord Number %d, Missing required field: Final Fire Acre Quantity.\n", lineNo);
			rejects++;
			continue;
		}
		if (strlen(vFields[iFireName].c_str()) > 0)
		{
			char sFireName[MAX_FIELD_LEN];
			strcpy(sFireName, vFields[iFireName].c_str());
			sFireName[25] = 0;//safety truncate!

			// if we've replaced commas with semicolons, change them back
			for (int i = 0; i < strlen(sFireName); i++)
				if (sFireName[i] == ';')
					sFireName[i] = ',';


			tfires.m_FireName = sFireName;
			tfires.m_FireName.Trim();
		}
		else
			tfires.SetFieldNull(&tfires.m_FireName);
		if (strlen(vFields[iStatisticalCause].c_str()) > 0)
		{
			long testVal = atol(vFields[iStatisticalCause].c_str());
			if (testVal >= 10 && testVal <= 13)
				testVal = 9;//assign to misc
			if (testVal < 1 || testVal > 9)
			{
				fprintf(errLog, "\tRecord Number %d, Invalid field: Statistical Cause Code'%s'.\n", lineNo, vFields[iStatisticalCause].c_str());
				rejects++;
				continue;

			}
			else
				tfires.m_StatisticalCause = testVal;
		}
		else
		{
			//error, cause is required
			fprintf(errLog, "\tRecord Number %d, Missing required field: Statistical Cause Code.\n", lineNo);
			rejects++;
			continue;
		}
		if (strlen(vFields[iLatitude].c_str()) > 0)
			tfires.m_latitude = atof(vFields[iLatitude].c_str());
		else
			tfires.SetFieldNull(&tfires.m_latitude);
		if (strlen(vFields[iLongitude].c_str()) > 0)
			tfires.m_longitude = atof(vFields[iLongitude].c_str());
		else
			tfires.SetFieldNull(&tfires.m_longitude);

		COleDateTime containDt;
		if (strlen(vFields[iContainDate].c_str()) > 0)
		{
			containDt.ParseDateTime(vFields[iContainDate].c_str());
			if (containDt.GetStatus() != COleDateTime::valid)
			{
				fprintf(errLog, "\tRecord Number %d, Fire Containment Date is not valid.\n", lineNo);
				rejects++;
				continue;
			}
			int hour = 0, minute = 0;
			if (vFields[iContainTime].size() > 0)
			{
				if (vFields[iContainTime].size() <= 2)//hour only
				{
					hour = atoi(vFields[iContainTime].c_str());
				}
				else if (vFields[iContainTime].size() == 3 || vFields[iContainTime].size() > 5) {
					hour = 0;
				}
				else {   // normal format
					// HHMM
					char tdate[3];
					tdate[0] = vFields[iContainTime][0];
					tdate[1] = vFields[iContainTime][1];
					tdate[2] = '\0';
					hour = atoi(tdate);

					tdate[0] = vFields[iContainTime][2];
					tdate[1] = vFields[iContainTime][3];
					tdate[2] = '\0';
					minute = atoi(tdate);
				}
			}
			if (hour < 0 || hour > 23) {
				hour = 13;
			}
			if (minute < 0 || minute > 59) {
				minute = 0;
			}
			tfires.m_Contain.SetDateTime(containDt.GetYear(), containDt.GetMonth(), containDt.GetDay(), hour, minute, 0);
			COleDateTime today;
			today = COleDateTime::GetCurrentTime();
			if (tfires.m_Discovery > today)
			{
				fprintf(errLog, "\tRecord Number %ld, Fire Containment Date/Time is not valid (future): %d/%d/%04d\n", lineNo, containDt.GetMonth(), containDt.GetDay(), containDt.GetYear());
				rejects++;

				continue;
			}
			tfires.m_StrategyMet = tfires.m_Contain;
		}
		else
		{
			tfires.SetFieldNull(&tfires.m_Contain);
			tfires.SetFieldNull(&tfires.m_StrategyMet);
		}

		tfires.SetFieldNull(&tfires.m_AgencyID);
		tfires.SetFieldNull(&tfires.m_RegionID);
		tfires.SetFieldNull(&tfires.m_UnitID);
		tfires.SetFieldNull(&tfires.m_SubunitID);


		if (strlen(vFields[iFireNumber].c_str()) > 0)
		{
			//first we need to find reginID and unitID, and add if necessary
			int agencyID = 6;
			int regionID = -1, unitID = -1;
			if (vFields[iCountyID].size() < 1 && vFields[iCounty].size() < 1)
			{
				fprintf(errLog, "\tRecord Number %ld, Missing County and County FIPS\n", lineNo);
				rejects++;
				continue;
			}
			if (vFields[iState].size() < 1)
			{
				fprintf(errLog, "\tRecord Number %ld, Invalid state\n", lineNo);
				rejects++;

				continue;
			}
			char sState[MAX_FIELD_LEN];
			char sRegionID[MAX_FIELD_LEN];
			if (vFields[iState].size() < 3)
			{
				CStateSet stateSet(pDoc->m_pDB);
				stateSet.m_strFilter.Format("[State] = '%s'", vFields[iState].c_str());
				stateSet.Open();
				if (!stateSet.IsEOF())
				{

					sprintf(sState, "%s", stateSet.m_StateName);

				}
				stateSet.Close();
			}
			else
				strcpy(sState, vFields[iState].c_str());
			// get NASF regionID based on state

			CFireRegionSet regSet(pDoc->m_pDB);
			if (strlen(sState) > 2)
				regSet.m_strFilter.Format("[AgencyID]=6 AND [Name] LIKE '%% %s%%'", sState);
			else
				regSet.m_strFilter.Format("[AgencyID]=6 AND [Name] LIKE '%s %%'", sState);
			regSet.Open();
			if (!regSet.IsEOF()) 
			{
				sprintf(sRegionID, "%d", regSet.m_RegionID);
				lastRegionID = regSet.m_RegionID;
			}
			regSet.Close();
			if (strlen(sRegionID) < 1)
			{
				fprintf(errLog, "\tRecord Number %ld, Invalid Region for state: %s\n", lineNo, sState);
				rejects++;
				continue;
			}
			CFireUnitSet unitSet(pDoc->m_pDB);
			// first search for the county
			// reduce the county name to its first word
			// to eliminate problems with "Lewis and Clark" vs. "Lewis & Clark", etc.
			//
			char sCounty[MAX_FIELD_LEN];
			char sCountyID[MAX_FIELD_LEN];
			if (vFields[iCounty].size() > 0)
				strcpy(sCounty, vFields[iCounty].c_str());
			else
				sCounty[0] = 0;
			if (vFields[iCountyID].size() > 0)
			{
				if (vFields[iCountyID].size() == 3 && vFields[iCountyID][0] == '0')//strip it
				{
					sCountyID[0] = vFields[iCountyID][1];
					sCountyID[1] = vFields[iCountyID][2];
					sCountyID[2] = 0;
					if (sCountyID[0] == '0')
					{
						sCountyID[0] = sCountyID[1];
						sCountyID[1] = 0;
						if (sCountyID[0] == '0')
							sCounty[0] = 0;
					}
					//strcpy(sCountyID, vFields[iCountyID].)
				}
				else
					strcpy(sCountyID, vFields[iCountyID].c_str());
			}
			else
				sCountyID[0] = 0;
			char county1[80];
			sprintf(county1, "%s", sCounty);
			for (int i = 0; i < strlen(county1); i++)
				if (county1[i] == ' ' || county1[i] == '\'')
					county1[i] = '\0';


			if (strlen(sCountyID) > 0 && strlen(sCounty) > 0)
				unitSet.m_strFilter.Format("[Name] LIKE '%s %s%%'", sCountyID, county1); //,sCountyID,sCounty);
			else if (strlen(sCounty) > 0)
				unitSet.m_strFilter.Format("[RegionID] = %s AND [Name] LIKE '%% %s%%'", sRegionID, county1);

			unitSet.Open();
			char sUnitID[MAX_FIELD_LEN];
			bool unitFound = false;
			if (!unitSet.IsEOF()) {

				sprintf(sUnitID, "%d", unitSet.m_UnitID);

				lastRegionID = unitSet.m_RegionID;

				unitFound = true;
			}
			else if (strlen(sCountyID) > 0)
			{

				unitSet.Close();
				unitSet.m_strFilter.Format("[RegionID] = %s AND [Name] LIKE '%s %%'", sRegionID, sCountyID);
				unitSet.Open();

				if (!unitSet.IsEOF()) {

					sprintf(sUnitID, "%d", unitSet.m_UnitID);
					lastRegionID = unitSet.m_RegionID;
					unitFound = true;
				}


			}
			unitSet.Close();
			if (!unitFound || strlen(sUnitID) < 1)
			{
				fprintf(errLog, "\tRecord Number %ld, Invalid County (%s) or FIPS (%s)\n", lineNo, sCounty, sCountyID);
				//fprintf(errLog, "\tRecord Number %ld, Invalid County or FIPS for '%s'\n", count, sNASFID);
				rejects++;

				continue;
			}
			unitID = atoi(sUnitID);
			regionID = atoi(sRegionID);


			char sFireNumber[MAX_FIELD_LEN];

			strcpy(sFireNumber, vFields[iFireNumber].c_str());
			sFireNumber[25] = 0;//safety truncate!
			tfires.m_FireNumber.Format("%d%02d%02d-%ld-%ld-%ld-0-%s", dt.GetYear(), dt.GetMonth(), dt.GetDay(), agencyID, regionID, unitID, sFireNumber);// = str;
			tfires.m_FireNumber.Trim();
			if (tfires.m_FireNumber.GetLength() > 50)
				tfires.m_FireNumber.SetAt(50, 0);
			tfires.m_RegionID = (long)regionID;
			tfires.m_AgencyID = (long)agencyID;
			tfires.m_UnitID = (long)unitID;
			tfires.m_SubunitID = 0;
			tfires.SetFieldNull(&tfires.m_SubunitID);
		}
		else
		{//error, Fire number is required
			fprintf(errLog, "\tRecord Number %d, Missing required field: Local Incident ID.\n", lineNo);
			rejects++;
			continue;
		}
		tfires.Update();

	}
		// stream2 << line;
	//stream2.getline(temp,MAX_FIELD_LEN,delim); 
	//test for BOM dammit
	/*if (temp[0] < 0)
	{
		std::string s1 = temp;
		std::string s = ltrim(s1);
		strcpy(temp, s.c_str());
	}
	while (temp[0] > 0 && temp[0] != '\n')
	{
		
		if (temp[0] > 0 && temp[0] != '\n')
		{
			if(strstr(temp,"Local Incident ID"))
				iFireNumber = iField;
			else if(strstr(temp,"Fire Discovery Date") || 
				   strstr(temp,"Fire Date") )
				iDiscoveryDate = iField;
			else if(strstr(temp,"Incident Name"))
				iFireName = iField;
			else if(strstr(temp,"Fire Discovery Time"))
				iDiscoveryTime = iField;
			else if(strstr(temp,"Fire Containment Date") ||
				strstr(temp,"Containment Date"))
				iContainDate = iField;
			else if(strstr(temp,"Fire Containment Time") || 
				strstr(temp,"Containment Time"))
				iContainTime = iField;
			else if(strstr(temp,"Fire Reporting Agency Unit ID"))
				iNASFID = iField;
			else if(strstr(temp,"State FIPS"))
				iStateID = iField;			
			else if(strstr(temp,"County FIPS"))
				iCountyID = iField;			
			else if(strstr(temp,"District"))
				iSubunitID = iField;
			else if(strstr(temp,"Latitude"))
				iLatitude = iField;
			else if(strstr(temp,"Longitude"))
				iLongitude = iField;
			else if(strstr(temp,"Cause Code") || 
				strstr(temp,"Statistical Cause Code"))
				iStatisticalCause = iField;
			else if(strstr(temp,"Final Fire Acre") || 
				strstr(temp,"Total Acres Burned"))
				iTotalAcres = iField;
			else if(strstr(temp,"State"))
				iState = iField;
			else if(strstr(temp,"County"))
				iCounty = iField;
	

			iField ++;
			// get next field
			stream2.getline(temp,MAX_FIELD_LEN,delim); 
		}
		
		

	}
	int numFields = iField;*/

	// subsequent lines are comma-separated data
	// '#' denotes a commented line to be skipped
/*	while (stream.good())// && (!cd || ! cd->Abort()))
	{

		 
	   	char sDiscoveryDate[MAX_FIELD_LEN] = "",
		   sDiscoveryTime[MAX_FIELD_LEN] = "",
		   //sFirstAttackDate[MAX_FIELD_LEN] = "",
		   //sFirstAttackTime[MAX_FIELD_LEN] = "",
		   //sAgencyID[MAX_FIELD_LEN] = "",
		   sRegionID[MAX_FIELD_LEN] = "",
		   sUnitID[MAX_FIELD_LEN] = "",
		   sSubunitID[MAX_FIELD_LEN] = "",
		   sNASFID[MAX_FIELD_LEN] = "",
		   sFireNumber[MAX_FIELD_LEN] = "",
		  // sState[MAX_FIELD_LEN] = "",
		  // sCounty[MAX_FIELD_LEN] = "",
		  // sStateID[MAX_FIELD_LEN] = "",
		  // sCountyID[MAX_FIELD_LEN] = "",
		   sTotalAcres[MAX_FIELD_LEN] = "",
		   sFireName[MAX_FIELD_LEN] = "",
		   sStatisticalCause[MAX_FIELD_LEN] = "",
		   sGeneralCause[MAX_FIELD_LEN] = "",
		   sSpecificCause[MAX_FIELD_LEN] = "",
		  // sClassPeople[MAX_FIELD_LEN] = "",
		   //sCauseNarr[MAX_FIELD_LEN] = "",
		   //sTownship[MAX_FIELD_LEN] = "",
		   //sRange[MAX_FIELD_LEN] = "",
		   //sSection[MAX_FIELD_LEN] = "",
		   //sSubSection[MAX_FIELD_LEN] = "",
		   sLatitude[MAX_FIELD_LEN] = "",
		   sLongitude[MAX_FIELD_LEN] = "",
		   //sSlope[MAX_FIELD_LEN] = "",
		   //sElevation[MAX_FIELD_LEN] = "",
		   //sAspect[MAX_FIELD_LEN] = "",
		   //sFuelModel[MAX_FIELD_LEN] = "",
		   sFireType[MAX_FIELD_LEN] = "",
		   sContainDate[MAX_FIELD_LEN] = "",
		   sContainTime[MAX_FIELD_LEN] = "",
		   sStrategyMetDate[MAX_FIELD_LEN] = "",
		   sStrategyMetTime[MAX_FIELD_LEN] = "",
		   sFireOutDate[MAX_FIELD_LEN] = "",
		   sFireOutTime[MAX_FIELD_LEN] = "",
           sJunk[MAX_INPUT_LINE] = "";

	   
	   
	    stream.getline(line,MAX_INPUT_LINE); 
		stringstream stream3;
		stream3 << line;
		stream3.getline(temp,MAX_INPUT_LINE,delim); 
        
		int iField = 0;
		while (iField < numFields && temp[0] != '\n')
		{
		
		


			// 2014 - quick scan for quoted records
			bool inQuote = false;
			for (int i=0; i <strlen(temp);i++)
			{
				if (temp[i] == '\0')
				exit;

				if (temp[i] == '"')
					inQuote = !inQuote;
			
				// change embedded comma to semicolon
				if (temp[i] == ',' && inQuote)
					temp[i] = ';';

			}
		

			if (!(temp[0] == 0 || temp[0] == '#' || temp[0] == '\n' || temp[0] == '\r')) // not a valid data field
			{

		
		    if (iDiscoveryDate == iField)
				sprintf(sDiscoveryDate,"%s",temp);
			else if (iDiscoveryTime == iField)
				sprintf(sDiscoveryTime,"%s",temp);
			//else if (iFirstAttackDate == iField)
            //    sprintf(sFirstAttackDate,"%s",temp);
			//else if (iFirstAttackTime == iField)
		    //    sprintf(sFirstAttackTime,"%s",temp);
			else if (iRegionID == iField)
                sprintf(sRegionID,"%s",temp);
			else if (iUnitID == iField)
				sprintf(sUnitID,"%s",temp);
			else if (iNASFID == iField)
				sprintf(sNASFID,"%s",temp);
			else if (iSubunitID == iField)
				sprintf(sSubunitID,"%s",temp);
			else if (iFireNumber == iField)
				sprintf(sFireNumber,"%s",temp);
		    //else if (iState == iField)
			//	sprintf(sState,"%s",temp);
		    //else if (iCounty == iField)
			//	sprintf(sCounty,"%s",temp);
			//else if (iStateID == iField)
			//	sprintf(sStateID,"%s",temp);
		    else if (iTotalAcres == iField)
				sprintf(sTotalAcres,"%s",temp);
		    else if (iFireName == iField)
				sprintf(sFireName,"%s",temp);
		    else if (iStatisticalCause == iField)
				sprintf(sStatisticalCause,"%s",temp);
		    else if (iGeneralCause == iField)
				sprintf(sGeneralCause,"%s",temp);
		    else if (iSpecificCause == iField)
				sprintf(sSpecificCause,"%s",temp);
		    else if (iFireType == iField)
				sprintf(sFireType,"%s",temp);
		    else if (iContainDate == iField)
				sprintf(sContainDate,"%s",temp);
		    else if (iContainTime == iField)
			    sprintf(sContainTime,"%s",temp);
			else if (iStrategyMetDate == iField)
				sprintf(sStrategyMetDate,"%s",temp);
			else if (iStrategyMetTime == iField)
				sprintf(sStrategyMetTime,"%s",temp);
		    else if (iFireOutDate == iField)
				sprintf(sFireOutDate,"%s",temp);
		    else if (iFireOutTime == iField)
				sprintf(sFireOutTime,"%s",temp);
			}
		
		   iField ++;

		   stream3.getline(temp,MAX_INPUT_LINE,delim); 

		}
		




		
		count ++;
       // if (cd)
		//   cd->Increment();

 		COleDateTime discoveryDate;  

		if (strlen(sFireNumber) < 1){
            fprintf(errLog, "\tRecord Number %d, no FireNumber\n", count );
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
	
		  // MM/DD/YYYY
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

		if (strlen(sDiscoveryTime)==3)
		{
			 sprintf(temp,"0%s",sDiscoveryTime);
			 sprintf(sDiscoveryTime,temp);
		}

		if (strlen(sDiscoveryTime) > 0){
           if (strlen(sDiscoveryTime) <= 2)
			   hr = atoi(sDiscoveryTime);
		   else if (strlen(sDiscoveryTime)==3 || strlen(sDiscoveryTime) > 5) {
              hr = 0;
		   } else {   // normal format
			  // HHMM
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
		if (hr < 0 || hr > 23){
              hr = 13;
		}
		if (min < 0 || min > 59){
		      min = 0;
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
		

		//tfires.m_Year.Format("%d",y);

		//if (strlen(sFireID) > 0)
		//tfires.m_FireID = lastFireID;//atol(sFireID);
		//else
			//tfires.SetFieldNull(&tfires.m_FireID);

		tfires.m_Discovery = discoveryDate;

        tfires.SetFieldNull(&tfires.m_AgencyID);
		tfires.SetFieldNull(&tfires.m_RegionID);
		tfires.SetFieldNull(&tfires.m_UnitID);
		tfires.SetFieldNull(&tfires.m_SubunitID);

		bool unitFound = false;
		//Region, Unit, Subunit check
		// 2014: moved this to the end of processing, so we don't add new units/subunits for rejected records
		//
		int agencyID = 6;
		int regionID = -1;
		int unitID = -1;
		int subunitID = -1;

		// look up unit/region based on county FIPS or county
		// ideally both fields should have values, but we'll 
		// try to build a query if just one is empty
		if (strlen(sCountyID) < 1 && strlen(sCounty) < 1)
		{
		   if(strlen(sCountyID) < 1)
			{
				fprintf(errLog, "\tRecord Number %ld, Missing County or FIPS\n",count);
				rejects ++;
			
				continue;
			}
		
		}

		if(strlen(sState) < 1)
		{
			fprintf(errLog, "\tRecord Number %ld, Invalid state\n",count,sState);
			rejects ++;
			
			continue;
		}
		char sStateName[80];

		// if it's a state abbreviation, find state name
		if (strlen(sState) < 3)
		{
			CStateSet stateSet(pDoc->m_pDB);
			stateSet.m_strFilter.Format("[State] = '%s'", sState);
			stateSet.Open();
			if (! stateSet.IsEOF())
			{

				   sprintf(sStateName,"%s", stateSet.m_StateName);

			}
			stateSet.Close();

		}

	
		
        // get NASF regionID based on state

		CFireRegionSet regSet(pDoc->m_pDB);
		if (strlen(sStateName) > 2)
		   regSet.m_strFilter.Format("[AgencyID]=6 AND [Name] LIKE '%% %s%%'", sStateName);
		else
		  regSet.m_strFilter.Format("[AgencyID]=6 AND [Name] LIKE '%s %%'", sState);
		regSet.Open();
		if (! regSet.IsEOF()){
            sprintf(sRegionID,"%d",regSet.m_RegionID);
		  
		    lastRegionID = regSet.m_RegionID;

		    
		 }
		regSet.Close();
		
	
			if(strlen(sRegionID) < 1)
				{
					fprintf(errLog, "\tRecord Number %ld, Invalid Region for state: %s\n",count,sState);
					rejects ++;
			
					continue;
				}


                CFireUnitSet unitSet(pDoc->m_pDB);

				// first search for the county
				// reduce the county name to its first word
				// to eliminate problems with "Lewis and Clark" vs. "Lewis & Clark", etc.
				//
				char county1[80];
				sprintf(county1,"%s",sCounty);
				for (int i=0; i < strlen(county1); i++)
					if (county1[i]==' ')
						county1[i] = '\0';

				
				if (strlen(sCountyID) > 0 && strlen(sCounty) > 0)
                    unitSet.m_strFilter.Format("[Name] LIKE '%s %s%%'",sCountyID,county1); //,sCountyID,sCounty);
				else if (strlen(sCounty) > 0)
					unitSet.m_strFilter.Format("[RegionID] = %s AND [Name] LIKE '%% %s'", sRegionID, county1);
				
                unitSet.Open();
			
				if (! unitSet.IsEOF()){
                
				   sprintf(sUnitID, "%d", unitSet.m_UnitID);
				   
				   lastRegionID = unitSet.m_RegionID;

				   unitFound = true;
				} else if (strlen(sCountyID) > 0)
				{

					unitSet.Close();
					unitSet.m_strFilter.Format("[RegionID] = %s AND [Name] LIKE '%s %%'", sRegionID,sCountyID);
					unitSet.Open();
			
					if (! unitSet.IsEOF()){
                
						sprintf(sUnitID, "%d", unitSet.m_UnitID);				   
						lastRegionID = unitSet.m_RegionID;
						unitFound = true;
					}


				}
				unitSet.Close();


			
				if(! unitFound || strlen(sUnitID) < 1 )
				{
					fprintf(errLog, "\tRecord Number %ld, Invalid County or FIPS for '%s'\n",count,sNASFID);
					rejects ++;
			
					continue;
				}
				unitID = atoi(sUnitID);
				regionID = atoi(sRegionID);
	
		tfires.m_RegionID = (long) regionID;
		tfires.m_AgencyID  = (long) agencyID;
		tfires.m_UnitID = (long) unitID;
		tfires.m_SubunitID = (long) subunitID;
		tfires.SetFieldNull(&tfires.m_SubunitID);
     

		
		if (strlen(sFireNumber) > 0)
		{
			sFireNumber[25] = 0;//safety truncate!
			tfires.m_FireNumber.Format("%d%02d%02d-%ld-%ld-%ld-0-%s", discoveryDate.GetYear(), discoveryDate.GetMonth(), discoveryDate.GetDay(), agencyID, regionID, unitID, sFireNumber);// = str;
			//tfires.m_FireNumber = sFireNumber;
			tfires.m_FireNumber.Trim();
		} 
		else
		{//error, Fire number is required
			fprintf(errLog, "\tRecord Number %d, Missing required field: Fire Number.\n", count );
				rejects ++;
				continue;
		}

		if (strlen(sState) > 0)
		{
			sState[2] = 0;//safety truncate!
			tfires.m_State = sState;
			tfires.m_State.Trim();
		} 
		else
            tfires.SetFieldNull(&tfires.m_State);

		if (strlen(sCountyID) > 0)
			tfires.m_County = atol(sCountyID);
		else
            tfires.SetFieldNull(&tfires.m_County);
		
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
           
		if (strlen(sLatitude) > 0)
		{
			tfires.m_latitude = atof(sLatitude);
		} 
		else 
		{
			   tfires.SetFieldNull(&tfires.m_latitude);
			  // tfires.SetFieldNull(&tfires.m_LatMM);
			  // tfires.SetFieldNull(&tfires.m_LatSS);
		}
		



		if (strlen(sLongitude) > 0)
		{
			tfires.m_longitude = atof(sLongitude);
		} 
		else 
		{
			   tfires.SetFieldNull(&tfires.m_longitude);
		}
		



		COleDateTime containDate;  

		if (strlen(sContainDate) > 0 ){

		   // format date & time properly
           int y=0,
			m=0,
			d=0,
			hr=13,
			min=0;


           char tdate[5];
		  

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
		   else if (strlen(sContainTime) > 5) {
               hr = 0;
		   } else {   // normal format
			  
			   if (strlen(sContainTime)==3)
			   {
				  tdate[0] = sContainTime[0];
                  tdate[1] = '\0';
		          hr = atoi(tdate);

				  tdate[0] = sContainTime[1];
		          tdate[1] = sContainTime[2];
                  tdate[2] = '\0';
		          min = atoi(tdate);

			   } else 
			   {
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
              hr = 13;
		}
		if (min < 0 || min > 59){
		     min = 0;
		}

	
		containDate.SetDateTime(y, m, d, hr,min, 0);
		tfires.SetFieldNull(&tfires.m_Contain);

		if(containDate.GetStatus() != 0 || y < 1900)
		{
			tfires.SetFieldNull(&tfires.m_Contain);
		}
		else
		{
			COleDateTime today;
			today = COleDateTime::GetCurrentTime();
			if(containDate > today)
			{
				fprintf(errLog, "\tRecord Number %ld, Contain Date/Time is not valid (future): %d/%d/%04d\n", count,  m, d, y);
				rejects ++;
			
				continue;
			}
			 tfires.m_Contain = containDate;

		}
		}


		COleDateTime StrategyMetDate;  

		if (strlen(sStrategyMetDate) > 0 ){

		   // format date & time properly
           int y=0,
			m=0,
			d=0,
			hr=13,
			min=0;


           char tdate[5];
		 
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

		if (strlen(sStrategyMetTime)==3)
		{
			 sprintf(temp,"0%s",sStrategyMetTime);
			 sprintf(sStrategyMetTime,temp);
		}

		if (strlen(sStrategyMetTime) > 0){
           if (strlen(sStrategyMetTime) <= 2)
			   hr = atoi(sStrategyMetTime);
		   else if (strlen(sStrategyMetTime)==3 || strlen(sStrategyMetTime) > 5) {
               hr = 13;
		   } else {   // normal format
			  
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
		if (hr < 0 || hr > 23){
            hr = 13;
		}
		if (min < 0 || min > 59){
		     min = 0;
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



		
		
	
		//tfires.m_FireID = lastFireID;
		tfires.Update();	
		
    
	}
	*/
		
	

	stream.close();
	
	 long appends = 0, edits = 0;

	 count = 0;
   	COleDateTime lastDay;
	   if(tfires.IsBOF() && tfires.IsEOF())
	   {//error, no valid fires
			fprintf(errLog, "\tNo valid fires to import!\n");

	   }
	   else
	   {
			tfires.MoveFirst();
		   
			
			while(!tfires.IsEOF())// && (!cd || !cd->Abort()))
			{
				count++;

				if(tfires.IsFieldNull(&tfires.m_UnitID))//no unitID, check for dups at region level
					fires.m_strFilter.Format("[AgencyID]=%ld AND [RegionID]=%ld AND [FireNumber]='%s'", 
						tfires.m_AgencyID, tfires.m_RegionID, tfires.m_FireNumber);
				else//check for dups at unit level
					fires.m_strFilter.Format("[AgencyID]=%ld AND [RegionID]=%ld AND [UnitID]=%ld AND [FireNumber]='%s'", 
						tfires.m_AgencyID, tfires.m_RegionID, tfires.m_UnitID, tfires.m_FireNumber);

				fires.Requery();


				int newRec = false;
				int m_RejectDups = false;

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

				if (newRec)
					//if (! tfires.IsFieldNull(&tfires.m_FireID))				
					//	fires.m_FireID = tfires.m_FireID;
				
				if (! tfires.IsFieldNull(&tfires.m_Discovery))
					fires.m_Discovery = tfires.m_Discovery;

				//if (! tfires.IsFieldNull(&tfires.m_FirstAttack))
				//	fires.m_FirstAttack = tfires.m_FirstAttack;
				//if(newRec && tfires.IsFieldNull(&tfires.m_FirstAttack))
					//fires.SetFieldNull(&fires.m_FirstAttack);

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

				//if (! tfires.IsFieldNull(&tfires.m_Year))
	   			//	fires.m_Year = tfires.m_Year;
				//if(newRec && tfires.IsFieldNull(&tfires.m_Year))
				//	fires.SetFieldNull(&fires.m_Year);

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

				if (!tfires.IsFieldNull(&tfires.m_latitude))
					fires.m_latitude = tfires.m_latitude;
				if (newRec && tfires.IsFieldNull(&tfires.m_latitude))
					fires.SetFieldNull(&fires.m_latitude);
				if (!tfires.IsFieldNull(&tfires.m_longitude))
					fires.m_longitude = tfires.m_longitude;
				if (newRec && tfires.IsFieldNull(&tfires.m_longitude))
					fires.SetFieldNull(&fires.m_longitude);
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
					fires.SetFieldNull(&fires.m_SubSection);

				if (! tfires.IsFieldNull(&tfires.m_LatDD))
					fires.m_LatDD = tfires.m_LatDD;
				if(newRec && tfires.IsFieldNull(&tfires.m_LatDD))
					fires.SetFieldNull(&fires.m_LatDD);

				if (! tfires.IsFieldNull(&tfires.m_LatMM))
					fires.m_LatMM = tfires.m_LatMM;
				if(newRec && tfires.IsFieldNull(&tfires.m_LatMM))
					fires.SetFieldNull(&fires.m_LatMM);

				if (! tfires.IsFieldNull(&tfires.m_LatSS))
					fires.m_LatSS = tfires.m_LatSS;
				if(newRec && tfires.IsFieldNull(&tfires.m_LatSS))
					fires.SetFieldNull(&fires.m_LatSS);

				if (! tfires.IsFieldNull(&tfires.m_LonDD))
					fires.m_LonDD = tfires.m_LonDD;
				if(newRec && tfires.IsFieldNull(&tfires.m_LonDD))
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

				/*if (!tfires.IsFieldNull(&tfires.m_FireOut))
					fires.m_FireOut = tfires.m_FireOut;
				if(newRec && tfires.IsFieldNull(&tfires.m_FireOut))
					fires.SetFieldNull(&fires.m_FireOut);*/



				fires.Update();
				//LogFireDataWarnings(errLog, &tfires);
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
		
			fprintf(errLog, "Processed %ld fire records :\n"
	
				"\tAppended %ld records.\n"
				"\tUpdated %ld existing records.\n"
				"\tRejected %ld records for bad data.\n",
				count, appends, edits,  rejects );
		
		errors = rejects;
	/*}
	else
	{
		errors = -1;
	}*/

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
	
	//if (cd)
	//	cd->DestroyWindow();

	try
	{
		pDoc->m_pDB->ExecuteSQL("DROP TABLE [ffpTempFire]");
	}
	catch(CDBException* e)
	{
		e->Delete();
	}


	EnableWindow(TRUE);
 


	return errors;

}
void CImportDialog::OnNASFFires()
{
	long agencyID = 6;
	

	chdir(theApp.userDir);

	CFileDialog	dlg (TRUE, _T("NASF Fires"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
		OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
		_T("NASF Fire Files (*.txt;*.csv)|*.txt;*.csv|All Files (*.*)|*.*||"), this);
	char *mfBuf = new char[MF_BUFSIZE];
	mfBuf[0] = 0;
	dlg.m_ofn.lpstrFile = mfBuf;
	dlg.m_ofn.nMaxFile = MF_BUFSIZE;
	int res = dlg.DoModal();

	if (res == IDOK)
	{
		CWaitCursor wait;
		long errors = 0;
		// Get the position of the first filename
		char *logFileName = GetTempFileName();
		FILE * logFile = fopen(logFileName, "wt");
		COleDateTime dateTime = COleDateTime::GetCurrentTime();
		fprintf(logFile, "FireFamily Plus NASF Fire Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
		POSITION filenamePosition = dlg.GetStartPosition();
		CString fileName;

		// Open each file selected
		int tErrors;
		bool didSomething = false;
		while ( filenamePosition != NULL )
		{
			fileName = dlg.GetNextPathName( filenamePosition );
			//tErrors = ImportGenericWx(fileName, logFile);
			tErrors = ImportNASFFire(fileName,logFile);
			

			if(tErrors >= 0)
			{
				errors += tErrors;
				didSomething = true;
			} else
			{
				//user cancelled
				//errors = -1;
				break;
			}
			
		}
		//if(errors >= 0)
		//{
		if (!didSomething)//res == IDCANCEL)
		{
			fclose(logFile);
			delete[] logFileName;
		} else {
			dateTime = COleDateTime::GetCurrentTime();
			CString strMsg;
			if(errors >= 0)
			{
				fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
				strMsg.Format("Import Complete.\n%ld errors were encountered.", errors);
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
			theApp.SetUserDir(dlg.GetPathName());
		}
	}
	else if(res == IDCANCEL)
	{
		int err = CommDlgExtendedError();
		if(err == FNERR_BUFFERTOOSMALL)
		{
			AfxMessageBox("File Name buffer overrun, select fewer files.");
		}
	}
	delete[] mfBuf;
	chdir(theApp.dbDir);

}

void CImportDialog::OnGenericFires()
{
	long agencyID = -1;
	if(agencies.GetCurSel() != CB_ERR)
		agencyID = agencies.GetItemData(agencies.GetCurSel());
	else
	{//report no agency selected
		CString strMsg = "An Agency must be selected in order to import fire data.";
		AfxMessageBox(strMsg);
		return;
	}

	chdir(theApp.userDir);

	CFileDialog	dlg (TRUE, _T("GenericWx"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
		OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
		_T("Generic Fire Import Text Files (*.txt;*.csv)|*.txt;*.csv|All Files (*.*)|*.*||"), this);
	char *mfBuf = new char[MF_BUFSIZE];
	mfBuf[0] = 0;
	dlg.m_ofn.lpstrFile = mfBuf;
	dlg.m_ofn.nMaxFile = MF_BUFSIZE;
	int res = dlg.DoModal();

	if (res == IDOK)
	{
		CWaitCursor wait;
		long errors = 0;
		// Get the position of the first filename
		char *logFileName = GetTempFileName();
		FILE * logFile = fopen(logFileName, "wt");
		COleDateTime dateTime = COleDateTime::GetCurrentTime();
		fprintf(logFile, "FireFamily Plus Generic Fire Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
		POSITION filenamePosition = dlg.GetStartPosition();
		CString fileName;

		// Open each file selected
		int tErrors;
		bool didSomething = false;
		while ( filenamePosition != NULL )
		{
			fileName = dlg.GetNextPathName( filenamePosition );
			//tErrors = ImportGenericWx(fileName, logFile);
			CGenericFireImportDialog gwd(this,NULL,firesSet.m_pDatabase, agencyID);
			gwd.fileName.Format("%s",fileName);
			gwd.errLog = logFile;
			gwd.pDoc = pDoc;
			res = gwd.DoModal();
			tErrors = gwd.errors;

			if(tErrors >= 0)
				errors += tErrors;
			else
			{//user cancelled
				//errors = -1;
				break;
			}
			if(res != IDCANCEL)
				didSomething = true;
		}
		//if(errors >= 0)
		//{
		if (!didSomething)//res == IDCANCEL)
		{
			fclose(logFile);
			delete[] logFileName;
		} else {
			dateTime = COleDateTime::GetCurrentTime();
			CString strMsg;
			if(errors >= 0)
			{
				fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
				strMsg.Format("Import Complete.\n%ld errors were encountered.", errors);
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
			theApp.SetUserDir(dlg.GetPathName());
		}
	}
	else if(res == IDCANCEL)
	{
		int err = CommDlgExtendedError();
		if(err == FNERR_BUFFERTOOSMALL)
		{
			AfxMessageBox("File Name buffer overrun, select fewer files.");
		}
	}
	delete[] mfBuf;
	chdir(theApp.dbDir);
}

void CImportDialog::OnWFMI()
{
	/* FIXME LJS TEMPORARY CODE HERE to populate the DOI unit codes table
	*/

	CDOIUnitSet doiSet(firesSet.m_pDatabase);
	doiSet.Open();
	if (doiSet.IsEOF()){  // 8/2006 - should not get here, as the ffp v4 upgrade routine now populates the refDOIUnits table....
		AfxMessageBox("This database does not contain any DOI unit codes. \nUnit codes must be imported from a text file.\nPlease contact the FFP helpdesk for further assistance.");

		OnWFMICodes();
	}

	chdir(theApp.userDir);
	CFileDialog	dlg (TRUE, _T("WFMI"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
		OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
		_T("WFMI/DOI Fire Import TextFiles (*.txt;*.csv)|*.txt;*.csv|All Files (*.*)|*.*||"), this);
	char *mfBuf = new char[MF_BUFSIZE];
	mfBuf[0] = 0;
	dlg.m_ofn.lpstrFile = mfBuf;
	dlg.m_ofn.nMaxFile = MF_BUFSIZE;
	int res = dlg.DoModal();
	if (res == IDOK)
	{
		CFireFilterDialog ffd(this);

		if(ffd.DoModal() != IDOK)
			return;
		CWaitCursor wait;
		long errors = 0;
		// Get the position of the first filename
		char *logFileName = GetTempFileName();
		FILE * logFile = fopen(logFileName, "wt");
		COleDateTime dateTime = COleDateTime::GetCurrentTime();
		fprintf(logFile, "FireFamily Plus WFMI / Generic Fire Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
		POSITION filenamePosition = dlg.GetStartPosition();
		CString fileName;

		// Open each file selected
		int tErrors;
		while ( filenamePosition != NULL )
		{
			fileName = dlg.GetNextPathName( filenamePosition );
			tErrors = ImportWFMIFire(fileName, logFile, &ffd);

			if(tErrors >= 0)
				errors += tErrors;
			else
			{//user cancelled
				errors = -1;
				break;
			}
		}
		//if(errors >= 0)
		//{
		dateTime = COleDateTime::GetCurrentTime();
		CString strMsg;
		if(errors >= 0)
		{
			fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
			strMsg.Format("Import Complete.\n%ld errors were encountered.", errors);
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
		theApp.SetUserDir(dlg.GetPathName());
	}
	else if(res == IDCANCEL)
	{
		int err = CommDlgExtendedError();
		if(err == FNERR_BUFFERTOOSMALL)
		{
			AfxMessageBox("File Name buffer overrun, select fewer files.");
		}
	}
	delete[] mfBuf;
	chdir(theApp.dbDir);
}

void CImportDialog::OnWFMICodes()
{
	chdir(theApp.userDir);
	CFileDialog	dlg (TRUE, _T("WFMI"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
		OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
		_T("WFMI / DOI Unit Code Import TextFiles (*.txt)|*.txt|All Files (*.*)|*.*||"), this);
	char *mfBuf = new char[MF_BUFSIZE];
	mfBuf[0] = 0;
	dlg.m_ofn.lpstrFile = mfBuf;
	dlg.m_ofn.nMaxFile = MF_BUFSIZE;
	int res = dlg.DoModal();
	if (res == IDOK)
	{
		CWaitCursor wait;
		long errors = 0;
		// Get the position of the first filename
		char *logFileName = GetTempFileName();
		FILE * logFile = fopen(logFileName, "wt");
		COleDateTime dateTime = COleDateTime::GetCurrentTime();
		fprintf(logFile, "FireFamily Plus WFMI / DOI Unit Code Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
		POSITION filenamePosition = dlg.GetStartPosition();
		CString fileName;

		// Open each file selected
		int tErrors;
		while ( filenamePosition != NULL )
		{
			fileName = dlg.GetNextPathName( filenamePosition );
			tErrors = ImportWFMICodes(fileName, logFile);
			//tErrors = ImportNewDOICodes(fileName, logFile);
			if(tErrors >= 0)
				errors += tErrors;
			else
			{//user cancelled
				errors = -1;
				break;
			}
		}
		//if(errors >= 0)
		//{
		dateTime = COleDateTime::GetCurrentTime();
		CString strMsg;
		if(errors >= 0)
		{
			fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
			strMsg.Format("Import Complete.\n%ld errors were encountered.", errors);
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
		theApp.SetUserDir(dlg.GetPathName());
	}
	else if(res == IDCANCEL)
	{
		int err = CommDlgExtendedError();
		if(err == FNERR_BUFFERTOOSMALL)
		{
			AfxMessageBox("File Name buffer overrun, select fewer files.");
		}
	}
	delete[] mfBuf;
	chdir(theApp.dbDir);
}

void CImportDialog::OnFw9()
{
	chdir(theApp.userDir);
	CFileDialog	dlg (TRUE, _T("fw9"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
		OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
		_T("Fire Weather Files (*.fw9;*.fw13)|*.fw9;*.fw13|Text Files (*.txt;*.csv)|*.txt;*.csv|All Files (*.*)|*.*||"), this);
	char *mfBuf = new char[MF_BUFSIZE];
	mfBuf[0] = 0;
	dlg.m_ofn.lpstrFile = mfBuf;
	dlg.m_ofn.nMaxFile = MF_BUFSIZE;
	int res = dlg.DoModal();
	if (res == IDOK)
	{
		CWaitCursor wait;
		long errors = 0;
		// Get the position of the first filename
		char *logFileName = GetTempFileName();
		FILE * logFile = fopen(logFileName, "wt");
		COleDateTime dateTime = COleDateTime::GetCurrentTime();
		fprintf(logFile, "FireFamily Plus FW9/FW13 Weather File Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
		POSITION filenamePosition = dlg.GetStartPosition();
		CString fileName;

		vector<string> importNames;
		// Open each file selected
		int tErrors = 0;
		while ( filenamePosition != NULL )
		{
			fileName = dlg.GetNextPathName( filenamePosition );
			importNames.push_back(fileName.GetBuffer());
			//tErrors = NewImportFW9(fileName, logFile);
			//if(tErrors >= 0)
			//	errors += tErrors;
			//else
			//{//user cancelled
			//	errors = -1;
			//	break;
			//}
		}
		//if ()
			//if(errors >= 0)
		//{
			dateTime = COleDateTime::GetCurrentTime();
			CString strMsg;
			EnableWindow(FALSE);

			if (IDOK == RunFW13Import(importNames, logFile, &tErrors))
			{
				fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
				strMsg.Format("Import Complete.\n%ld errors were encountered.\nRecords imported: %ld\nCheck log file for Hourly Precip calculation notes.", errors, count);
			}
			else
			{
				fprintf(logFile, "\n****** Import Cancelled by user. **********\n");
				strMsg.Format("Import cancelled. Some data may have been imported.\nPlease check the log file for complete information.");
			}
			EnableWindow(TRUE);

			fclose(logFile);
			CImportDoneDialog idd(this);
			idd.statusStr = strMsg;
			idd.logFileName = logFileName;
			delete[] logFileName;
			idd.DoModal();
			theApp.SetUserDir(dlg.GetPathName());
		//}
	}
	else if(res == IDCANCEL)
	{
		int err = CommDlgExtendedError();
		if(err == FNERR_BUFFERTOOSMALL)
		{
			AfxMessageBox("File Name buffer overrun, select fewer files.");
		}
	}
	delete[] mfBuf;
	chdir(theApp.dbDir);
}


int CImportDialog::ImportWFMIFire(CString fileName, FILE *errLog, CFireFilterDialog *ffd){
	int errors = 0;

	CWaitCursor wait;

	fprintf(errLog, "**************************************************\nImport File Name: %s\n", fileName);
	EnableWindow(FALSE);
//	CCancelDialog *cd = NULL;

/*	cd = new CCancelDialog();

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
		 cd->SetTitle("Importing WFMI Fire Data");
	  }
	}*/
	
	
	if(pDoc->HasTempFire())
	{
		try
		{
			pDoc->m_pDB->ExecuteSQL("DROP TABLE [ffpTempFire]");
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	pDoc->CreateTempFire();
	CFiresSet tFires(firesSet.m_pDatabase);
	tFires.Open(CRecordset::dynaset, "ffpTempFire");

	char delim = '|';
	char line[MAX_INPUT_LINE];
	char temp[MAX_INPUT_LINE];
	char sFireID[MAX_FIELD_LEN],
		sAgency[MAX_FIELD_LEN],
		sUnit[MAX_FIELD_LEN],
		sUnitID[MAX_FIELD_LEN],
		sFireName[MAX_FIELD_LEN],
		sFireNumber[MAX_FIELD_LEN],
		sFireCode[MAX_FIELD_LEN],
		sFireType[MAX_FIELD_LEN],
		sProtectionType[MAX_FIELD_LEN],
		sStartTime[MAX_FIELD_LEN],
		sInitAttTime[MAX_FIELD_LEN],
		sControlTime[MAX_FIELD_LEN],
		sControlAcres[MAX_FIELD_LEN],
		sGeneralCause[MAX_FIELD_LEN],
		sDatum[MAX_FIELD_LEN],
		sLatitude[MAX_FIELD_LEN],
		sLongitude[MAX_FIELD_LEN],
		sSlope[MAX_FIELD_LEN],
		sElevation[MAX_FIELD_LEN],
		sAspect[MAX_FIELD_LEN],
		sFuelModel[MAX_FIELD_LEN],
		sComplete[MAX_INPUT_LINE],
		sOwnership[MAX_INPUT_LINE];
	CString strLine;
	ifstream stream;

	utils util(firesSet.m_pDatabase);

	stream.open(fileName);
	//check for proper header info
	bool goodFile = false;
	while (stream.good())// && (!cd || ! cd->Abort()))
	{
		stream.getline(line,MAX_INPUT_LINE,'\n');
		strLine = line;
		strLine.Trim();
		if(strLine.GetLength() <= 0)
		{
			continue;
		}
		stringstream stream2;
		stream2 << line;
		stream2.getline(sFireID,MAX_FIELD_LEN,delim);
		if(strstr(sFireID, "Export Name") > 0)
		{
			stream2.getline(sFireID,MAX_FIELD_LEN,delim);
			if(strncmp(sFireID, "PCHA", strlen("PCHA")) == 0)
			{
				goodFile = true;
				break;
			}
		}
	}
	if(!goodFile)
	{
		errors = 1;
		fprintf(errLog, "No Export Name|PCHA header encountered.\n");
		stream.close();
		tFires.Close();
		return 1;
	}
	count = 0;
	long updates = 0, rejects = 0,skipped = 0;
	int lastRegionID = 0;

	CString temp2, temp3;
	//stringstream stream2;

	CFiresSet cFires(firesSet.m_pDatabase);
	cFires.Open();

	while (stream.good())// && (!cd || ! cd->Abort()))
	{
		stream.getline(line,MAX_INPUT_LINE,'\n');
		int i;
		for (i=0;i<6;i++)
			temp[i] = line[i];
		temp[i] = '\0';
		if (atoi(temp) < 1){ // not an input line
			continue;
		}
		//delim = '|';//line[i];

		stringstream stream2;
		stream2 << line;
		stream2.getline(sFireID,MAX_FIELD_LEN,delim);
		stream2.getline(sAgency,MAX_FIELD_LEN,delim);
		stream2.getline(sUnit,MAX_FIELD_LEN,delim);
		stream2.getline(sUnitID,MAX_FIELD_LEN,delim);
		stream2.getline(sFireName,MAX_FIELD_LEN,delim);
		stream2.getline(sFireNumber,MAX_FIELD_LEN,delim);
		stream2.getline(sFireCode,MAX_FIELD_LEN,delim);
		stream2.getline(sFireType,MAX_FIELD_LEN,delim);
		stream2.getline(sProtectionType,MAX_FIELD_LEN,delim);
		stream2.getline(sStartTime,MAX_FIELD_LEN,delim);
		stream2.getline(sInitAttTime,MAX_FIELD_LEN,delim);
		stream2.getline(sControlTime,MAX_FIELD_LEN,delim);
		stream2.getline(sControlAcres,MAX_FIELD_LEN,delim);
		stream2.getline(sGeneralCause,MAX_FIELD_LEN,delim);
		stream2.getline(sDatum,MAX_FIELD_LEN,delim);
		stream2.getline(sLatitude,MAX_FIELD_LEN,delim);
		stream2.getline(sLongitude,MAX_FIELD_LEN,delim);
		stream2.getline(sSlope,MAX_FIELD_LEN,delim);
		stream2.getline(sElevation,MAX_FIELD_LEN,delim);
		stream2.getline(sAspect,MAX_FIELD_LEN,delim);
		stream2.getline(sFuelModel,MAX_FIELD_LEN,delim);
		stream2.getline(sComplete,MAX_INPUT_LINE,delim);
		//stream2.getline(sComplete,MAX_INPUT_LINE,delim);

		COleDateTime dt;

		count ++;
		//if (cd)
		//	cd->Increment();

		char bufFilter[3];
		sprintf(bufFilter,"  ");

		int goodRec = false;
		if (strlen(sFireType) > 0 && strlen(sProtectionType) > 0){
			if (atoi(sFireType) > 9 || atoi(sProtectionType) > 9){
				fprintf(errLog, "\tRecord Number %ld, Invalid FireType  or ProtectionType (%s %s)\n", count,sFireType,sProtectionType );
				rejects ++;
				continue;
			}

			sprintf(bufFilter,"%2d",atoi(sFireType) * 10 + atoi(sProtectionType));

			switch(atoi(bufFilter))
			{
			case 11:
				if(ffd->type11)
					goodRec = true;
				break;
			case 12:
				if(ffd->type12)
					goodRec = true;
				break;
			case 13:
				if(ffd->type13)
					goodRec = true;
				break;
			case 14:
				if(ffd->type14)
					goodRec = true;
				break;
			case 15:
				if(ffd->type15)
					goodRec = true;
				break;
			case 16:
				if(ffd->type16)
					goodRec = true;
				break;
			case 19:
				if(ffd->type19)
					goodRec = true;
				break;
			case 21:
				if(ffd->type21)
					goodRec = true;
				break;
			case 22:
				if(ffd->type22)
					goodRec = true;
				break;
			case 23:
				if(ffd->type23)
					goodRec = true;
				break;
			case 25:
				if(ffd->type25)
					goodRec = true;
				break;
			case 26:
				if(ffd->type26)
					goodRec = true;
				break;
			case 37:
				if(ffd->type37)
					goodRec = true;
				break;
			case 48:
				if(ffd->type48)
					goodRec = true;
				break;
			case 49:
				if(ffd->type49)
					goodRec = true;
				break;
			}
		}
		if(!goodRec){
			//	 fprintf(errLog, "\t(Skipped Record Number %ld, due to FireType /ProtectionType (%s) filter)\n", count,bufFilter );
			skipped ++;
			continue;
		}

		if (strlen(sStartTime) < 2){
			fprintf(errLog, "\tRecord Number %ld, no StartTime\n", count );
			rejects ++;
			continue;
		}

		int y,m,d,hr,min;

		strncpy(temp, &sStartTime[6], 4);
		temp[4] = 0;
		y = atoi(temp);

		strncpy(temp, &sStartTime[0], 2);
		temp[2] = 0;
		m = atoi(temp);

		strncpy(temp, &sStartTime[3], 2);
		temp[2] = 0;
		d = atoi(temp);

		strncpy(temp, &sStartTime[11], 2);
		temp[2] = 0;
		hr = atoi(temp);

		strncpy(temp, &sStartTime[14], 2);
		temp[2] = 0;
		min = atoi(temp);

		dt.SetDateTime(y, m, d, hr, min, 0);
		if(dt.GetStatus() != 0 || y < 1900)
		{
			fprintf(errLog, "\tRecord Number %ld, %s: %d/%d/%04d\n", count , errStrings[1], m, d, y);
			rejects ++;

			continue;
		}
		COleDateTime today;
		today = COleDateTime::GetCurrentTime();
		if(dt > today)
		{
			fprintf(errLog, "\tRecord Number %ld, %s: %d/%d/%04d\n", count, errStrings[1], m, d, y);
			rejects ++;

			continue;
		}

		// check fireNumber and FireID fields
		if (strlen(sFireNumber) < 1){
			if (strlen(sFireID) > 0)
				sprintf(sFireNumber,"%s",sFireID);
			else {
				fprintf(errLog, "\tRecord Number %d, no FireID or FireNumber: %d/%d/%04d\n", count , m, d, y);
				rejects ++;

				continue;
			}
		}
		// new 2014: WFMI unit ID may have "US" prepended to it.... if so, strip it off
		if (strlen(sUnitID) > 1)
		   if (sUnitID[0]=='U' && sUnitID[1]=='S')
		   {
			   char tempUnit[MAX_FIELD_LEN];
			   sprintf(tempUnit,"%s",sUnitID);
			   int i=0;
			   for (i=0;i<strlen(tempUnit)-2;i++)
				   sUnitID[i] = tempUnit[i+2];
			   sUnitID[i]='\0';

		   }

		CDOIUnitSet doiSet(firesSet.m_pDatabase);
		if (sUnitID && strlen(sUnitID) > 0){
			doiSet.m_strFilter.Format("[WFMICode] = '%s'",sUnitID);
			doiSet.Open();
			if (doiSet.IsEOF()){
				// look up agency first
				CFireAgencySet agencySet(firesSet.m_pDatabase);
				agencySet.m_strFilter.Format("[Name]='%s'",sAgency);
				agencySet.Open();
				if (agencySet.IsEOF()){
					rejects ++;
					fprintf(errLog, "\tRecord Number %ld, UnitCode: %s has no match in refDOIUnits, COULDN'T ADD (no agency):  %d/%d/%04d\n", count, sUnitID, m, d, y);
					doiSet.Close();
					agencySet.Close();
					continue;
				} else {
					fprintf(errLog, "\tRecord Number %ld, UnitCode: %s has no match in refDOIUnits, ADDED:  %d/%d/%04d\n", count, sUnitID, m, d, y);
					// add as a new unit, with name = WFMIcode

					CFireUnitSet unitSet(firesSet.m_pDatabase);
					unitSet.m_strSort.Format("[UnitID]");
					unitSet.Open();
					unitSet.MoveLast();
					int nextID = unitSet.m_UnitID + 1;
					unitSet.AddNew();
					unitSet.m_Name.Format("%s",sUnitID);
					unitSet.m_UnitID = nextID;
					unitSet.m_RegionID = lastRegionID;
					unitSet.Update();

					doiSet.AddNew();
					doiSet.m_AgencyID = agencySet.m_AgencyID;
					doiSet.m_WFMIcode.Format("%s",sUnitID);
					doiSet.m_WFMIcode.Trim();
					doiSet.m_WFMIcode = doiSet.m_WFMIcode.Mid(0, 6);
					doiSet.m_RegionID = lastRegionID;
					doiSet.m_UnitID = nextID;
					doiSet.m_SubunitID = -999;   // temporary flag, will be changed to 0 at end of import process below
					doiSet.Update();
					doiSet.Requery();
					unitSet.Close();
				}
			}
		} else {
			fprintf(errLog, "\tRecord Number %ld, No WFMI UnitCode: %d/%d/%04d\n", count,  m, d, y);
			rejects++;
			continue;
		}
		//2022/09
		// New fire number for Fires table
		CString newFireNumber;
		newFireNumber.Format("%d%02d%02d-%d-%d-%d-0-%s", dt.GetYear(), dt.GetMonth(), dt.GetDay(), doiSet.m_AgencyID, doiSet.m_RegionID, doiSet.m_UnitID, sFireNumber);

		// check for dupes
		cFires.m_strFilter.Format("[FireNumber]='%s' AND [AgencyID]=%d AND [RegionID]=%d AND [UnitID]=%d",
			newFireNumber.GetBuffer(), doiSet.m_AgencyID, doiSet.m_RegionID, doiSet.m_UnitID);
		cFires.Requery();
		if (! cFires.IsEOF()){ // dupe
			doiSet.Close();
			rejects ++;
			fprintf(errLog, "\tRecord Number %ld, FireNumber %s  Duplicate record:  %d/%d/%04d\n", count, newFireNumber.GetBuffer(), m, d, y);
			continue;
		}
		//make sure not a duplicate in the current import file!!!!!!!!
		tFires.m_strFilter.Format("[FireNumber]='%s' AND [AgencyID]=%d AND [RegionID]=%d AND [UnitID]=%d",
			newFireNumber.GetBuffer(), doiSet.m_AgencyID, doiSet.m_RegionID, doiSet.m_UnitID);
		tFires.Requery();
		if(!tFires.IsEOF())
		{
			doiSet.Close();
			rejects ++;
			fprintf(errLog, "\tRecord Number %ld, FireNumber %s  Duplicate record:  %d/%d/%04d\n", count, newFireNumber.GetBuffer(), m, d, y);
			continue;
		}
		tFires.m_strFilter = "";
		tFires.Requery();
		tFires.AddNew();

		// now get region/agency/unit/subunit from refDOIUnits table
		if (doiSet.IsOpen()){
			if (! doiSet.IsEOF()){
				tFires.m_AgencyID = doiSet.m_AgencyID;
				tFires.m_RegionID = doiSet.m_RegionID;
				if (doiSet.m_RegionID)
					lastRegionID = doiSet.m_RegionID;

				tFires.m_UnitID = doiSet.m_UnitID;
				tFires.m_SubunitID = doiSet.m_SubunitID;
			}
			doiSet.Close();
		}
		tFires.m_Discovery = dt;
		//tFires.m_FireID = atoi(sFireID);
		//tFires.m_Year.Format("%d",y);
		//	tFires.m_AgencyID = FIXME;
		//	tFires.m_RegionID = FIXME;
		//	tFires.m_UnitID = FIXME;
		//	tFires.m_SubunitID = FIXME;

		//CString tempNumber;
		//tempNumber.Format("%s",sFireNumber);

		tFires.m_FireNumber = newFireNumber.GetBuffer();
		//	tFires.m_State = FIXME;
		//	tFires.m_County = FIXME:
		tFires.m_TotalAcres = atof(sControlAcres);
		//  tFires.SizeClass = FIXME;
		sFireName[25] = 0;
		tFires.m_FireName = sFireName;
		tFires.m_FireName.Trim();
		int causeVal = atoi(sGeneralCause);
		bool validCause = util.validateStatCause(causeVal);
		if (validCause)
			  tFires.m_StatisticalCause = atoi(sGeneralCause);
		else
			{
				fprintf(errLog, "\tRecord Number %ld, invalid statistical cause: %s\n", count, sGeneralCause);
				rejects ++;
				continue;
			 }
		

		if(strlen(sLatitude) > 0)
		{
			tFires.m_latitude = atof(sLatitude);
			/*int dd, mm, ss;
			DecimalDegree2DDMMSS(atof(sLatitude), &dd, &mm, &ss);
			tFires.m_LatDD = dd;
			tFires.m_LatMM = mm;
			tFires.m_LatSS = ss;*/
		}
		else
		{
			tFires.SetFieldNull(&tFires.m_latitude);
			//tFires.SetFieldNull(&tFires.m_LatMM);
			//tFires.SetFieldNull(&tFires.m_LatSS);
		}
		if(strlen(sLongitude) > 0)
		{
			tFires.m_longitude = atof(sLongitude);
			/*int dd, mm, ss;
			DecimalDegree2DDMMSS(atof(sLongitude), &dd, &mm, &ss);
			tFires.m_LonDD = dd;
			tFires.m_LonMM = mm;
			tFires.m_LonSS = ss;*/
		}
		else
		{
			tFires.SetFieldNull(&tFires.m_longitude);
			//tFires.SetFieldNull(&tFires.m_LonMM);
			//tFires.SetFieldNull(&tFires.m_LonSS);
		}
		/*     tFires.m_LatDD = atoi(sLatitude);
		tFires.m_LatMM = (int) ((atof(sLatitude) + .000001 - (float) (tFires.m_LatDD)) * 60.0);
		tFires.m_LatSS = (int) ((atof(sLatitude) + .000001 - ((float) (tFires.m_LatDD) + 0.0000001) - (float) (tFires.m_LatMM) / 60.0) * 60.0);

		if (tFires.m_LatMM < 0)
		tFires.m_LatMM = 0 - tFires.m_LatMM;
		if (tFires.m_LatSS < 0)
		tFires.m_LatSS = 0 - tFires.m_LatSS;

		tFires.m_LonDD = atoi(sLongitude);
		tFires.m_LonMM = (int) ((atof(sLongitude) + .000001 - (float) (tFires.m_LonDD)) * 60.0);
		tFires.m_LonSS = (int) ((atof(sLongitude) + .000001 - ((float) (tFires.m_LonDD) + 0.000001) - (float) (tFires.m_LonMM) / 60.0) * 60.0);

		if (tFires.m_LonMM < 0)
		tFires.m_LonMM = 0 - tFires.m_LonMM;
		if (tFires.m_LonSS < 0)
		tFires.m_LonSS = 0 - tFires.m_LonSS;
		*/

		/*if (strlen(sInitAttTime) < 2) {
			// fprintf(errLog, "\tWARNING: Record Number %d, Init Attack Time is NULL\n", count );
			// rejects ++;
			// continue;
		} else {
			strncpy(temp, &sInitAttTime[6], 4);
			temp[4] = 0;
			y = atoi(temp);

			strncpy(temp, &sInitAttTime[0], 2);
			temp[2] = 0;
			m = atoi(temp);

			strncpy(temp, &sInitAttTime[3], 2);
			temp[2] = 0;
			d = atoi(temp);

			strncpy(temp, &sInitAttTime[11], 2);
			temp[2] = 0;
			hr = atoi(temp);

			strncpy(temp, &sInitAttTime[14], 2);
			temp[2] = 0;
			min = atoi(temp);

			dt.SetDateTime(y, m, d, hr, min, 0);
			tFires.m_FirstAttack = dt;
		}*/

		if (strlen(sControlTime) < 2){
			//  fprintf(errLog, "\tWARNING: Record Number %d, Control Time is NULL\n", count );
			// rejects ++;
			// continue;
		} else {
			strncpy(temp, &sControlTime[6], 4);
			temp[4] = 0;
			y = atoi(temp);

			strncpy(temp, &sControlTime[0], 2);
			temp[2] = 0;
			m = atoi(temp);

			strncpy(temp, &sControlTime[3], 2);
			temp[2] = 0;
			d = atoi(temp);

			strncpy(temp, &sControlTime[11], 2);
			temp[2] = 0;
			hr = atoi(temp);

			strncpy(temp, &sControlTime[14], 2);
			temp[2] = 0;
			min = atoi(temp);

			dt.SetDateTime(y, m, d, hr, min, 0);
			tFires.m_StrategyMet = dt;
		}
		/*if (strlen(sSlope) == 1)
		      tFires.m_Slope = sSlope;
		if (strlen(sElevation)==1)
		   tFires.m_Elevation = sElevation;
		if (strlen(sAspect)==1)
		   tFires.m_Aspect = sAspect;
		if(strlen(sFuelModel) == 1)
			tFires.m_FuelModel = sFuelModel;
		tFires.m_FireType = sFireType;*/

		tFires.Update();

		//fprintf(errLog,"\tAppended record %d\n",count);
	}

	cFires.Close();

	stream.close();

	updates = WriteTempFires(&tFires, 0);
	tFires.Close();

	//if(!cd || !cd->Abort())
	//{
		fprintf(errLog, "Processed %d fire records\n"
			"\tSkipped %d fire records due to Fire Type filter.\n"
			"\tAppended %d fire records.\n"
			"\tRejected %d fire records.\n",
			count, skipped,count - skipped - rejects, rejects);
		errors = rejects ;
		// see if we added any units
		CDOIUnitSet doiSet(firesSet.m_pDatabase);
		doiSet.m_strFilter.Format("[SubunitID] = -999");
		doiSet.Open();
		if (! doiSet.IsEOF()){
			CString msgStr;
			msgStr.Format("The WFMI import file included new units, which have been added to this database.\n\nThe following new units have been added:\n\n");

			while (! doiSet.IsEOF()){
				CString unitStr,
					unitStr2,
					unitStr3;

				CFireAgencySet agencySet(firesSet.m_pDatabase);

				agencySet.m_strFilter.Format("[AgencyID] = %d", doiSet.m_AgencyID);
				agencySet.Open();
				if (! agencySet.IsEOF())
					unitStr.Format("Agency: %s", agencySet.m_Name);
				else
					unitStr.Format("Agency: N/A");
				agencySet.Close();

				CFireRegionSet regSet(firesSet.m_pDatabase);

				regSet.m_strFilter.Format("[RegionID] = %d", doiSet.m_RegionID);
				regSet.Open();
				if (! regSet.IsEOF())
					unitStr2.Format("\tRegion: %s", regSet.m_Name);
				else
					unitStr2.Format("\tRegion: N/A");
				regSet.Close();

				unitStr3.Format("\tUnit: %s\n", doiSet.m_WFMIcode);

				msgStr.Append(unitStr);
				msgStr.Append(unitStr2);
				msgStr.Append(unitStr3);

				doiSet.Edit();
				doiSet.m_SubunitID = 0;
				doiSet.Update();
				doiSet.MoveNext();
			}
			AfxMessageBox(msgStr);
		}
		doiSet.Close();
	/*}
	else
	{
		errors = -1;
		fprintf(errLog, "Import Cancelled by User.");
	}
	if (cd)
		cd->DestroyWindow();
*/
	EnableWindow(TRUE);

	return errors;
}

int CImportDialog::ImportWFMICodes(CString fileName, FILE *errLog){
	int errors = 0;

	CWaitCursor wait;

	fprintf(errLog, "**************************************************\nImport File Name: %s\n", fileName);
	EnableWindow(FALSE);
//	CCancelDialog *cd = NULL;

/*	cd = new CCancelDialog();

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
	     cd->SetTitle("Importing WFMI (DOI) Unit Codes");
	  }
	}*/


	// file format from http://www.nifc.blm.gov/nsdu/unit_id/publish/UnitIdExport.txt :
	//UnitId|GeographicArea|Gacc|WildlandRole|UnitType|Department|Agency|Parent|Country|State|Code|Name|OrgCode|Address|Phone|Email|Started|Modified|Stopped|RemovalReason
	// (We just need UnitID, Agency, and Name)
	//
	// Try to match Name field to existing FFP agency/region/unit/subunit record.

	ifstream stream;

	stream.open(fileName);

	count = 0;
	long imported = 0, updates = 0, rejects = 0;

	char line[MAX_INPUT_LINE];
	char temp[MAX_INPUT_LINE];
	CString temp2, temp3;
	stringstream stream2;

	utils util(firesSet.m_pDatabase);

	char sWFMIcode[MAX_FIELD_LEN],
		sAgency[MAX_FIELD_LEN],
		sDept[MAX_FIELD_LEN],
		sName[MAX_FIELD_LEN];

	CString workingName,tempName;

	char delim = '|';

	int agencyID = 0,
		regionID = 0,
		unitID = 0,
		subunitID = 0;

	bool found = false;

	CFireUnitSet unitSet(firesSet.m_pDatabase);
	unitSet.m_strSort="[Name]";
	unitSet.Open();

	CFireSubunitSet subunitSet(firesSet.m_pDatabase);
	subunitSet.m_strSort="[Name]";
	subunitSet.Open();

	CFireRegionSet regionSet(firesSet.m_pDatabase);
	regionSet.m_strSort="[Name]";
	regionSet.Open();

	CFireAgencySet agencySet(firesSet.m_pDatabase);
	agencySet.m_strSort="[Name]";
	agencySet.Open();

	CDOIUnitSet doiSet(firesSet.m_pDatabase);
	doiSet.Open();
	while (! doiSet.IsEOF()){
		doiSet.Delete();
		doiSet.MoveNext();
	}
	doiSet.Close();
	doiSet.Open();

	// remove double spaces first

	while (! regionSet.IsEOF()){
		CString tempstr1;

		tempstr1.Format(regionSet.m_Name);
		tempstr1.Replace("  "," ");
		regionSet.Edit();
		regionSet.m_Name = tempstr1;
		regionSet.Update();
		regionSet.MoveNext();
	}
	regionSet.Close();
	regionSet.Open();

	while (stream.good())// && (!cd || ! cd->Abort()))
	{
		found = false;

		subunitID = 0;
		unitID = 0;
		regionID = 0;
		agencyID = 0;

		stream.getline(line,MAX_INPUT_LINE,'\n');

		stringstream stream2;

		stream2 << line;
		/* all wfmi codes
		stream2.getline(sWFMIcode,MAX_FIELD_LEN,delim);
		stream2.getline(temp,MAX_FIELD_LEN,delim); // geo area
		stream2.getline(temp,MAX_FIELD_LEN,delim); // gacc
		stream2.getline(temp,MAX_FIELD_LEN,delim); // wildlandrole
		stream2.getline(sDept,MAX_FIELD_LEN,delim); // unittype
		stream2.getline(temp,MAX_FIELD_LEN,delim); // department
		stream2.getline(sAgency,MAX_FIELD_LEN,delim);
		stream2.getline(temp,MAX_FIELD_LEN,delim); // parent
		stream2.getline(temp,MAX_FIELD_LEN,delim); // country
		stream2.getline(temp,MAX_FIELD_LEN,delim); // state
		stream2.getline(sCode,MAX_FIELD_LEN,delim); // code
		stream2.getline(sName,MAX_FIELD_LEN,delim);
		*/
		// just fire codes
		stream2.getline(sWFMIcode,MAX_FIELD_LEN,delim);
		stream2.getline(sAgency,MAX_FIELD_LEN,delim);
		stream2.getline(sDept,MAX_FIELD_LEN,delim); // unittype
		stream2.getline(sName,MAX_FIELD_LEN,delim);
		stream2.getline(temp,MAX_FIELD_LEN); // unittype

		/*
		tempName.Format("%s",sDept);
		if (tempName.Compare("Federal")){
		count ++;
		fprintf(errLog,"Rejected (non-federal): %s  (%s)\n", sWFMIcode,sName);
		rejects++;
		cd.Increment();
		continue;
		}
		*/
		CString sCode;
		sCode.Format("%s",sWFMIcode);
		sCode.Trim();

		// just skip agencies that we're not interested in
		tempName.Format("%s",sAgency);
		tempName.Trim();

		bool wantAgency = false;
		if (tempName.GetLength() > 1){
			if (!tempName.Compare("FS"))
				wantAgency = true;
			if (!tempName.Compare("BLM"))
				wantAgency = true;
			if (!tempName.Compare("BIA"))
				wantAgency = true;
			if (!tempName.Compare("FWS"))
				wantAgency = true;
			if (!tempName.Compare("NPS"))
				wantAgency = true;
		}

		if (! wantAgency){
			count ++;
			//if (cd)
			 //  cd->Increment();
			continue;
		}

		// massage name field

		workingName.Format("%s",sName);
		workingName.Trim();

		CString tempName2;
		CString workingName2 = "2359203580923509852";
		CString workingName3 ="234582520982350";
		CString workingName4 = "1313151325151251";

		workingName4.Format("%s",sName);

		// sql escape for MS Access
		workingName4.Replace("'","''");
		workingName4.Trim();

		int strpos = 0;

		// get rid of single quotes

		strpos = workingName.Find("'");
		if (strpos >= 0){
			tempName.Format("%s",workingName.Left(strpos));
			tempName2.Format("%s",workingName.Mid(strpos + 1,workingName.GetLength() - strpos - 1));
			workingName.Format("%s%s",tempName,tempName2);
		}

		//- convert "Ranger District" to "RD"
		strpos = workingName.Find("Ranger District");
		if (strpos >= 0){
			tempName.Format("%s",workingName.Left(strpos));
			workingName.Format("%sRD",tempName);
		}

		/*  //- convert "Fort" to "FT or FT."
		strpos = workingName.Find("Fort");
		if (strpos >= 0){
		tempName.Format("%s",workingName.Left(strpos));
		tempName2.Format("%s", workingName.Mid(strpos+5,workingName.GetLength() - strpos - 5));
		if (strpos > 0){
		workingName.Format("%s FT %s",tempName,tempName2);
		workingName2.Format("%s FT.%s",tempName,tempName2);
		} else {
		workingName.Format("FT %s",tempName,tempName2);
		workingName2.Format("%s FT.%s",tempName,tempName2);
		}
		}
		*/

		//- convert "MTN " to "MTN."
		strpos = workingName.Find("MTN ");
		if (strpos >= 0){
			tempName.Format("%s",workingName.Left(strpos));
			tempName2.Format("%s", workingName.Mid(strpos+4,workingName.GetLength() - strpos - 4));

			workingName2.Format("%s MTN. %s",tempName,tempName2);
		}

		//- convert "National Forest" to "NF or N.F."
		strpos = workingName.Find("National Forest");
		if (strpos >= 0){
			tempName.Format("%s",workingName.Left(strpos));
			workingName.Format("%sNF",tempName);
			workingName2.Format("%sN.F.",tempName);
		}

		//- convert "National Wildlife Refuge" to "NWR"
		// also try without the "NWR".

		strpos = workingName.Find("National Wildlife Refuge");
		if (strpos >= 0){
			tempName.Format("%s",workingName.Left(strpos));
			workingName.Format("%sNWR",tempName);
			workingName2.Format("%s",tempName);
		}

		tempName.Format("%s",workingName.GetString());
		workingName.Format("%s",tempName.Left(15));

		// now check subunits table
		if (workingName2.GetLength() > 1)
			subunitSet.m_strFilter.Format("[Name] LIKE '%%%s%%' OR [Name] LIKE '%%%s%%' OR [Name] LIKE '%s%%' OR [Name] LIKE '%%%s%%' OR [Name] LIKE '%%%s%%'",workingName2.GetString(),workingName.GetString(),workingName.GetString(),workingName.GetString(),workingName4.GetString(),sCode);
		else
			subunitSet.m_strFilter.Format("[Name] LIKE '%s' OR [Name] LIKE '%%%s%%' OR [Name] LIKE '%s%%' OR [Name] LIKE '%%%s%%' OR [Name] LIKE '%%%s%%'",workingName.GetString(),workingName.GetString(),workingName.GetString(),workingName4.GetString(),sCode);
		subunitSet.Requery();
		if (!subunitSet.IsEOF()){
			// found matching subunit
			found = true;
			unitID = subunitSet.m_UnitID;
			subunitID = subunitSet.m_SubunitID;

			// need to look up region as well
			unitSet.m_strFilter.Format("[UnitID]=%d",unitID);
			unitSet.Requery();
			if (! unitSet.IsEOF())
				regionID = unitSet.m_RegionID;

			// need to look up agency as well
			if (regionID > 0){
				regionSet.m_strFilter.Format("[RegionID]=%d",regionID);
				regionSet.Requery();
				if (! regionSet.IsEOF()){
					agencyID = regionSet.m_AgencyID;
					imported ++;
				}
			}
		}

		if (! found){
			// now check units table
			if (workingName2.GetLength() > 1)
				unitSet.m_strFilter.Format("[Name] LIKE '%%%s%%' OR [Name] LIKE '%%%s%%' OR [Name] LIKE '%s%%' OR [Name] LIKE '%%%s%%'  OR [Name] LIKE '%%%s%%'",workingName2.GetString(),workingName.GetString(),workingName.GetString(),workingName.GetString(),workingName4.GetString(),sCode);
			else
				unitSet.m_strFilter.Format("[Name] LIKE '%s' OR [Name] LIKE '%%%s%%' OR [Name] LIKE '%s%%' OR [Name] LIKE '%%%s%%'  OR [Name] LIKE '%%%s%%'",workingName.GetString(),workingName.GetString(),workingName.GetString(),workingName4.GetString(),sCode);
			unitSet.Requery();
			if (!unitSet.IsEOF()){
				// found matching unit
				found = true;

				unitID = unitSet.m_UnitID;
				regionID = unitSet.m_RegionID;
				// need to look up agency as well
				// need to look up agency as well
				if (regionID > 0){
					regionSet.m_strFilter.Format("[RegionID]=%d",regionID);
					regionSet.Requery();
					if (! regionSet.IsEOF()){
						agencyID = regionSet.m_AgencyID;
						imported ++;
				 }
				}
			}
		}

		/*
		if (! found){
		// now check region table
		regionSet.m_strFilter.Format("[Name] LIKE '%s' OR [Name] LIKE '%%%s%%' OR [Name] LIKE '%s%%' OR [Name] LIKE '%%%s%%' OR [Name] LIKE '%%%s%%'",workingName.GetString(),workingName.GetString(),workingName.GetString(),workingName4.GetString(),sCode);
		regionSet.Requery();
		if (!regionSet.IsEOF()){
		// found matching region
		found = true;

		agencyID = regionSet.m_AgencyID;
		regionID = regionSet.m_RegionID;
		}
		}
		*/

		// add new record
		if (found){
			//fprintf(errLog," - MATCH\n");
			doiSet.AddNew();
			doiSet.m_WFMIcode = sCode.GetString();
			doiSet.m_SubunitID = subunitID;
			doiSet.m_UnitID = unitID;
			doiSet.m_RegionID = regionID;
			doiSet.m_AgencyID = agencyID;
			doiSet.Update();
		} else {
			if (workingName2.GetLength() > 1)
				fprintf(errLog,"Checking: %s  (%s)(%s)", sCode.GetString(),workingName,workingName2);
			else
				fprintf(errLog,"Checking: %s  (%s)", sCode.GetString(),workingName);

			fprintf(errLog," - not found\n");
			rejects ++;
		}
		count ++;
		//if (cd)
		//	cd->Increment();
	}

	unitSet.Close();
	subunitSet.Close();
	regionSet.Close();
	agencySet.Close();
	doiSet.Close();
	stream.close();

	/*if(!cd || !cd->Abort())
	{
		fprintf(errLog, "Processed %ld  records\n"
			"\tAdded %ld  records to database.\n"
			"\tRejected %ld records for bad data.\n",
			count, count-rejects, rejects);
		errors = rejects;
	}
	else
	{
		errors = -1;
		fprintf(errLog, "Import Cancelled by User.");
	}
	if (cd)
		cd->DestroyWindow();
	*/
	EnableWindow(TRUE);

	return errors;
}

// temporary function used to import new FFP unit (and some region) codes for DOI
// source file: new_doi_units.txt (from Larry 9/2006)
//
// file format:
//
// code|agency|unit|definition|ffp_region|ffp_unit
//
// where 'definition' is the new name, and 'ffp_unit' is the old name.

int CImportDialog::ImportNewDOICodes(CString fileName, FILE *errLog){
	int errors = 0;

	CWaitCursor wait;

	fprintf(errLog, "**************************************************\nImport File Name: %s\n", fileName);
	EnableWindow(FALSE);

//	CCancelDialog *cd = NULL;

/*	cd = new CCancelDialog();

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
		 cd->SetTitle("Importing new Unit Codes");
	  }
	}
*/


	

	ifstream stream;

	stream.open(fileName);

	count = 0;
	long new_units = 0, new_regions = 0, updated_units=0, updated_regions = 0, rejects = 0;

	char line[MAX_INPUT_LINE];
	char temp[MAX_INPUT_LINE];
	CString temp2, temp3;
	stringstream stream2;

	char delim = '\t';

	while (stream.good())// && (!cd || ! cd->Abort()))
	{
		char sCode[MAX_FIELD_LEN],
			sAgency[MAX_FIELD_LEN],
			sUnit[MAX_FIELD_LEN],
			sDefinition[MAX_FIELD_LEN],
			sFFPRegion[MAX_FIELD_LEN],
			sFFPUnit[MAX_FIELD_LEN];

		stream.getline(line,MAX_INPUT_LINE,'\n');

		stringstream stream2;

		stream2 << line;

		stream2.getline(sCode,MAX_FIELD_LEN,delim);
		stream2.getline(sAgency,MAX_FIELD_LEN,delim);
		stream2.getline(sUnit,MAX_FIELD_LEN,delim);
		stream2.getline(sDefinition,MAX_FIELD_LEN,delim);
		stream2.getline(sFFPRegion,MAX_FIELD_LEN,delim);
		stream2.getline(sFFPUnit,MAX_FIELD_LEN);

		// max field length is 40
		if (strlen(sDefinition) > 40)
			sDefinition[40] = '\0';

		// region?
		if (strlen(sFFPRegion) > 0){
			CFireRegionSet regionSet(firesSet.m_pDatabase);
			CString tempstr;
			tempstr.Format("%s",sFFPRegion);
			// sql escape for MS Access
			tempstr.Replace("'","''");

			regionSet.m_strFilter.Format("[Name]='%s'",tempstr);
			regionSet.Open();
			if (regionSet.IsEOF()){
				fprintf(errLog,"ERROR: ffp region '%s' not found.\n", sFFPRegion);
				rejects ++;
				regionSet.Close();
				continue;
			}
			fprintf(errLog,"UPDATE REGION: '%s' -> '%s'\n", sFFPRegion,sDefinition);
			regionSet.Edit();
			regionSet.m_Name.Format("%s",sDefinition);
			updated_regions++;
			regionSet.Update();
			regionSet.Close();
		} else
			if (strlen(sFFPUnit) > 0){
				if (strstr(sFFPUnit,"Unknown")){  // add new?
					fprintf(errLog,"ERROR: unknown unit. '%s' not implemented yet.\n", sDefinition);
					rejects ++;

					continue;
				} else {
					CFireUnitSet unitSet(firesSet.m_pDatabase);
					CString tempstr;
					tempstr.Format("%s",sFFPUnit);
					// sql escape for MS Access
					tempstr.Replace("'","''");
					unitSet.m_strFilter.Format("[Name]='%s'",tempstr);
					unitSet.Open();
					if (unitSet.IsEOF()){
						fprintf(errLog,"ERROR: ffp unit '%s' not found.\n", sFFPUnit);
						rejects ++;
						unitSet.Close();
						continue;
					}
					fprintf(errLog,"UPDATE UNIT: '%s' -> '%s'\n", sFFPUnit,sDefinition);
					unitSet.Edit();
					unitSet.m_Name.Format("%s",sDefinition);
					unitSet.Update();
					unitSet.Close();
					updated_units++;
				}
			}

			count ++;
			//if (cd)
			//	cd->Increment();
	}

	// now remove "A00"-type prefixes from units
	CFireUnitSet unitSet(firesSet.m_pDatabase);
	unitSet.Open();
	while (! unitSet.IsEOF()){
		CString tempstr;
		tempstr.Format("%s",unitSet.m_Name);
		tempstr.TrimLeft();
		if (tempstr.GetLength() < 5)
			continue;

		char a1 = tempstr.GetAt(0);
		char a2 = tempstr.GetAt(1);
		char a3 = tempstr.GetAt(2);
		char a4 = tempstr.GetAt(3);
		char a5 = tempstr.GetAt(4);

		if (a1 >= 65 && a1 <= 122)
			if (a2 >= 48 && a2 <= 57)
				if (a3 >= 48 && a3 <= 57){
					tempstr.SetAt(0,' ');
					tempstr.SetAt(1,' ');
					tempstr.SetAt(2,' ');
					tempstr.SetAt(3, ' ');
					tempstr.TrimLeft();
					unitSet.Edit();
					unitSet.m_Name = tempstr;
					unitSet.Update();
				}
				// get rid of 5-digit stuff
				if (a1 >= 48 && a1 <= 57)
					if (a2 >= 48 && a2 <= 57)
						if (a3 >= 48 && a3 <= 57)
							if (a4 >= 48 && a3 <= 57)
								if (a5 >= 48 && a3 <= 57){
									tempstr.SetAt(0,' ');
									tempstr.SetAt(1,' ');
									tempstr.SetAt(2,' ');
									tempstr.SetAt(3, ' ');
									tempstr.SetAt(4, ' ');
									tempstr.TrimLeft();
									unitSet.Edit();
									unitSet.m_Name = tempstr;
									unitSet.Update();
								}
								unitSet.MoveNext();
	}
	unitSet.Close();

	//if(!cd || !cd->Abort())
	//{
		fprintf(errLog, "Processed %ld  records\n"
			"\tUpdated %ld  units.\n"
			"\tUpdated %ld  regions.\n"
			"\tAdded %ld new units.\n"
			"\tAdded %ld new regions.\n"
			"\tRejected %ld records for bad data.\n",
			count, updated_units,updated_regions,new_units,new_regions,rejects);
		errors = rejects;
	/*}
	else
	{
		errors = -1;
		fprintf(errLog, "Import Cancelled by User.");
	}*/
	//if (cd)
	//	cd->DestroyWindow();

	EnableWindow(TRUE);

	return errors;
}

int CImportDialog::NewImportFW9(CString fileName, FILE *errLog, bool bOverwrite)
{
	std::list<double> prev23;
	for (int i = 0; i < 23; i++)
		prev23.push_back(0.0);
	int errors = 0;
	int staProcessed = 0;
	if (pCUPDUPData && pCUPDUPData->ShouldTerminate())
		return -1;
	char progText[256];
	//EnableWindow(FALSE);

	if(pDoc->HasTempWx())
	{
		try
		{
			CString strSql = "DROP TABLE [ffpTempWx]";
			regs.m_pDatabase->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}

		//pDoc->m_pDB->DeleteTableDef("ffpTempWx");
	}
	pDoc->CreateTempWx();
	fprintf(errLog, "\n*****************************************************************\nImport File Name: %s\n", fileName);
	CList<CString, CString> staList;
	CWxSet wxObs(regs.m_pDatabase);
	wxObs.m_strSort = "[ObsDate]";
	wxObs.Open();
	CWxSet twxObs(regs.m_pDatabase);
	twxObs.m_strSort = "[ObsDate]";
	twxObs.Open(CRecordset::dynaset, "ffpTempWx");

	CSIGStationSet stnSet(regs.m_pDatabase);
	stnSet.Open();
	CSIGStationSet wrccStnSet(regs.m_pDatabase);
	wrccStnSet.Open();
	FILE *stream = fopen(fileName, "rb");
	ifstream fin(fileName,ios_base::in|ios_base::binary);

	count = 0;
	long updates = 0, overwrite=0, daily = 0,raws = 0, fcast = 0, other = 0, nonwx = 0, rejects = 0, lineNo = 0;
	bool isRaws, isNFDRS;
	int staAdded = 0;
	int encType = 0;
	int prevPcpCode = -1, pcpCode = -1;
	if (! fin.eof())
	{
		//int canTransact = twxObs.m_pDatabase->CanTransact();
		int transStatus = twxObs.m_pDatabase->BeginTrans();
		char buf[161], temp[16];
		wchar_t bufw[161]; 
		CString sta, oldStationID, seekStr;
		oldStationID = " ";
		int m, d, y;

		// first check to see if we've got ASCII

		fin.read((char *) (&bufw),152);

		if (bufw[0] > 256)
			encType = 0;   // ASCII
		else
			encType = 1;   // UTF-16
	    fin.close();
		
		fin.open(fileName,ios_base::in|ios_base::binary);   // re-open

		//need to know precipitation code
		while (!fin.eof() && !pCUPDUPData->ShouldTerminate() && pcpCode < 0)// && (!cd || !cd->Abort()))
		{
			if (encType == 0)
			{   // Ascii / UTF-8
				fin.getline(buf, 1024);
			}
			else
			{   // UTF-16
				fin.read((char *)(&bufw), 152);
				for (int i = 0; i < 75; i++)
					buf[i] = (char)bufw[i + 2];
			}
			if (strlen(buf) > 63)
			{
				if (buf[0] != 'W')
				{
					continue;
				}
				strncpy(temp, &buf[62], 1);
				temp[1] = 0;
				//1 = running 24 inches, 2 = running 24 mm, 3 = hourly inches, 4 = hourly mm
				pcpCode = atoi(temp);
				if (pcpCode < 1 || pcpCode > 4)
					pcpCode = -1;
			}
		}
		if (pcpCode > 0)
			prevPcpCode = pcpCode;
		else
		{
			fprintf(errLog, "\tError: Can not determine precipitation code (column 63)\n", lineNo, errStrings[7]);
			errors++;
		}
		fin.seekg(0, fin.beg);
		int recNo = 0;
		while(!fin.eof() && !pCUPDUPData->ShouldTerminate())// && (!cd || !cd->Abort()))
		{
			recNo++;
			sprintf(progText, "Reading record %d", recNo);
				pCUPDUPData->SetProgress(progText);

			if (encType == 0) 
			{   // Ascii / UTF-8
				fin.getline(buf,1024);
			} else
			{   // UTF-16
				fin.read((char *) (&bufw),152);
				for (int i=0; i < 75 ; i++)
					buf[i] = (char) bufw[i+2];
			}
			
			if(fin.eof())
				break;
			int rtype = 0;
			lineNo++;
			isRaws = isNFDRS = false;
			//check buffer for NODATA since IBM can't understand fixed width fields....
			//NODATA is -9999 slammed anywhere into the record
			if(strstr(buf, "-9999") != NULL)
			{
				fprintf(errLog, "\tError: Line Number %ld, %s\n", lineNo, errStrings[7]);
				errors++;
				continue;
			}
			//check record type
			if (buf[0] != 'W')
			{
				nonwx++;
				continue;
			}

			if ( buf[1] == '1' && buf[2] == '3')
				rtype = 13;
			// FW13
			else if (buf[1] == '9' && buf[2] == '8')
				rtype = 9;
			// FW9
			else
			{
				nonwx++;
				continue;
			}
			switch(buf[21])
			{
			case 'R':
				isRaws = true;
				raws++;
				break;
			case 'F':
				fcast++;
				break;
			case 'O':
				isNFDRS = true;
				daily++;
				break;
			default:
				other++;
			}
			if(!isNFDRS && !isRaws)
				continue;
			//have type 'O' wx record
			//set record members to values from file
			strncpy(temp, &buf[3], 6);
			temp[6] = 0;
			CString sta = temp;
			sta.MakeLower();   // treat all WRCC IDs as lower case
			sta.TrimRight();
			if(sta.GetLength() == 0)//stationID can not be blank!!!!!!
			{
				fprintf(errLog, "\tError: Line Number %ld, %s\n", lineNo, errStrings[0]);
				errors++;
				continue;
			}
			// new 2013: add leading zero if WIMS ID is only 5 digits
			CString tempSta = sta;// .TrimLeft();
			tempSta.TrimLeft();
			if (tempSta.GetLength() == 5)
			{
				sta.Format("0%s",tempSta);
			}
			//doesn't check for duplicates!!!!!
			strncpy(temp, &buf[9], 4);
			temp[4] = 0;
			y = atoi(temp);
			strncpy(temp, &buf[13], 2);
			temp[2] = 0;
			m = atoi(temp);
			strncpy(temp, &buf[15], 2);
			temp[2] = 0;
			d = atoi(temp);
			COleDateTime y2kCheck;
			y2kCheck.SetDateTime(y, m, d, 0, 0, 0);
			if(y2kCheck.GetStatus() != 0 || y < 1900)
			{
				fprintf(errLog, "\tError: Line Number %ld, %s: %d/%d/%04d\n", lineNo, errStrings[1], m, d, y);
				rejects++;
				errors++;
				continue;
			}
			COleDateTime today;
			today = COleDateTime::GetCurrentTime();
			if(!theApp.m_enableFuture)
				if(y2kCheck > today)
				{
					fprintf(errLog, "\tError: Line Number %ld, %s: %d/%d/%04d\n", lineNo, errStrings[1], m, d, y);
					rejects++;
					errors++;
					continue;
				}

			twxObs.AddNew();
			if(staList.IsEmpty())
				staList.AddTail(sta);
			else
			{
				bool there = false;
				POSITION pos = staList.GetHeadPosition();
				while(pos)
				{
					if(staList.GetNext(pos) == sta)
					{
						there = true;
						break;
					}
				}
				if(!there)
					staList.AddHead(sta);
			}
			twxObs.m_StationID = sta;

			//if(isRaws)
			//{
			strncpy(temp, &buf[17], 2);
			temp[2] = 0;
			int hr = atoi(temp);
			if(hr >= 24 || hr < 0)
			{
				fprintf(errLog, "\tError: Line Number %ld, %s : %d\n", lineNo, errStrings[5], hr);
				errors++;
				continue;
			}

			// new change 9/2006 ljs
			// removed explicit minute field. minute now set to 0 (for nearest hour)
			//
			strncpy(temp, &buf[19], 2);
			temp[2] = 0;
			int mn = atoi(temp);
			if(mn > 60 || mn < 0)
			{
				fprintf(errLog, "\tError: Line Number %ld, %s : %d\n", lineNo, errStrings[6], mn);
				errors++;
				continue;
			}
			//twxObs.m_ObsDate.SetDateTime(y, m, d, hr, mn, 0);

			twxObs.m_ObsDate.SetDateTime(y, m, d, hr, 0, 0);

			// if obs minute was in latter part of hour, round up an hour
			if (mn > 30){
				COleDateTimeSpan ts(0,1,0,0);
				twxObs.m_ObsDate += ts;
			}
			twxObs.m_Hour = twxObs.m_ObsDate.GetHour();

			if(isRaws)
				twxObs.m_ObsType = "R";
			//}
			else //nfdrs
			{
				//CTime tTime(y, m, d, 13, 0, 0);
				//twxObs.m_ObsDate = tTime;
				//twxObs.m_ObsDate.SetDateTime(y, m, d, 13, 0, 0);
				twxObs.m_ObsType = "O";
			}
			strncpy(temp, &buf[22], 1);
			temp[1] = 0;
			if(temp[0] != ' ')
				twxObs.m_SOW = atoi(temp);
			else
				twxObs.SetFieldNull(&twxObs.m_SOW);
			strncpy(temp, &buf[23], 3);
			temp[3] = 0;
			int db;
			if(strncmp(temp, "   ", 3) != 0)
			{
				db = atoi(temp);
				twxObs.m_Temp = db;
			}
			else
			{
				twxObs.SetFieldNull(&twxObs.m_Temp);
				db = -999;
			}
			char t[2];
			t[0] = buf[61];
			t[1] = 0;
			strncpy(temp, &buf[26], 3);
			temp[3] = 0;
			if (strncmp(temp, "   ", 3) != 0)
			{
				twxObs.m_RH = RH(atoi(t), atoi(temp), db);
				twxObs.m_RH = max(twxObs.m_RH, 1);
			}
			//twxObs.m_RH = atoi(temp);
			else
				twxObs.SetFieldNull(&twxObs.m_RH);

			//****************WIND SPEED AND DIRECTION *****************
			strncpy(temp, &buf[29], 3);
			temp[3] = 0;
			int tdir = -1, tws = -1;
			if(strncmp(temp, "   ", 3) != 0)
				tdir = atoi(temp);
			//twxObs.m_WDir = atoi(temp);
			strncpy(temp, &buf[32], 3);
			temp[3] = 0;
			if(strncmp(temp, "   ", 3) != 0)
				tws = atoi(temp);
			if(tws >= 0 && tdir >= 0 && tdir <= 360)
			{
				//if(tws == 0)
				//	tdir = 0;
				//else
				//if(tdir > 8)
				//{//convert dir to 1 - 8 scale
				twxObs.m_WAzimuth = tdir;
				if(tdir > 337 || tdir < 23)
					tdir = 8;
				else if(tdir > 22 && tdir < 68)
					tdir = 1;
				else if(tdir > 67 && tdir < 113)
					tdir = 2;
				else if(tdir > 112 && tdir < 158)
					tdir = 3;
				else if(tdir > 157 && tdir < 203)
					tdir = 4;
				else if(tdir > 202 && tdir < 248)
					tdir = 5;
				else if(tdir > 247 && tdir < 293)
					tdir = 6;
				else if(tdir > 292 && tdir < 338)
					tdir = 7;
				//}
				twxObs.m_WDir = tdir;
				twxObs.m_WS = tws;
			}
			if(tdir < 0)
			{
				twxObs.SetFieldNull(&twxObs.m_WDir);
				twxObs.SetFieldNull(&twxObs.m_WAzimuth);
			}
			if(tws < 0)
				twxObs.SetFieldNull(&twxObs.m_WS);

			//	twxObs.m_WS = atoi(temp);

			//***************end winds ********************************
			strncpy(temp, &buf[35], 2);
			temp[2] = 0;
			if(strncmp(temp, "  ", 2) != 0 && strncmp(temp, "000", 2) != 0)
				twxObs.m_OMC10 = (float) (atof(temp));
			else
				twxObs.SetFieldNull(&twxObs.m_OMC10);
			strncpy(temp, &buf[37], 3);
			temp[3] = 0;
			if(strncmp(temp, "   ", 3) != 0 && strncmp(temp, "000", 3) != 0)
				twxObs.m_TmpMax = atoi(temp);
			else
				twxObs.SetFieldNull(&twxObs.m_TmpMax);
			strncpy(temp, &buf[40], 3);
			temp[3] = 0;
			if(strncmp(temp, "   ", 3) != 0 && strncmp(temp, "000", 3) != 0)
				twxObs.m_TmpMin = atoi(temp);
			else
				twxObs.SetFieldNull(&twxObs.m_TmpMin);
			strncpy(temp, &buf[43], 3);
			temp[3] = 0;
			if(strncmp(temp, "   ", 3) != 0 && strncmp(temp, "000", 3) != 0)
				twxObs.m_RHMax = atoi(temp);
			else
				twxObs.SetFieldNull(&twxObs.m_RHMax);
			strncpy(temp, &buf[46], 3);
			temp[3] = 0;
			if(strncmp(temp, "   ", 3) != 0 && strncmp(temp, "000", 3) != 0)
				twxObs.m_RHMin = max(atoi(temp), 1);
			else
				twxObs.SetFieldNull(&twxObs.m_RHMin);
			strncpy(temp, &buf[49], 2);
			temp[2] = 0;
			if(strncmp(temp, "  ", 2) != 0)
				twxObs.m_PPTDUR = atoi(temp);
			else
				twxObs.m_PPTDUR = 0;
			//NEED TO CHECK PRECIP MEASUREMENT TYPE CODE
			strncpy(temp, &buf[62], 1);
			temp[1] = 0;
			//1 = running 24 inches, 2 = running 24 mm, 3 = hourly inches, 4 = hourly mm
			pcpCode = atoi(temp);
			if (prevPcpCode <= 0)
				prevPcpCode = pcpCode;
			if (prevPcpCode != pcpCode)
			{
				//this is an error that never should happen
				fprintf(errLog, "\tError: Line Number %ld, %s : %d\n", lineNo, errStrings[8], pcpCode);
				errors++;
				continue;
			}
			strncpy(temp, &buf[51], 5);
			temp[5] = 0;
			switch (pcpCode)
			{
			case 1:
				twxObs.m_PPTAMT = atof(temp) / 1000.0;//inches have implied decimal point
				twxObs.SetFieldNull(&twxObs.m_HourlyPrecip);
				break;
			case 2:
				twxObs.m_PPTAMT = atof(temp) * 0.03937007874; //millimeter to inch
				twxObs.SetFieldNull(&twxObs.m_HourlyPrecip);
				break;
			case 3:
				twxObs.m_HourlyPrecip = atof(temp) / 1000.0;//inches have implied decimal point

				twxObs.SetFieldNull(&twxObs.m_PPTAMT);
				break;
			case 4:
				twxObs.m_HourlyPrecip = atof(temp) * 0.03937007874; //millimeter to inch
				twxObs.SetFieldNull(&twxObs.m_PPTAMT);
				break;
			default:
				fprintf(errLog, "\tError: Line Number %ld, %s : %d\n", lineNo, errStrings[9], pcpCode);
				errors++;
				continue;

			}
			/* old code, before intro of new pcp measurement type code
			if(strcmp(temp, "    ") == 0 && twxObs.m_PPTDUR > 0)
				twxObs.m_PPTAMT = 0.005;
			else
			{
				if(strchr(temp, '.'))//if there's a decimal point read it straight
					twxObs.m_PPTAMT = (atof(temp));
				else
					twxObs.m_PPTAMT = (atof(temp) / 1000.0);
			}*/
			//***********************************************************************************
			// added code to get wet flag   5/2006
			strncpy(temp, &buf[56],1);
			temp[1] = 0;
			if (temp[0] == 'Y' || temp[0] == 'y')
				twxObs.m_WetFlag = 1;
			else
				twxObs.m_WetFlag = 0;
			// end wet flag addition

			strncpy(temp, &buf[57], 2);
			temp[2] = 0;
			if(strncmp(temp, "  ", 2) != 0)
				twxObs.m_GreenHerb = atoi(temp);
			else
				twxObs.SetFieldNull(&twxObs.m_GreenHerb);
			//****************************************************************
			strncpy(temp, &buf[59], 2);
			temp[2] = 0;
			if(strncmp(temp, "  ", 2) != 0)
				twxObs.m_GreenShrub = atoi(temp);
			else
				twxObs.SetFieldNull(&twxObs.m_GreenShrub);
			strncpy(temp, &buf[63], 1);
			temp[1] = 0;
			if(temp[0] != ' ')
			{
				int seas = atoi(temp);
				if(seas >= 1 && seas <= 4)
					twxObs.m_Season = atoi(temp);
			}
			else
				twxObs.SetFieldNull(&twxObs.m_Season);

			// added code to get solar radiation 5/2006
			strncpy(temp, &buf[64],4);
			temp[4] = 0;
			CString tSlr = temp;
			tSlr.Trim();
			//if(strncmp(temp, "    ", 4) != 0)
			if (tSlr.GetLength() > 0)
			{
				int tSolRad = atoi(tSlr);
				if(tSolRad < 1400 && tSolRad >= 0)
					twxObs.m_SolarRadiation = tSolRad;// atoi(tSlr);
				else
					twxObs.SetFieldNull(&twxObs.m_SolarRadiation);
			}
			else
				twxObs.SetFieldNull(&twxObs.m_SolarRadiation);
			// end solar radiation addition

			//****************GUST SPEED AND DIRECTION *****************
			// 11/2012 added for FFP4.1
			strncpy(temp, &buf[68], 3);
			temp[3] = 0;
			int tgdir = -1, tgws = -1;
			if(strncmp(temp, "   ", 3) != 0)
				tgdir = atoi(temp);

			strncpy(temp, &buf[71], 3);
			temp[3] = 0;
			if(strncmp(temp, "   ", 3) != 0)
				tgws = atoi(temp);
			if(tgws >= 0 && tgdir > 0 && tgdir <= 360)
			{
				if (tgdir==360)
					tgdir = 0;

				twxObs.m_GustDir = tgdir;
				twxObs.m_GustSpd = tgws;
			}
			if(tgdir <= 0)
			{
				twxObs.SetFieldNull(&twxObs.m_GustDir);
			}
			if(tgws < 0)
				twxObs.SetFieldNull(&twxObs.m_GustSpd);

			//***************end gusts ********************************

			// added code to get snow flag
			// 11/2012 added for FFP4.1

			strncpy(temp, &buf[74],1);
			temp[1] = 0;
			if (temp[0] == 'Y' || temp[0] == 'y')
				twxObs.m_SnowFlag = 1;
			else if (temp[0] == 'N' || temp[0] == 'n')
				twxObs.m_SnowFlag = 0;
			else
				twxObs.SetFieldNull(&twxObs.m_SnowFlag);
			// end snow flag addition

			twxObs.Update();
		}
	}
	//ok, put into wxObs appending or editing
	COleDateTime lastDay;
	//CTime lastDay;
	long edits = 0, appends = 0, wetFlagEdits = 0, nDuplicates = 0;
	CString strSQLa, sta6;
	int recNo = 0;
	twxObs.m_pDatabase->CommitTrans(); //so we can requery

	POSITION pos = staList.GetHeadPosition();
	/*while (pos)//&& (!cd || !cd->Abort()))
	{//process for each station, doing type 'O' records first

		sta = staList.GetNext(pos);

		sta.Format("%6.6s", sta);
		strSQLa.Format("[StationID] = '%6.6s'", sta);

		// 09/2012 add functionality to set DailyObs field //
		//CSIGStationSet stnSet(fireSet.m_pDatabase);
		//stnSet.Open();
		// look for station record
		stnSet.m_strFilter.Format("[StationID] = '%6.6s'", sta);
		stnSet.Requery();
	}

	//to maintain referential integrity,
	pos = staList.GetHeadPosition();*/
	while (pos && !pCUPDUPData->ShouldTerminate())
	{//check each station
		bool skipStation = false;
		CString strSQL, sta;
		sta = staList.GetNext(pos);

		// new behavior 2013 FFP4.1
		// check for WRCC-WIMS station correlation BEFORE creating new station record
		CString wrccSta = sta;
		wrccSta.Trim();
		wrccSta.MakeLower();

		bool isWRCCstation = false;
		bool correlatedWRCC = false;
		bool existingStation = false;

		if (wrccSta.GetLength() < 6)
			isWRCCstation = true;

		// rebuild SQL string for wxObs
		CString strSQLb;
		strSQLb.Format("[StationID] = '%6.6s'", sta);
		if (isWRCCstation)
		{
			// check to see if there's already a corresponding WIMS station
			wrccStnSet.m_strFilter.Format("[WRCC_ID] = '%s'", wrccSta);
			//strSQLb.Format("[StationID] = '%4.4s'", sta);
			wrccStnSet.Requery();
			if (!wrccStnSet.IsEOF())
				if (wrccStnSet.m_StationID.Trim().GetLength() > 4)
					correlatedWRCC = true;
		}

		// is station in DB already?
		if (!stnSet.IsOpen())
			stnSet.Open();

		stnSet.m_strFilter.Format("[StationID] = '%6.6s'", sta);
		stnSet.Requery();
		if (!stnSet.IsEOF())
			existingStation = true;

		//wxObs.m_StationID = sta.Trim();

		CString stationLog;
		if (isWRCCstation)
			stationLog.Format("Using WRCC station ID: %s\n", sta);
		else
			stationLog.Format("Using WIMS station ID: %s\n", sta);

		// new case for correlated station: give user the option of importing
		// using either WIMS ID or WRCC ID
		if (correlatedWRCC)
		{
			stnSet.m_strFilter.Format("[WRCC_ID] = '%s'", wrccSta);
			stnSet.Requery();
			sta = stnSet.m_StationID;
			//wxObs.m_StationID = sta.Trim();

			CString message;
			message.Format("WRCC stationID '%s' matches WIMS stationID '%s - %s'.\n\nDo you want to: ", wrccSta, sta, stnSet.m_Name.Trim());
			CString wimsStation;
			wimsStation.Format("Import data to '%s'", sta);
			CString wrccStation;
			wrccStation.Format("Import data to '%s'", wrccSta);
			CImportWRCCDialog iwd(NULL, message, wimsStation, wrccStation);
			if (iwd.DoModal() != IDOK)
			{
				stationLog.Format("Skipping station ID: %s\n", sta);
				skipStation = true;
			}
			else
			{
				if (iwd.importWRCC == 1)
				{
					// use the WRCC ID

					stnSet.m_strFilter.Format("[StationID] = '%6.6s'", wrccSta);
					stnSet.Requery();
					//wxObs.m_StationID = wrccSta.Trim();
					sta = wrccSta;
					if (stnSet.IsEOF())
						existingStation = false;
				}
				else
					stationLog.Format("Using WIMS station ID: %s\n", sta);
				stnSet.m_strFilter.Format("[StationID] = '%6.6s'", sta);
				stnSet.Requery();
				if (!stnSet.IsEOF())
					existingStation = true;

			}
			pCUPDUPData->SetFrontmost();
		}

		fprintf(errLog, "%s", stationLog);

		if (skipStation)
			continue;

		// check to see if we need to add a new station
		if (!existingStation)
		{
			COleDateTime lookG(1999, 4, 15, 14, 0, 0);
			COleDateTime lookF(1999, 10, 15, 14, 0, 0);
			stnSet.AddNew();
			stnSet.m_StationID.Format("%6.6s", sta);
			stnSet.m_Name = _T(" ");
			stnSet.m_NFDRSFM = _T(" ");
			stnSet.m_Use88 = 0;
			stnSet.m_LatDegrees = 0;
			stnSet.m_LonDegrees = 0;
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
			stnSet.m_RegSchdObs = 13;
			theApp.m_NFDRS2016.iSetFuelModel('V');
			stnSet.m_SCM_V = theApp.m_NFDRS2016.GetSCMax();
			stnSet.m_MX_Humid_V = theApp.m_NFDRS2016.GetMxdHumid();
			theApp.m_NFDRS2016.iSetFuelModel('W');
			stnSet.m_SCM_W = theApp.m_NFDRS2016.GetSCMax();
			stnSet.m_MX_Humid_W = theApp.m_NFDRS2016.GetMxdHumid();
			theApp.m_NFDRS2016.iSetFuelModel('X');
			stnSet.m_SCM_X = theApp.m_NFDRS2016.GetSCMax();
			stnSet.m_MX_Humid_X = theApp.m_NFDRS2016.GetMxdHumid();
			theApp.m_NFDRS2016.iSetFuelModel('Y');
			stnSet.m_SCM_Y = theApp.m_NFDRS2016.GetSCMax();
			stnSet.m_MX_Humid_Y = theApp.m_NFDRS2016.GetMxdHumid();
			theApp.m_NFDRS2016.iSetFuelModel('Z');
			stnSet.m_SCM_Z = theApp.m_NFDRS2016.GetSCMax();
			stnSet.m_MX_Humid_Z = theApp.m_NFDRS2016.GetMxdHumid();

			wrccStnSet.m_strFilter.Format("[WRCC_ID] = '%s'", wrccSta);
			wrccStnSet.Requery();

			if (!wrccStnSet.IsEOF())
			{//got a match for a wrcc station (most likely an FPA fwx file)
				wrccStnSet.MoveFirst();
				//pull in data from station by wrcc_ID
				stnSet.m_Name = _T(wrccStnSet.m_Name);
				stnSet.m_NFDRSFM = wrccStnSet.m_NFDRSFM;
				stnSet.m_Use88 = wrccStnSet.m_Use88;
				stnSet.m_LatDegrees = wrccStnSet.m_LatDegrees;
				stnSet.m_LonDegrees = wrccStnSet.m_LonDegrees;
				stnSet.m_Elevation = wrccStnSet.m_Elevation;
				stnSet.m_ClimateCls = wrccStnSet.m_ClimateCls;
				stnSet.m_SlopeCls = wrccStnSet.m_SlopeCls;
				stnSet.m_HerbAnnual = wrccStnSet.m_HerbAnnual;
				stnSet.m_GreenJulian = wrccStnSet.m_GreenJulian;
				stnSet.m_FreezeJulian = wrccStnSet.m_FreezeJulian;
				stnSet.m_StartKBDI = wrccStnSet.m_StartKBDI;
				stnSet.m_Deciduous = wrccStnSet.m_Deciduous;
				stnSet.m_AvgPrecip = wrccStnSet.m_AvgPrecip;
				stnSet.m_Start1000 = wrccStnSet.m_Start1000;
				stnSet.m_FM1Eq10 = wrccStnSet.m_FM1Eq10;
				stnSet.m_Aspect = wrccStnSet.m_Aspect;
				stnSet.m_PsnOnSlope = wrccStnSet.m_PsnOnSlope;
				stnSet.m_RegSchdObs = wrccStnSet.m_RegSchdObs;
				stnSet.m_StnType = wrccStnSet.m_StnType;
				stnSet.m_ObsAgy = wrccStnSet.m_ObsAgy;
				stnSet.m_ObsUnit = wrccStnSet.m_ObsUnit;
				stnSet.m_Site = wrccStnSet.m_Site;
				stnSet.m_State = wrccStnSet.m_State;

				stnSet.m_UseStick = wrccStnSet.m_UseStick;
				stnSet.m_UseDormant = wrccStnSet.m_UseDormant;
				stnSet.m_DormantJulian = wrccStnSet.m_DormantJulian;

				stnSet.m_FSRegion = wrccStnSet.m_FSRegion;

				stnSet.m_County = wrccStnSet.m_County;
				stnSet.m_WRCC_ID = wrccStnSet.m_WRCC_ID;
				stnSet.m_NESDIS_ID = wrccStnSet.m_NESDIS_ID;

				fprintf(errLog, "Added Station: %s\n\t*Note: New station ID %s matched WRCC ID %s\n"
					"\tPopulating new station record with %s station data.\n",
					sta, sta, wrccStnSet.m_WRCC_ID, wrccStnSet.m_StationID);
			}
			else
				fprintf(errLog, "Added Station: %s\n", sta);

			stnSet.Update();

			staAdded++;
		}
		staProcessed++;
		int regHour = 13;
		if (!stnSet.IsEOF())
		{
			regHour = stnSet.m_RegSchdObs;
		}
		stnSet.Close();
		// rebuild SQL string for wxObs
		strSQLa.Format("[StationID] = '%6.6s'", sta);
		sta6.Format("%6.6s", sta);



		wxObs.m_strFilter = strSQLa;
		twxObs.m_strFilter = strSQLb;
		BOOL wxRequery = wxObs.Requery();
		BOOL tWxRequery = twxObs.Requery();

		//right here, ensure precipitation fields are updated due to only one type of precip being passed in
		if (pcpCode == 1 || pcpCode == 2)
		{
			sprintf(progText, "Calculating Hourly Precip for %s", sta);
			pCUPDUPData->SetProgress(progText);
			twxObs.CalcHourlyPrecip(sta);
		}
		else if (pcpCode == 3 || pcpCode == 4)
		{
			sprintf(progText, "Calculating 24 Hour Precip for %s", sta);
			pCUPDUPData->SetProgress(progText);
			twxObs.Calc24HourPrecip(sta);
		}

		lastDay.SetDateTime(1900, 1, 1, 1, 1, 1);//minimum start date
		twxObs.m_pDatabase->BeginTrans();
		while (!twxObs.IsEOF() && !pCUPDUPData->ShouldTerminate())// && (!cd || !cd->Abort()))
		{
			bool tryEdit = false;
			bool thisEdit = false;
			count++;
			sprintf(progText, "Updating WxObs %ld", count);
			pCUPDUPData->SetProgress(progText);
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


				fprintf(errLog, "ERROR: StationID %s Date: %s duplicate record rejected (time duplicate)\n",
					twxObs.m_StationID, twxObs.m_ObsDate.Format("%m/%d/%y %H:%M"));
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
				&& wxObs.m_StationID == sta6)//twxObs.m_StationID)
			{


				tryEdit = true;
				//edits++;
				//if(wxObs.Is
				//twxObs.MoveNext();
				//continue;
				wxObs.Edit();

				ATLTRACE("Calling Edit for stationID: %s, %d, %d, %d, %d\n", twxObs.m_StationID, twxObs.m_ObsDate.GetYear(), twxObs.m_ObsDate.GetMonth(), twxObs.m_ObsDate.GetDay(), twxObs.m_ObsDate.GetHour());

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
			}

			// force stationID to the WIMS ID used for import
			// (twxObs WRCC ID may correlate to wxObs WIMS ID)

			wxObs.m_StationID.Format("%6.6s", sta);

			bool replaceAll = bOverwrite;

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

			/* 09/2012   set DailyObs field appropriately *///hah ha ha nice try, can allow duplicates
			wxObs.m_DailyObs = 0;
			if (wxObs.m_ObsType.CompareNoCase("O") == 0)
				wxObs.m_DailyObs = 1;
			else
				if ((wxObs.m_ObsType.CompareNoCase("R") == 0)
					&& (wxObs.m_Hour == regHour)
					&& (!wxObs.IsFieldNull(&wxObs.m_SOW)))
					wxObs.m_DailyObs = 1;
			try
			{
				wxObs.Update();
				//wxRequery = wxObs.Requery();
			}
			catch (CDBException *e)
			{
				e->Delete();
			}
			if (tryEdit && thisEdit)
				edits++;
			LogWxDataWarnings(errLog, &twxObs);
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
			}
		}
		else
			twxObs.m_pDatabase->Rollback();
		pCUPDUPData->AllowCancel(TRUE);
	}

	if (raws > count)
		raws = count;

	fprintf(errLog, "Processed %ld weather records for %d stations:\n", count, staProcessed);

	if (staProcessed > 0)
	{
		fprintf(errLog,
			"\tProcessed %ld NFDRS weather records.\n"
			"\tProcessed %ld RAWS records.\n", count - raws, raws);
		if (appends > 0)
			fprintf(errLog, "\tAppended %ld records.\n", appends);

		if (nDuplicates > 0)
			fprintf(errLog, "\tRejected %ld duplicate records.\n", nDuplicates);

		if (edits >= 0)
			fprintf(errLog, "\tUpdated %ld records.\n", edits);

		if (wetFlagEdits > 0)
			fprintf(errLog, "\tEdited %ld previously blank Wet Flag fields for existing type O records.\n", wetFlagEdits);

		//if (overwrite > 0)
			//fprintf(errLog, "\tReplaced %ld existing records with updated type O observations.\n", overwrite);

		if (nonwx > 0)
			fprintf(errLog, "\tDisregarded %ld non-weather records.\n", nonwx);

		if (fcast > 0)
			fprintf(errLog, "\tDisregarded %ld forecast records.\n", fcast);

		if (other > 0)
			fprintf(errLog, "\tDisregarded %ld other weather records.\n", other);

		if (rejects > 0)
			fprintf(errLog, "\tRejected %ld records for bad date field.\n", rejects);

		if (staAdded > 0)
			fprintf(errLog, "\tAdded %d new stations.\n", staAdded);



	}

	fclose(stream);

	twxObs.Close();
	stnSet.Close();
	wrccStnSet.Close();
	wxObs.Close();
	//if (cd)
	//	cd->DestroyWindow();
	staList.RemoveAll();
	try
	{
		pDoc->m_pDB->ExecuteSQL("DROP TABLE [ffpTempWx]");
	}
	catch (CDBException* e)
	{
		e->Delete();
	}
	return errors;
}

	void CImportDialog::OnFwx()
	{
		chdir(theApp.userDir);
		CFileDialog	dlg (TRUE, _T("fwx"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
			OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
			_T("Fire Weather Transfer Files (*.fwx)|*.fwx|Text Files (*.txt;*.csv)|*.txt;*.csv|All Files (*.*)|*.*||"), this);
		char *mfBuf = new char[MF_BUFSIZE];
		mfBuf[0] = 0;
		dlg.m_ofn.lpstrFile = mfBuf;
		dlg.m_ofn.nMaxFile = MF_BUFSIZE;
		int res = dlg.DoModal();
		if (res == IDOK)
		{
			CWaitCursor wait;
			long errors = 0;
			//if(!pDoc->HasTempWx())
			//	pDoc->CreateTempWx();
			// Get the position of the first filename
			char *logFileName = GetTempFileName();
			FILE * logFile = fopen(logFileName, "wt");
			COleDateTime dateTime = COleDateTime::GetCurrentTime();
			fprintf(logFile, "FireFamily Plus FWX Weather Transfer File Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			POSITION filenamePosition = dlg.GetStartPosition();
			CString fileName;

			vector<string> importNames;
			// Open each file selected
			int tErrors = 0;
			while ( filenamePosition != NULL )
			{
				fileName = dlg.GetNextPathName( filenamePosition );
				importNames.push_back(fileName.GetBuffer());
				/*tErrors = NewImportFWX(fileName, logFile);
				if(tErrors >= 0)
					errors += tErrors;
				else
				{//user cancelled
					errors = -1;
					break;
				}*/
			}
			//if(errors >= 0)
			//{
			dateTime = COleDateTime::GetCurrentTime();
			CString strMsg;
			if (IDOK == RunFWXImport(importNames, logFile, &tErrors))
				//if(errors >= 0)
			{
				fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
				strMsg.Format("Import Complete.\n%ld errors were encountered.", errors);
			}
			else
			{
				fprintf(logFile, "\n****** Import Cancelled by user. **********\n");
				strMsg.Format("Import cancelled. Some data may have been imported.\nPlease check the log file for complete information.");
			}
			EnableWindow(TRUE);
			fclose(logFile);
			CImportDoneDialog idd(this);
			idd.statusStr = strMsg;
			idd.logFileName = logFileName;
			delete[] logFileName;
			idd.DoModal();
			theApp.SetUserDir(dlg.GetPathName());
		}
		else if(res == IDCANCEL)
		{
			int err = CommDlgExtendedError();
			if(err == FNERR_BUFFERTOOSMALL)
			{
				AfxMessageBox("File Name buffer overrun, select fewer files.");
			}
		}
		delete[] mfBuf;
		chdir(theApp.dbDir);
	}

	void CImportDialog::OnRaw()
	{
		long agencyID = -1;
		if(agencies.GetCurSel() != CB_ERR)
			agencyID = agencies.GetItemData(agencies.GetCurSel());
		else
		{//report no agency selected
			CString strMsg = "An Agency must be selected in order to import fire data.";
			AfxMessageBox(strMsg);
			return;
		}
		chdir(theApp.userDir);
		CFileDialog	dlg (TRUE, _T("raw"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
			OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
			_T("Fire Record Files (*.raw)|*.raw|Text Files (*.txt;*.csv)|*.txt;*.csv|All Files (*.*)|*.*||"), this);
		char *mfBuf = new char[MF_BUFSIZE];
		mfBuf[0] = 0;
		dlg.m_ofn.lpstrFile = mfBuf;
		dlg.m_ofn.nMaxFile = MF_BUFSIZE;
		int res = dlg.DoModal();
		nNewSubunits = nNewUnits = 0;
		if (res == IDOK)
		{
			CWaitCursor wait;
			long errors = 0;
			// Get the position of the first filename
			char *logFileName = GetTempFileName();
			FILE * logFile = fopen(logFileName, "wt");
			COleDateTime dateTime = COleDateTime::GetCurrentTime();
			fprintf(logFile, "FireFamily Plus RAW Fire Data Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			POSITION filenamePosition = dlg.GetStartPosition();
			CString fileName;

			// Open each file selected
			int tErrors;
			while ( filenamePosition != NULL )
			{
				fileName = dlg.GetNextPathName( filenamePosition );
				tErrors = ImportRAW(fileName, logFile, agencyID);
				if(tErrors >= 0)
					errors += tErrors;
				else
				{//user cancelled
					errors = -1;
					break;
				}
			}
			dateTime = COleDateTime::GetCurrentTime();
			CString strMsg;
			if(errors >= 0)
			{
				fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
				strMsg.Format("Import Complete.\n%ld errors were encountered.", errors);
				if(nNewUnits > 0)
				{
					CString nuStr;
					nuStr.Format("\n%ld new Fire Units were created.", nNewUnits);
					strMsg = strMsg + nuStr;
				}
				if(nNewSubunits > 0)
				{
					CString nuStr;
					nuStr.Format("\n%ld new Fire SubUnits were created.", nNewSubunits);
					strMsg = strMsg + nuStr;
				}
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
			theApp.SetUserDir(dlg.GetPathName());
		}
		else if(res == IDCANCEL)
		{
			int err = CommDlgExtendedError();
			if(err == FNERR_BUFFERTOOSMALL)
			{
				AfxMessageBox("File Name buffer overrun, select fewer files.");
			}
		}
		delete[] mfBuf;
		chdir(theApp.dbDir);
	}

	BOOL CImportDialog::OnInitDialog()
	{
		CDialog::OnInitDialog();

		// TODO: Add extra initialization here
		int loc = agencies.AddString("USFS");
		if(loc >= 0)
			agencies.SetItemData(loc, 1);
		loc = agencies.AddString("DOI Agencies");
		if(loc >= 0)
			agencies.SetItemData(loc, 5);
		CFireAgencySet agencySet(regs.m_pDatabase);
		agencySet.Open();
		while(!agencySet.IsEOF())
		{
			if(agencySet.m_AgencyID > 5) //Custom agency
			{
				loc = agencies.AddString(agencySet.m_Name);
				if(loc >= 0)
					agencies.SetItemData(loc, agencySet.m_AgencyID);
				if (agencySet.m_Name.Compare("GACC") == 0)
					m_GACC_Agency_ID = agencySet.m_AgencyID;
			}
			agencySet.MoveNext();
		}
		agencySet.Close();

		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}

	void CImportDialog::GetFireRecRAW(char * buf, FILE * stream, long agencyID, bool threeLineFormat)
	{
		//read a fire rec, based on agency type...(grrr)
		if(agencyID == 1)//USFS
		{
			//blank out 262 chars
			int c = 0, r;
			int i;
			for(i = 0; i < 262; i++)
				buf[i] = ' ';
			char temp[270];
			if(!threeLineFormat)
			{
				while(c < 262 && !feof(stream))
				{
					if(!fgets(temp, 262, stream))
						break;
					r = strlen(temp);
					for(i = 0; i < r && c < 263; i++)
						if(isprint(temp[i]))
							buf[c++] = temp[i];
				}
			}
			else //old, three lines per record format
			{
				fgets(temp, 110, stream);
				for(i = 0; i < 100; i++)
					if(i < (int)strlen(temp) && isprint(temp[i]))
						buf[i] = temp[i];
				fgets(temp, 110, stream);
				for(i = 0; i < 100; i++)
					if(i < (int)strlen(temp) && isprint(temp[i]))
						buf[i + 100] = temp[i];
				fgets(temp, 110, stream);
				for(i = 0; i < 62; i++)
					if(i < (int)strlen(temp) && isprint(temp[i]))
						buf[i + 200] = temp[i];
			}
			buf[262] = 0;
		}
		fplLineNo++;
	}

	int CImportDialog::GetFireRecFPL(char * buf, FILE * stream, long agencyID, CFireFilterDialog *ffd)
	{
		//read a fire rec, based on agency type...(grrr)
		switch(agencyID)
		{
		case 1://USFS
			{
				//blank out 80 chars
				int i;
				for(i = 0; i < 80; i++)
					buf[i] = ' ';
				char temp[90];
				fgets(temp, 90, stream);
				fplLineNo++;
				for(i = 0; i < 80; i++)
					if(i < (int)strlen(temp) && isprint(temp[i]))
						buf[i] = temp[i];
				buf[80] = 0;
			}
			break;
		case 2://BIA **Bunch of extra info, condense
		case 3://BLM
		case 4://NPS
		case 5://FWS
			{
				char temp[851];//wow!
				fgets(temp, 850, stream);
				fplLineNo++;
				//check type first!
				bool goodRec = false;
				strncpy(buf, &temp[19], 2);
				buf[2] = 0;
				switch(atoi(buf))
				{
				case 11:
					if(ffd->type11)
						goodRec = true;
					break;
				case 12:
					if(ffd->type12)
						goodRec = true;
					break;
				case 13:
					if(ffd->type13)
						goodRec = true;
					break;
				case 14:
					if(ffd->type14)
						goodRec = true;
					break;
				case 15:
					if(ffd->type15)
						goodRec = true;
					break;
				case 16:
					if(ffd->type16)
						goodRec = true;
					break;
				case 19:
					if(ffd->type19)
						goodRec = true;
					break;
				case 21:
					if(ffd->type21)
						goodRec = true;
					break;
				case 22:
					if(ffd->type22)
						goodRec = true;
					break;
				case 23:
					if(ffd->type23)
						goodRec = true;
					break;
				case 25:
					if(ffd->type25)
						goodRec = true;
					break;
				case 26:
					if(ffd->type26)
						goodRec = true;
					break;
				case 37:
					if(ffd->type37)
						goodRec = true;
					break;
				case 48:
					if(ffd->type48)
						goodRec = true;
					break;
				case 49:
					if(ffd->type49)
						goodRec = true;
					break;
				}
				if(!goodRec)
					return 0;
				strncpy(buf, temp, 8);//unitid
				strncpy(&buf[8], &temp[9], 4);//year
				strncpy(&buf[12], &temp[14], 4);//fire number
				strncpy(&buf[16], &temp[38], 10);//fire name
				strncpy(&buf[26], &temp[54], 6);//lat
				strncpy(&buf[32], &temp[61], 7);//long(positive)
				strncpy(&buf[39], &temp[102], 4);//township
				strncpy(&buf[43], &temp[107], 4);//range
				strncpy(&buf[47], &temp[112], 2);//section
				strncpy(&buf[49], &temp[140], 6);//disc date
				strncpy(&buf[55], &temp[147], 4);//disc time
				strncpy(&buf[59], &temp[163], 6);//init attack date
				strncpy(&buf[65], &temp[170], 4);//init attack time
				strncpy(&buf[69], &temp[209], 6);//control date
				strncpy(&buf[75], &temp[216], 4);//control time
				strncpy(&buf[79], &temp[221], 8);//control acres
				strncpy(&buf[87], &temp[230], 6);//dec out date
				strncpy(&buf[93], &temp[299], 2);//state
				strncpy(&buf[95], &temp[22], 1);//stat cause
				if(buf[95] == '0')
					buf[95] = '1';
				strncpy(&buf[96], &temp[24], 2);//specific cause
				if((buf[96] == '0' || buf[96] == ' ') && (buf[97] == '0' || buf[97] == ' '))
					buf[97] = '1';
				buf[98] = 0;
			}
			break;
		}//end switch
		return 1;
	}

	int CImportDialog::WriteFireRecFPL(char *buf, long agencyID,
		FILE *errLog, CFiresSet *tFires)
	{
		bool append = true;
		char temp[16];
		int m, d, y, hr, mn;
		long regID, unitID;
		CString str;
		char newUnitStr[64];
		COleDateTime dt;
		utils util(firesSet.m_pDatabase);

		switch(agencyID)
		{
		case 1://USFS
			{	//check if record already exists
				//agency == 1, year + reg + unit + firenum is unique
				strncpy(temp, buf, 2);
				temp[2] = 0;
				if(temp[0] == ' ')
					temp[0] = '0';
				if(temp[1] == ' ')
					temp[1] = '0';
				regID = GetFireRegion(&regs, agencyID, temp);
				if(regID == 0)
				{
					fprintf(errLog, "\tRecord Number %ld, %s(Region): '%s'\n", fplLineNo, errStrings[2], temp);
					return -2;
				}
				strncpy(temp, &buf[2], 2);
				temp[2] = 0;
				if(temp[0] == ' ')
					temp[0] = '0';
				if(temp[1] == ' ')
					temp[1] = '0';
				unitID = GetFireUnit(&units, agencyID, regID, temp);
				if(unitID == 0)
					strcpy_s(newUnitStr, temp);
				//if(unitID == 0)
				//{
				//		fprintf(errLog, "\tRecord Number %ld, %s(Forest): '%s'\n", fplLineNo, errStrings[3], temp);
				//		return -3;
				//	}
				strncpy(temp, &buf[8], 2);
				temp[2] = 0;
				y = atoi(temp);
				if(y >= PIVOT)
					y += 1900;
				else
					y += 2000;
				strncpy(temp, &buf[10], 2);
				temp[2] = 0;
				m = atoi(temp);
				strncpy(temp, &buf[12], 2);
				temp[2] = 0;
				d = atoi(temp);
				strncpy(temp, &buf[14], 2);
				temp[2] = 0;
				hr = atoi(temp);
				dt.SetDateTime(y, m, d, hr, 0, 0);
				if(dt.GetStatus() != 0 || y < 1900)
				{
					fprintf(errLog, "\tRecord Number %ld, %s: %d/%d/%04d\n", fplLineNo, errStrings[1], m, d, y);
					return -1;
				}
				COleDateTime today;
				today = COleDateTime::GetCurrentTime();
				if(dt > today)
				{
					fprintf(errLog, "\tRecord Number %ld, %s: %d/%d/%04d\n", fplLineNo, errStrings[1], m, d, y);
					return -1;
				}
				if(unitID == 0)
				{
					unitID = AddFireUnit(&units, agencyID, regID, newUnitStr);
					fprintf(errLog, "\tRecord Number %ld, Added New Unit: '%2s Unknown'\n", fplLineNo, newUnitStr);
					nNewUnits++;
				}
				strncpy(temp, &buf[5], 3);
				temp[3] = 0;
				//tFires->m_strFilter.Format("[Year] = '%d' and [AgencyID] = 1 and [RegionID] = %ld and [UnitID] = %ld and [FireNumber] = '%s'",
				//	dt.GetYear(), regID, unitID, temp);
				//tFires->Requery();
				//if(tFires->IsEOF() && tFires->IsBOF())
				//{
				tFires->AddNew();
				//tFires->m_FireID = ++(*fireID);
				//}
				//else
				//{
				//append = false;
				//tFires->Edit();
				//}
				str.Format("%3.3s", temp);
				tFires->m_FireNumber = str;
				tFires->m_AgencyID = agencyID;
				tFires->m_RegionID = regID;
				tFires->m_UnitID = unitID;
				tFires->SetFieldNull(&tFires->m_SubunitID);
				COleDateTime tTime(y, m, d, hr, 0, 0);
				tFires->m_Discovery = tTime;
				str.Format("%d", dt.GetYear());
				//tFires->m_Year = str;
				if(buf[16] == 'L')//Lat/Long dd MM
				{
					double dd = 0, mm = 0, ss = 0;
					strncpy(temp, &buf[17], 2);
					temp[2] = 0;
					if (strlen(trim(temp)) > 0)
					{
						dd = atof(temp);
						strncpy(temp, &buf[19], 2);
						temp[2] = 0;
						if (strlen(trim(temp)) > 0)
							mm = atof(temp);
						tFires->m_latitude = dd + mm / 60.0;
					}
					else
						tFires->SetFieldNull(&tFires->m_latitude);
					//else
					//	tFires->SetFieldNull(&tFires->m_LatMM);
					//tFires->SetFieldNull(&tFires->m_LatSS);
					dd = 0; 
					mm = 0; 
					ss = 0;
					strncpy(temp, &buf[21], 3);
					temp[3] = 0;
					if (strlen(trim(temp)) > 0)
					{
						dd = -atof(temp);
						strncpy(temp, &buf[24], 2);
						temp[2] = 0;
						if (strlen(trim(temp)) > 0)
							mm = atof(temp);
						tFires->m_longitude = dd - mm / 60.0;
					}
					else
						tFires->SetFieldNull(&tFires->m_longitude);
					//tFires->SetFieldNull(&tFires->m_LonSS);
				}
				else
				{
					tFires->SetFieldNull(&tFires->m_latitude);
					tFires->SetFieldNull(&tFires->m_longitude);
					//tFires->SetFieldNull(&tFires->m_LatSS);
					//tFires->SetFieldNull(&tFires->m_LonDD);
					//tFires->SetFieldNull(&tFires->m_LonMM);
					//tFires->SetFieldNull(&tFires->m_LonSS);
				}
				strncpy(temp, &buf[26], 1);
				temp[1] = 0;
				if(strlen(trim(temp)) > 0)
				{
					int causeVal = atoi(temp);
					bool validCause = util.validateStatCause(causeVal);
					if (validCause)
						 tFires->m_StatisticalCause = atoi(temp);
					else
					{
						fprintf(errLog, "\tRecord Number %ld, invalid statistical cause: %s\n", fplLineNo, temp);
						return -1;
					 }
		
				} else
					tFires->SetFieldNull(&tFires->m_StatisticalCause);
				/*strncpy(temp, &buf[28], 1);
				temp[1] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_GeneralCause = atol(temp);
				else
					tFires->SetFieldNull(&tFires->m_GeneralCause);
				strncpy(temp, &buf[29], 2);
				temp[2] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_SpecificCause = atol(temp);
				else
					tFires->SetFieldNull(&tFires->m_SpecificCause);
				strncpy(temp, &buf[31], 1);
				temp[1] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_ClassPeople = atol(temp);
				else
					tFires->SetFieldNull(&tFires->m_ClassPeople);
				str.Format("%1.1s", &buf[32]);
				str.TrimRight();
				if(str.GetLength() > 0)
					tFires->m_SizeClass = str;
				else
					tFires->SetFieldNull(&tFires->m_SizeClass);*/
				strncpy(temp, &buf[33], 7);
				temp[7] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_TotalAcres = atof(temp);
				else
					tFires->m_TotalAcres = 0;
				//NULL OUT REMAINING FIELDS IF NOT APPENDING
				if(!append)
				{
					/*tFires->SetFieldNull(&tFires->m_Township);
					tFires->SetFieldNull(&tFires->m_Range);
					tFires->SetFieldNull(&tFires->m_Section);
					tFires->SetFieldNull(&tFires->m_SubSection);
					tFires->SetFieldNull(&tFires->m_State);
					tFires->SetFieldNull(&tFires->m_County);
					tFires->SetFieldNull(&tFires->m_Ignition);
					tFires->SetFieldNull(&tFires->m_FirstAttack);
					tFires->SetFieldNull(&tFires->m_Reinforcement);
					tFires->SetFieldNull(&tFires->m_DeclareWildlandFire);*/
					tFires->SetFieldNull(&tFires->m_Contain);
					tFires->SetFieldNull(&tFires->m_StrategyMet);
					//tFires->SetFieldNull(&tFires->m_FireOut);
					tFires->SetFieldNull(&tFires->m_FireName);
					//tFires->SetFieldNull(&tFires->m_WildNum);
				}
				tFires->Update();
			}
			break;
			//dept. of interior agencies
		case 2://BIA
		case 3://BLM
		case 4://NPS
		case 5://FWS
			{
				char firNum[8];
				strncpy(temp, &buf[8], 4);
				temp[4] = 0;
				y = atoi(temp);
				strncpy(temp, &buf[51], 2);
				temp[2] = 0;
				m = atoi(temp);
				strncpy(temp, &buf[53], 2);
				temp[2] = 0;
				d = atoi(temp);
				strncpy(temp, &buf[55], 2);
				temp[2] = 0;
				hr = atoi(temp);
				strncpy(temp, &buf[57], 2);
				temp[2] = 0;
				mn = atoi(temp);
				dt.SetDateTime(y, m, d, hr, mn, 0);
				if(dt.GetStatus() != 0 || y < 1900)
				{
					fprintf(errLog, "\tRecord Number %ld, %s: %d/%d/%04d Hour: %d Min: %d\n",
						fplLineNo, errStrings[1], m, d, y, hr, mn);
					return -1;
				}
				COleDateTime today;
				today = COleDateTime::GetCurrentTime();
				if(dt > today)
				{
					fprintf(errLog, "\tRecord Number %ld, %s: %d/%d/%04d Hour: %d Min: %d\n",
						fplLineNo, errStrings[1], m, d, y, hr, mn);
					return -1;
				}
				strncpy(firNum, &buf[12], 4);
				firNum[4] = 0;
				switch(agencyID)
				{
				case 2://BIA
					{//check if record already exists
						//agency == 1, year + reg + unit + firenum is unique
						strncpy(temp, buf, 3);
						temp[3] = 0;
						regID = GetFireRegion(&regs, agencyID, temp);
						if(regID == 0)
						{
							fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[2], temp);
							return -2;
						}
						strncpy(temp, &buf[3], 4);
						temp[4] = 0;
						unitID = GetFireUnit(&units, agencyID, regID, temp);
						if(unitID == 0)
						{
							unitID = AddFireUnit(&units, agencyID, regID, temp);
							fprintf(errLog, "\tRecord Number %ld, Added New Unit: '%4s Unknown'\n", fplLineNo, temp);
							nNewUnits++;
						}
						//if(unitID == 0)
						//{
						//	fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[3], temp);
						//	return -3;
						//}
						//tFires->m_strFilter.Format("[Year] = '%d' and [AgencyID] = 2 and [RegionID] = %ld and [UnitID] = %ld and [FireNumber] = '%s'",
						//	dt.GetYear(), regID, unitID, firNum);
					}
					break;
				case 3://BLM
					{//check if record already exists
						//agency == 1, year + reg + unit + firenum is unique
						strncpy(temp, buf, 2);
						temp[2] = 0;
						regID = GetFireRegion(&regs, agencyID, temp);
						if(regID == 0)
						{
							fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[2], temp);
							return -2;
						}
						strncpy(temp, buf, 5);
						temp[5] = 0;
						unitID = GetFireUnit(&units, agencyID, regID, temp);
						if(unitID == 0)
						{
							unitID = AddFireUnit(&units, agencyID, regID, temp);
							fprintf(errLog, "\tRecord Number %ld, Added New Unit: '%5s Unknown'\n", fplLineNo, temp);
							nNewUnits++;
						}
						//if(unitID == 0)
						//{
						//	fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[3],temp);
						//	return -3;
						//	}
						//tFires->m_strFilter.Format("[Year] = '%d' and [AgencyID] = 3 and [RegionID] = %ld and [UnitID] = %ld and [FireNumber] = '%s'",
						//	dt.GetYear(), regID, unitID, firNum);
					}
					break;
				case 4://NPS
					{//check if record already exists
						//agency == 1, year + reg + unit + firenum is unique
						strncpy(temp, buf, 4);
						temp[4] = 0;
						regID = GetFireRegion(&regs, agencyID, temp);
						if(regID == 0)
						{
							fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[2], temp);
							return -2;
						}
						strncpy(temp, &buf[4], 4);
						temp[4] = 0;
						unitID = GetFireUnit(&units, agencyID, regID, temp);
						if(unitID == 0)
						{
							unitID = AddFireUnit(&units, agencyID, regID, temp);
							fprintf(errLog, "\tRecord Number %ld, Added New Unit: '%4s Unknown'\n", fplLineNo, temp);
							nNewUnits++;
						}
						//if(unitID == 0)
						//{
						//	fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[3], temp);
						//	return -3;
						//}
						//tFires->m_strFilter.Format("[Year] = '%d' and [AgencyID] = 4 and [RegionID] = %ld and [UnitID] = %ld and [FireNumber] = '%s'",
						//	dt.GetYear(), regID, unitID, firNum);
					}
					break;
				case 5://FWS
					{//check if record already exists
						//agency == 1, year + reg + unit + firenum is unique
						sprintf(temp, "%c0000", buf[1]);
						regID = GetFireRegion(&regs, agencyID, temp);
						if(regID == 0)
						{
							fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[2], temp);
							return -2;
						}
						strncpy(temp, &buf[1], 5);
						temp[5] = 0;
						unitID = GetFireUnit(&units, agencyID, regID, temp);
						if(unitID == 0)
						{
							unitID = AddFireUnit(&units, agencyID, regID, temp);
							fprintf(errLog, "\tRecord Number %ld, Added New Unit: '%5s Unknown'\n", fplLineNo, temp);
							nNewUnits++;
						}
						//if(unitID == 0)
						//{
						//	fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[3], temp);
						//	return -3;
						//}
						//tFires->m_strFilter.Format("[Year] = '%d' and [AgencyID] = 5 and [RegionID] = %ld and [UnitID] = %ld and [FireNumber] = '%s'",
						//	dt.GetYear(), regID, unitID, firNum);
					}
					break;
				}//end switch 2
				//tFires->Requery();
				//	if(tFires->IsEOF() && tFires->IsBOF())
				//{
				tFires->AddNew();
				//tFires->m_FireID = ++(*fireID);
				//}
				//	else
				//	{
				//		append = false;
				//		tFires->Edit();
				//	}
				//str.Format("%4.4s", temp);
				tFires->m_FireNumber = firNum;
				tFires->m_AgencyID = agencyID;
				tFires->m_RegionID = regID;
				tFires->m_UnitID = unitID;
				tFires->SetFieldNull(&tFires->m_SubunitID);
				//CTime tTime(
				//COleDateTime tTime(y, m, d, hr, mn, 0);
				//tFires->m_Discovery = tTime;
				tFires->m_Discovery = dt;
				//str.Format("%d", dt.GetYear());
				//tFires->m_Year = str;
				double dd = 0.0, mm = 0.0, ss = 0.0;
				strncpy(temp, &buf[26], 2);
				temp[2] = 0;
				if (strlen(trim(temp)) > 0)
				{
					dd = atof(temp);
					strncpy(temp, &buf[28], 2);
					temp[2] = 0;
					if (strlen(trim(temp)) > 0)
						mm = atof(temp);
					strncpy(temp, &buf[30], 2);
					temp[2] = 0;
					if (strlen(trim(temp)) > 0)
						ss = atof(temp);
					tFires->m_latitude = dd + mm / 60.0 + ss / 3600.0;
				}
				else
					tFires->SetFieldNull(&tFires->m_latitude);
				/*strncpy(temp, &buf[28], 2);
				temp[2] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_LatMM = atol(temp);
				else
					tFires->SetFieldNull(&tFires->m_LatMM);
				strncpy(temp, &buf[30], 2);
				temp[2] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_LatSS = atol(temp);
				else
					tFires->SetFieldNull(&tFires->m_LatSS);*/
				dd = mm = ss = 0.0;
				strncpy(temp, &buf[32], 3);
				temp[3] = 0;
				if (strlen(trim(temp)) > 0)
				{
					dd = -atof(temp);
					strncpy(temp, &buf[35], 2);
					temp[2] = 0;
					if (strlen(trim(temp)) > 0)
						mm = atol(temp);
					strncpy(temp, &buf[37], 2);
					temp[2] = 0;
					if (strlen(trim(temp)) > 0)
						ss = atol(temp);
					tFires->m_longitude = dd - mm / 60.0 - ss / 3600.0;
				}
				else
					tFires->SetFieldNull(&tFires->m_longitude);
				/*strncpy(temp, &buf[35], 2);
				temp[2] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_LonMM = atol(temp);
				else
					tFires->SetFieldNull(&tFires->m_LonMM);
				strncpy(temp, &buf[37], 2);
				temp[2] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_LonSS = atol(temp);
				else
					tFires->SetFieldNull(&tFires->m_LonSS);*/
				/*   Causes - May want to confirm what's done here!!!! ******************************/
				strncpy(temp, &buf[95], 1);
				temp[1] = 0;
				if(strlen(trim(temp)) > 0)
				{
					int causeVal = atoi(temp);
					bool validCause = util.validateStatCause(causeVal);
					if (validCause)
						 tFires->m_StatisticalCause = atoi(temp);
					else
					{
						fprintf(errLog, "\tRecord Number %ld, invalid statistical cause: %s\n", fplLineNo, temp);
						return -1;
					 }
				} else
					tFires->m_StatisticalCause = 1;//assume lightning
				/*strncpy(temp, &buf[96], 2);
				temp[2] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_SpecificCause = atol(temp);
				else
					tFires->SetFieldNull(&tFires->m_SpecificCause);*/

				strncpy(temp, &buf[79], 8);
				temp[8] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_TotalAcres = atof(temp) / 10.0;
				else
					tFires->m_TotalAcres = 0;
				strncpy(temp, &buf[16], 10);
				temp[10] = 0;
				if(strlen(trim(temp)) > 0)
				{
					tFires->m_FireName = temp;
					tFires->m_FireName.Trim();
				}
				else
					tFires->SetFieldNull(&tFires->m_FireName);
				/*strncpy(temp, &buf[39], 4);
				temp[4] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_Township = temp;
				else
					tFires->SetFieldNull(&tFires->m_Township);
				strncpy(temp, &buf[43], 4);
				temp[4] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_Range = temp;
				else
					tFires->SetFieldNull(&tFires->m_Range);
				strncpy(temp, &buf[47], 2);
				temp[2] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_Section = atol(temp);
				else
					tFires->SetFieldNull(&tFires->m_Section);
				strncpy(temp, &buf[93], 2);
				temp[2] = 0;
				if(strlen(trim(temp)) > 0)
					tFires->m_State = temp;
				else
					tFires->SetFieldNull(&tFires->m_State);*/

				//NULL OUT REMAINING FIELDS IF NOT APPENDING
				/*if(!append)
				{
				tFires->SetFieldNull(&tFires->m_SubSection);
				tFires->SetFieldNull(&tFires->m_County);
				tFires->SetFieldNull(&tFires->m_Ignition);
				tFires->SetFieldNull(&tFires->m_FirstAttack);
				tFires->SetFieldNull(&tFires->m_Reinforcement);
				tFires->SetFieldNull(&tFires->m_DeclareWildlandFire);
				tFires->SetFieldNull(&tFires->m_Contain);
				tFires->SetFieldNull(&tFires->m_StrategyMet);
				tFires->SetFieldNull(&tFires->m_FireOut);
				tFires->SetFieldNull(&tFires->m_WildNum);
				tFires->SetFieldNull(&tFires->m_GeneralCause);
				tFires->SetFieldNull(&tFires->m_ClassPeople);
				tFires->SetFieldNull(&tFires->m_SizeClass);
				}*/
				tFires->Update();
			}
			break;
		}//end switch 1
		if(append)
			return 0;
		//update
		return 1;
	}

	int CImportDialog::WriteFireRecRAW(char *buf, long agencyID,
		FILE *errLog, CFiresSet *tFires)
	{
		bool append = true;
		char temp[16];
		int m, d, y, hr, min;
		long regID, unitID;
		CString str, sFireNum;
		COleDateTime dt;
		utils util(firesSet.m_pDatabase);

		if(agencyID == 1)//USFS
		{	//check if record already exists
			//agency == 1, year + reg + unit + firenum is unique
			strncpy(temp, buf, 2);
			temp[2] = 0;
			regID = GetFireRegion(&regs, agencyID, temp);
			if(regID == 0)
			{
				fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[2], temp);
				return -2;
			}
			strncpy(temp, &buf[143], 4);
			temp[4] = 0;
			y = atoi(temp);
			strncpy(temp, &buf[147], 2);
			temp[2] = 0;
			m = atoi(temp);
			strncpy(temp, &buf[149], 2);
			temp[2] = 0;
			d = atoi(temp);
			strncpy(temp, &buf[151], 2);
			temp[2] = 0;
			hr = atoi(temp);
			strncpy(temp, &buf[153], 2);
			temp[2] = 0;
			min = atoi(temp);
			if(y > 1900 && m > 0 && m < 13 && d > 0 && d < 32)
			{
				dt.SetDateTime(y, m, d, hr, min, 0);
				if(dt.GetStatus() != 0 || y < 1900)
				{
					fprintf(errLog, "\tRecord Number %ld, %s: %d/%d/%04d\n", fplLineNo, errStrings[1], m, d, y);
					return -1;
				}
			}
			else
			{
				fprintf(errLog, "\tRecord Number %ld, %s: %d/%d/%04d\n", fplLineNo, errStrings[1], m, d, y);
				return -1;
			}

			COleDateTime today;
			today = COleDateTime::GetCurrentTime();
			if(dt > today)
			{
				fprintf(errLog, "\tRecord Number %ld, %s: %d/%d/%04d\n", fplLineNo, errStrings[1], m, d, y);
				return -1;
			}
			strncpy(temp, &buf[2], 2);
			temp[2] = 0;
			unitID = GetFireUnit(&units, agencyID, regID, temp);
			if(unitID == 0)
			{
				unitID = AddFireUnit(&units, agencyID, regID, temp);
				fprintf(errLog, "\tRecord Number %ld, Added New Unit: '%2s Unknown'\n", fplLineNo, temp);
				nNewUnits++;
				//fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[3], temp);
				//return -3;
			}
			strncpy(temp, &buf[4], 3);
			temp[3] = 0;
			sFireNum.Format("%3.3s", temp);
			//tFires->m_strFilter.Format("[Year] = '%d' and [AgencyID] = 1 and [RegionID] = %ld and [UnitID] = %ld and [FireNumber] = '%s'",
			//	dt.GetYear(), regID, unitID, temp);
			//tFires->Requery();
			//if(tFires->IsEOF() && tFires->IsBOF())
			//{
			tFires->AddNew();
			//tFires->m_FireID = ++(*fireID);
			/*}
			else
			{
			append = false;
			tFires->Edit();
			}*/
			tFires->m_AgencyID = agencyID;
			tFires->m_RegionID = regID;
			tFires->m_UnitID = unitID;
			//CTime tTime(y, m, d, hr, min, 0);
			tFires->m_Discovery = dt;
			//str.Format("%d", dt.GetYear());
			//tFires->m_Year = str;
			strncpy(temp, &buf[7], 2);
			temp[2] = 0;
			long suID = 0;
			if(strlen(trim(temp)) > 0)
			{
				CString junk = temp;
				junk.Trim();
				if(junk.GetLength() < 2)
				{
					temp[0] = '0';
					temp[1] = junk[0];
					temp[2] = 0;
				}

				suID = GetFireSubunit(&subs, agencyID, unitID, temp);
				if(suID == 0)
				{
					suID = AddFireSubUnit(&subs, unitID, temp);
					fprintf(errLog, "\tRecord Number %ld, Added New SubUnit: '%2s Unknown'\n", fplLineNo, temp);
					nNewSubunits++;
					//return -4;
				}
				tFires->m_SubunitID = suID;
			}
			else
				tFires->SetFieldNull(&tFires->m_SubunitID);
			tFires->m_FireNumber.Format("%d%02d%02d-%ld-%ld-%ld-%ld-%s", dt.GetYear(), dt.GetMonth(), dt.GetDay(),agencyID, regID, unitID, suID, sFireNum);// = str;
			strncpy(temp, &buf[9], 1);
			temp[1] = 0;
			if(strlen(trim(temp)) > 0)
			{
				int causeVal = atoi(temp);
				bool validCause = util.validateStatCause(causeVal);
				if (validCause)
						 tFires->m_StatisticalCause = atoi(temp);
					else
					{
						fprintf(errLog, "\tRecord Number %ld, invalid statistical cause: %s\n", fplLineNo, temp);
						return -1;
					 }
			} else
				tFires->SetFieldNull(&tFires->m_StatisticalCause);
			/*strncpy(temp, &buf[10], 1);
			temp[1] = 0;
			if(strlen(trim(temp)) > 0)
				tFires->m_GeneralCause = atol(temp);
			else
				tFires->SetFieldNull(&tFires->m_GeneralCause);
			strncpy(temp, &buf[11], 2);
			temp[2] = 0;
			if(strlen(trim(temp)) > 0)
				tFires->m_SpecificCause = atol(temp);
			else
				tFires->SetFieldNull(&tFires->m_SpecificCause);
			strncpy(temp, &buf[13], 1);
			temp[1] = 0;
			if(strlen(trim(temp)) > 0)
				tFires->m_ClassPeople = atol(temp);
			else
				tFires->SetFieldNull(&tFires->m_ClassPeople);
			str.Format("%1.1s", &buf[14]);
			str.TrimRight();
			if(str.GetLength() > 0)
				tFires->m_SizeClass = str;
			else
				tFires->SetFieldNull(&tFires->m_SizeClass);*/
			strncpy(temp, &buf[15], 9);
			temp[9] = 0;
			if(strlen(trim(temp)) > 0)
				tFires->m_TotalAcres = atof(temp);
			else
				tFires->m_TotalAcres = 0;
			double dd = 0.0, mm = 0.0, ss = 0.0;
			strncpy(temp, &buf[70], 2);
			temp[2] = 0;
			if (strlen(trim(temp)) > 0)
			{
				dd = atof(temp);
				strncpy(temp, &buf[72], 2);
				temp[2] = 0;
				if (strlen(trim(temp)) > 0)
					mm = atof(temp);
				strncpy(temp, &buf[74], 2);
				temp[2] = 0;
				if (strlen(trim(temp)) > 0)
					ss = atof(temp);
				tFires->m_latitude = dd + mm / 60.0 + ss / 3600.0;
			}
			else
				tFires->SetFieldNull(&tFires->m_latitude);
			dd = mm = ss = 0.0;
			/*strncpy(temp, &buf[72], 2);
			temp[2] = 0;
			if (strlen(trim(temp)) > 0)
			{
				tFires->m_LatMM = atol(temp);
			}
			else
				tFires->SetFieldNull(&tFires->m_LatMM);
			strncpy(temp, &buf[74], 2);
			temp[2] = 0;
			if(strlen(trim(temp)) > 0)
				tFires->m_LatSS = atol(temp);
			else
				tFires->SetFieldNull(&tFires->m_LatSS);*/
			strncpy(temp, &buf[76], 3);
			temp[3] = 0;
			if (strlen(trim(temp)) > 0)
			{
				dd = -atof(temp);
				strncpy(temp, &buf[79], 2);
				temp[2] = 0;
				if (strlen(trim(temp)) > 0)
					mm = atof(temp);
				strncpy(temp, &buf[81], 2);
				temp[2] = 0;
				if (strlen(trim(temp)) > 0)
					ss = atof(temp);
				tFires->m_longitude = dd - mm / 60.0 - ss / 3600.0;
			}
			else
				tFires->SetFieldNull(&tFires->m_longitude);
			/*strncpy(temp, &buf[79], 2);
			temp[2] = 0;
			if(strlen(trim(temp)) > 0)
				tFires->m_LonMM = atol(temp);
			else
				tFires->SetFieldNull(&tFires->m_LonMM);
			strncpy(temp, &buf[81], 2);
			temp[2] = 0;
			if(strlen(trim(temp)) > 0)
				tFires->m_LonSS = atol(temp);
			else
				tFires->SetFieldNull(&tFires->m_LonSS);
			str.Format("%5.5s", &buf[83]);
			str.TrimRight();
			if(str.GetLength() > 0)
				tFires->m_Township = str;
			else
				tFires->SetFieldNull(&tFires->m_Township);
			str.Format("%5.5s", &buf[88]);
			str.TrimRight();
			if(str.GetLength() > 0)
				tFires->m_Range = str;
			else
				tFires->SetFieldNull(&tFires->m_Range);
			strncpy(temp, &buf[93], 2);
			temp[2] = 0;
			if(strlen(trim(temp)) > 0)
				tFires->m_Section = atol(temp);
			else
				tFires->SetFieldNull(&tFires->m_Section);
			str.Format("%4.4s", &buf[95]);
			str.TrimRight();
			if(str.GetLength() > 0)
				tFires->m_SubSection = str;
			else
				tFires->SetFieldNull(&tFires->m_SubSection);
			str.Format("%2.2s", &buf[110]);
			str.TrimRight();
			if(str.GetLength() > 0)
				tFires->m_State = str;
			else
				tFires->SetFieldNull(&tFires->m_State);
			strncpy(temp, &buf[112], 3);
			temp[3] = 0;
			if(strlen(trim(temp)) > 0)
				tFires->m_County = atol(temp);
			else
				tFires->SetFieldNull(&tFires->m_County);*/
			//dates....
			/*strncpy(temp, &buf[131], 4);
			temp[4] = 0;
			y = atoi(temp);
			strncpy(temp, &buf[135], 2);
			temp[2] = 0;
			m = atoi(temp);
			strncpy(temp, &buf[137], 2);
			temp[2] = 0;
			d = atoi(temp);
			strncpy(temp, &buf[139], 2);
			temp[2] = 0;
			hr = atoi(temp);
			strncpy(temp, &buf[141], 2);
			temp[2] = 0;
			min = atoi(temp);
			if(y > 1900 && m > 0 && m < 13 && d > 0 && d < 32)
			{
				dt.SetDateTime(y, m, d, hr, min, 0);
				//CTime tTime(y, m, d, hr, min, 0);
				tFires->m_Ignition = dt;
			}
			else
				tFires->SetFieldNull(&tFires->m_Ignition);
			strncpy(temp, &buf[155], 4);
			temp[4] = 0;
			y = atoi(temp);
			strncpy(temp, &buf[159], 2);
			temp[2] = 0;
			m = atoi(temp);
			strncpy(temp, &buf[161], 2);
			temp[2] = 0;
			d = atoi(temp);
			strncpy(temp, &buf[163], 2);
			temp[2] = 0;
			hr = atoi(temp);
			strncpy(temp, &buf[165], 2);
			temp[2] = 0;
			min = atoi(temp);
			if(y > 1900 && m > 0 && m < 13 && d > 0 && d < 32)
			{
				dt.SetDateTime(y, m, d, hr, min, 0);
				//CTime tTime(y, m, d, hr, min, 0);
				tFires->m_FirstAttack = dt;
			}
			else
				tFires->SetFieldNull(&tFires->m_FirstAttack);
			strncpy(temp, &buf[167], 4);
			temp[4] = 0;
			y = atoi(temp);
			strncpy(temp, &buf[171], 2);
			temp[2] = 0;
			m = atoi(temp);
			strncpy(temp, &buf[173], 2);
			temp[2] = 0;
			d = atoi(temp);
			strncpy(temp, &buf[175], 2);
			temp[2] = 0;
			hr = atoi(temp);
			strncpy(temp, &buf[177], 2);
			temp[2] = 0;
			min = atoi(temp);
			if(y > 1900 && m > 0 && m < 13 && d > 0 && d < 32)
			{
				dt.SetDateTime(y, m, d, hr, min, 0);
				tFires->m_Reinforcement = dt;
			}
			else
				tFires->SetFieldNull(&tFires->m_Reinforcement);
			strncpy(temp, &buf[179], 4);
			temp[4] = 0;
			y = atoi(temp);
			strncpy(temp, &buf[183], 2);
			temp[2] = 0;
			m = atoi(temp);
			strncpy(temp, &buf[185], 2);
			temp[2] = 0;
			d = atoi(temp);
			strncpy(temp, &buf[187], 2);
			temp[2] = 0;
			hr = atoi(temp);
			strncpy(temp, &buf[189], 2);
			temp[2] = 0;
			min = atoi(temp);
			if(y > 1900 && m > 0 && m < 13 && d > 0 && d < 32)
			{
				dt.SetDateTime(y, m, d, hr, min, 0);
				//CTime tTime(y, m, d, hr, min, 0);
				tFires->m_DeclareWildlandFire = dt;
			}
			else
				tFires->SetFieldNull(&tFires->m_DeclareWildlandFire);*/
			strncpy(temp, &buf[191], 4);
			temp[4] = 0;
			y = atoi(temp);
			strncpy(temp, &buf[195], 2);
			temp[2] = 0;
			m = atoi(temp);
			strncpy(temp, &buf[197], 2);
			temp[2] = 0;
			d = atoi(temp);
			strncpy(temp, &buf[199], 2);
			temp[2] = 0;
			hr = atoi(temp);
			strncpy(temp, &buf[201], 2);
			temp[2] = 0;
			min = atoi(temp);
			if(y > 1900 && m > 0 && m < 13 && d > 0 && d < 32)
			{
				dt.SetDateTime(y, m, d, hr, min, 0);
				//CTime tTime(y, m, d, hr, min, 0);
				tFires->m_Contain = dt;
			}
			else
				tFires->SetFieldNull(&tFires->m_Contain);
			strncpy(temp, &buf[203], 4);
			temp[4] = 0;
			y = atoi(temp);
			strncpy(temp, &buf[207], 2);
			temp[2] = 0;
			m = atoi(temp);
			strncpy(temp, &buf[209], 2);
			temp[2] = 0;
			d = atoi(temp);
			strncpy(temp, &buf[211], 2);
			temp[2] = 0;
			hr = atoi(temp);
			strncpy(temp, &buf[213], 2);
			temp[2] = 0;
			min = atoi(temp);
			if(y > 1900 && m > 0 && m < 13 && d > 0 && d < 32)
			{
				dt.SetDateTime(y, m, d, hr, min, 0);
				//CTime tTime(y, m, d, hr, min, 0);
				tFires->m_StrategyMet = dt;
			}
			else
				tFires->SetFieldNull(&tFires->m_StrategyMet);
			/*strncpy(temp, &buf[215], 4);
			temp[4] = 0;
			y = atoi(temp);
			strncpy(temp, &buf[219], 2);
			temp[2] = 0;
			m = atoi(temp);
			strncpy(temp, &buf[221], 2);
			temp[2] = 0;
			d = atoi(temp);
			strncpy(temp, &buf[223], 2);
			temp[2] = 0;
			hr = atoi(temp);
			strncpy(temp, &buf[225], 2);
			temp[2] = 0;
			min = atoi(temp);
			if(y > 1900 && m > 0 && m < 13 && d > 0 && d < 32)
			{
				dt.SetDateTime(y, m, d, hr, min, 0);
				//CTime tTime(y, m, d, hr, min, 0);
				tFires->m_FireOut = dt;
			}
			else
				tFires->SetFieldNull(&tFires->m_FireOut);*/
			str.Format("%20.20s", &buf[227]);
			str.Trim();
			if(str.GetLength() > 0)
				tFires->m_FireName = str;
			else
				tFires->SetFieldNull(&tFires->m_FireName);
			/*strncpy(temp, &buf[259], 3);
			temp[3] = 0;
			if(strlen(trim(temp)) > 0)
				tFires->m_WildNum = atol(temp);
			else
				tFires->SetFieldNull(&tFires->m_WildNum);*/

			tFires->Update();
		}
		if(append)
			return 0;

		//update
		return 1;
	}

	void CImportDialog::OnSelchangeAgency()
	{
		// TODO: Add your control notification handler code here
		int loc = agencies.GetCurSel();
		if(loc != CB_ERR)
		{
			long agency = agencies.GetItemData(loc);
			if (agency == m_GACC_Agency_ID)
			{
				rawButton.EnableWindow(FALSE);
				m_genericFireButton.EnableWindow(FALSE);
				m_WFMIbutton.EnableWindow(FALSE);
				m_nasfFireButton.EnableWindow(FALSE);
				m_btnINFORMS.EnableWindow(TRUE);
			}
			else
			{
				switch (agency)
				{
				case 1://USFS
					rawButton.EnableWindow(TRUE);
					m_genericFireButton.EnableWindow(FALSE);
					m_WFMIbutton.EnableWindow(FALSE);
					m_nasfFireButton.EnableWindow(FALSE);
					m_btnINFORMS.EnableWindow(FALSE);
					break;
				case 2:
				case 3:
				case 4:
				case 5://FWS
					rawButton.EnableWindow(FALSE);
					m_genericFireButton.EnableWindow(FALSE);
					m_WFMIbutton.EnableWindow(TRUE);
					m_nasfFireButton.EnableWindow(FALSE);
					m_btnINFORMS.EnableWindow(FALSE);
					break;
				case 6: // NASF national association of state foresters   - added for 2014 v4.2
					rawButton.EnableWindow(FALSE);
					m_WFMIbutton.EnableWindow(FALSE);
					m_genericFireButton.EnableWindow(FALSE);
					m_nasfFireButton.EnableWindow(TRUE);
					m_btnINFORMS.EnableWindow(FALSE);
					break;
				default:
					rawButton.EnableWindow(FALSE);
					m_WFMIbutton.EnableWindow(FALSE);
					m_genericFireButton.EnableWindow(TRUE);
					m_nasfFireButton.EnableWindow(FALSE);
				}
			}
		}
		else
		{
			rawButton.EnableWindow(FALSE);
			m_genericFireButton.EnableWindow(FALSE);
			m_WFMIbutton.EnableWindow(FALSE);
			m_nasfFireButton.EnableWindow(FALSE);
		}
	}

	/*int CImportDialog::ImportCustom(long agencyID)
	{
	chdir(theApp.userDir);
	CFileDialog	dlg (TRUE, _T("fpl"), NULL, OFN_HIDEREADONLY,
	_T("Fire Record Files (*.fpl)|*.fpl|All Files (*.*)|*.*||"), this);
	if(dlg.DoModal() == IDOK)
	{
	CCuctomFireDialog cfd(this, regs.m_pDatabase, agencyID, dlg.GetPathName());
	cfd.DoModal();
	}
	chdir(theApp.workDir);
	return 0;
	}*/

	void CImportDialog::OnStations()
	{
		chdir(theApp.userDir);
		CFileDialog	dlg (TRUE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
			OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
			_T("Station Catalog Files (*.txt;*.csv;*.htm;*.html)|*.txt;*.csv;*.htm;*.html|All Files (*.*)|*.*||"), this);
		char *mfBuf = new char[MF_BUFSIZE];
		mfBuf[0] = 0;
		dlg.m_ofn.lpstrFile = mfBuf;
		dlg.m_ofn.nMaxFile = MF_BUFSIZE;
		int res = dlg.DoModal();
		if (res == IDOK)
		{
			CWaitCursor wait;
			long errors = 0;
			// Get the position of the first filename
			char *logFileName = GetTempFileName();
			FILE * logFile = fopen(logFileName, "wt");
			COleDateTime dateTime = COleDateTime::GetCurrentTime();
			fprintf(logFile, "FireFamily Plus Station Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			POSITION filenamePosition = dlg.GetStartPosition();
			CString fileName;

			// Open each file selected
			while ( filenamePosition != NULL )
			{
				fileName = dlg.GetNextPathName( filenamePosition );
				errors += ImportStationFile(fileName, logFile);
			}
			dateTime = COleDateTime::GetCurrentTime();
			fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
			fclose(logFile);
			CString strMsg;
			strMsg.Format("Import Complete.\n%ld errors were encountered.\n"
				"Be sure to check for and enter missing values for imported stations.\n"
				"Especially Greenup and Freeze dates! (The Grippster, 1999)\n", errors);
			CImportDoneDialog idd(this);
			idd.statusStr = strMsg;
			idd.logFileName = logFileName;
			free(logFileName);
			idd.DoModal();
			//CDialog::OnOK();
			theApp.SetUserDir(dlg.GetPathName());
		}
		else if(res == IDCANCEL)
		{
			int err = CommDlgExtendedError();
			if(err == FNERR_BUFFERTOOSMALL)
			{
				AfxMessageBox("File Name buffer overrun, select fewer files.");
			}
		}
		delete[] mfBuf;
		chdir(theApp.dbDir);
	}

	int CImportDialog::ImportStationFile(CString fileName, FILE *logFile)
	{
		char buf[270];
		int errors = 0;
		int nStations = 0;
		fprintf(logFile, "\n***************************************************\nImporting %s\n", fileName);
		FILE *stream = fopen(fileName, "rb");
		long catStart = -1;
		char lastSta[256] = "";
		char currSta[256] = "";
		while(!feof(stream))// && catStart < 0)
		{
			if(fgets(buf, 262, stream) != NULL)
			{
				buf[staHeader.GetLength()] = 0;
				if(staHeader.Compare(buf) == 0)
				{
					buf[staHeader.GetLength()] = '-';
					strcpy_s(currSta, buf);
					if(strcmp(currSta, lastSta) != 0)
					{
						catStart = ftell(stream);
						errors += ImportStation(stream, logFile);
						strcpy_s(lastSta, currSta);
						nStations++;
					}
				}
			}
		}
		fclose(stream);
		fprintf(logFile, "\n***************************************************\nImported %d stations from %s\n",
			nStations, fileName);
		if(catStart < 0)
		{
			fprintf(logFile, "\tError: Could not find catalog header in file:\n\t"
				"%s\n\tFile does not appear to be a station catalog.\n\n",
				fileName);
			return 1;
		}
		return errors;
	}

	int CImportDialog::ImportStation(FILE *stream, FILE *logFile)
	{//imports a station, returning number of errors written to log file
		//also writes summaries to log file
		long catStart = ftell(stream);
		char buf[270];
		int errors = 0;
		//now look for the fields for the station
		bool found = false, append = false;
		while(!feof(stream))
		{
			if(fgets(buf, 265, stream) == NULL)
				break;
			if(strncmp("Station:", buf, 8) == 0)
			{//station line found!
				found = true;
				break;
			}
		}
		if(!found)
		{
			fprintf(logFile, "\tError: Could not find StationID line\n\t"
				"%s\n\tFile does not appear to be a station catalog.\n");
			//fclose(stream);
			return 1;
		}
		char temp[256];
		strncpy(temp, &buf[9], 6);
		temp[6] = 0;
		bool okSta = true;
		for(int i = 0; i < 6; i++)
		{
			if(!isalnum(temp[i]) )
				okSta = false;
		}
		if(!okSta)
		{
			fprintf(logFile, "\tError: \tStationID '%s' is invalid.\n",
				temp);
			//fclose(stream);
			return 1;
		}
		//set up destination table
		CSIGStationSet staSet(regs.m_pDatabase);
		staSet.m_strFilter.Format("[StationID] = '%s'", temp);
		staSet.Open();
		if(staSet.IsBOF() || staSet.IsEOF())
		{
			append = true;
			staSet.AddNew();
		}
		else
			staSet.Edit();
		staSet.m_StationID = temp;
		strncpy(temp, &buf[24], 20);
		temp[20] = 0;
		staSet.m_Name = temp;
		if(strlen(buf) > 56)//look for NESDIS_ID
		{
			char *p = strstr(buf, "NESDIS: ");
			if(p)
			{
				p += 8;
				strncpy(temp, p, 8);
				temp[8] = 0;
				if(strcmp(temp, "________") != 0 && strcmp(temp, "        ") != 0)
				{
					staSet.m_NESDIS_ID = temp;
				}
				else
				{
					errors++;
					fprintf(logFile, "\tError: Blank NESDIS ID in line:\n\t%s\n", buf);
				}
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Could not find NESDIS ID in line:\n\t%s\n", buf);
			}
			//strncpy(temp, &buf[55], 8);
			//temp[8] = 0;
			//staSet.m_NESDIS_ID = temp;
		}
		//long errCount = 0;
		found = false;
		fseek(stream, catStart, SEEK_SET);
		char *ptr;
		while(!found && !feof(stream))
		{
			if(fgets(buf, 265, stream) == NULL)
				break;
			if(strncmp("Type:", buf, 5) == 0)
				found = true;
		}
		if(found)
		{
			long val = atol(&buf[6]);
			if(val > 0 && val < 9)
				staSet.m_StnType = val;
			else
			{
				errors++;
				fprintf(logFile, "\tError: Invalid Station Type in line:\n\t%s\n", buf);
			}
			char *p = strstr(buf, "Obs Time/Z: ");
			if(p)
			{
				p += 12;
				strncpy(temp, p, 2);
				temp[2] = 0;
				int tRSO = atoi(temp);
				if(tRSO >= 12 && tRSO <= 14)
				{
					staSet.m_RegSchdObs = tRSO;
				}
				else
				{
					errors++;
					fprintf(logFile, "\tError: Invalid Obs Time in line:\n\t%s\n", buf);
				}
				p = strstr(p, "/");
				if (p)
				{
					p++;
					strncpy(temp, p, 4);
					temp[4] = 0;
					staSet.m_timeZoneOffset = theApp.m_timeZones.GetTimeZone(temp).m_offset;
				}
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Could not find Obs Time in line:\n\t%s\n", buf);
			}
		}
		else
		{
			errors++;
			fprintf(logFile, "\tError: Could not find station type line.\n");
		}
		found = false;
		fseek(stream, catStart, SEEK_SET);
		while(!found && !feof(stream))
		{
			if(fgets(buf, 265, stream) == NULL)
				break;
			if(strncmp("State:", buf, 6) == 0)
				found = true;
		}
		if(found)
		{
			long val = atol(&buf[7]);
			CStateSet stateSet(regs.m_pDatabase);
			stateSet.m_strFilter.Format("[StateCode] = %ld", val);
			stateSet.Open();
			if(!stateSet.IsBOF() && !stateSet.IsEOF())
			{
				staSet.m_State = stateSet.m_State;
				ptr = strstr(buf, "County:");
				if(ptr && strlen(ptr) > 7)
				{
					long cval = atol(&ptr[7]);
					CCountySet countySet(regs.m_pDatabase);
					countySet.m_strFilter.Format("[StateCode] = %ld and [CountyID] = %ld", val, cval);
					countySet.Open();
					if(!countySet.IsBOF() && !countySet.IsEOF())
						staSet.m_County = cval;
					else
					{
						errors++;
						fprintf(logFile, "\tError: Invalid County code: %ld\n", cval);
					}
					countySet.Close();
				}
				else
				{
					errors++;
					fprintf(logFile, "\tError: Missing County code in line:\n\t%s\n", buf);
				}
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Invalid State code: %ld\n", val);
			}
			stateSet.Close();
			ptr = strstr(buf, "Lat/Lon:");
			if(ptr && strlen(ptr) > 25)
			{
				char crap[64];
				double dval;// = (double) atof(&ptr[8]);
				int dlat, mlat, slat, dlon, mlon, slon;
				sscanf(ptr, "%s %d %d %d, %d %d %d", crap, &dlat, &mlat, &slat, &dlon, &mlon, &slon);
				DDMMSS2DecimalDegree(dlat, mlat, slat, &dval);
				if(dval >= -90.0 && dval <= 90.0)
					staSet.m_LatDegrees = dval;
				else
				{
					errors++;
					fprintf(logFile, "\tError: Invalid Latitude degrees: %f\n", dval);
				}
				DDMMSS2DecimalDegree(dlon, mlon, slon, &dval);
				if(dval > 0)//probably got it from an old station catalog
					dval = -dval;
				//ptr = strstr(ptr, ",");
				//if(ptr && strlen(ptr) > 3)
				//{
				//dval = atof(&ptr[1]);
				if(dval >= -180 && dval <= 180)
					staSet.m_LonDegrees = dval;
				else
				{
					errors++;
					fprintf(logFile, "\tError: Invalid Longitude degrees: %d\n", dval);
				}
				//}
				//else
				//{
				//errors++;
				//fprintf(logFile, "\tError: Missing longitude in line:\n\t%s\n", buf);
				//}
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Missing Lat/Lon fields in line:\n\t%s\n", buf);
			}
		}
		else
		{
			errors++;
			fprintf(logFile, "\tError: Could not find State, County, Lat/Lon line.\n");
		}
		found = false;
		fseek(stream, catStart, SEEK_SET);
		while(!found && !feof(stream))
		{
			if(fgets(buf, 265, stream) == NULL)
				break;
			if(strncmp("Obs Agy:", buf, 8) == 0)
				found = true;
		}
		if(found)
		{
			long val = atol(&buf[9]);
			if(val > 0 && val < 10)
				staSet.m_ObsAgy = val;
			else
			{
				errors++;
				fprintf(logFile, "\tError: Invalid Agency ID in line:\n\t%s\n", buf);
			}
			ptr = strstr(buf, "Unit:");
			if(ptr && strlen(ptr) > 6)
			{
				strncpy(temp, &ptr[6], 12);
				temp[12] = 0;
				staSet.m_ObsUnit = temp;
			}
			else
			{
				errors++;
				fprintf(logFile, "/tError: Missing Unit field in line:\n\t%s\n", buf);
			}
			ptr = strstr(buf, "FS Reg:");
			if(ptr && strlen(ptr) > 8)
			{
				val = atol(&ptr[8]);
				if(val > 0 && val < 10)
					staSet.m_FSRegion = val;
				else
				{
					errors++;
					fprintf(logFile, "\tError: Invalid FS Region in line:\n\t%s\n", buf);
				}
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: FS Region field in line:\n\t%s\n", buf);
			}
		}
		else
		{
			errors++;
			fprintf(logFile, "\tError: Could not find Observation Agency, Unit and Region line.\n");
		}
		found = false;
		fseek(stream, catStart, SEEK_SET);
		while(!found && !feof(stream))
		{
			if(fgets(buf, 265, stream) == NULL)
				break;
			if(strncmp("Site:", buf, 5) == 0)
				found = true;
		}
		if(found)
		{
			strncpy(temp, &buf[6], 1);
			temp[1] = 0;
			if(temp[0] >= '1' && temp[0] <= '3')
			{
				switch(temp[0])
				{
				case '1':
					staSet.m_PsnOnSlope = "L";
					break;
				case '2':
					staSet.m_PsnOnSlope = "M";
					break;
				case '3':
					staSet.m_PsnOnSlope = "U";
					break;
				}
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Invalid Site value: %s\n", temp);
			}
			ptr = strstr(buf, "Elev:");
			if(ptr && strlen(ptr) > 5)
			{
				int ival = atoi(&ptr[5]);
				staSet.m_Elevation = ival;
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Missing Elevation field in line:\n\t%s\n", buf);
			}
			ptr = strstr(buf, "Asp:");
			if(ptr && strlen(ptr) > 4)
			{
				strncpy(temp, &ptr[5], 1);
				temp[1] = 0;
				if(isdigit(temp[0]))
					staSet.m_Aspect = temp;
				else
				{
					errors++;
					fprintf(logFile, "\tError: Invalid Aspect code: %s\n", temp);
				}
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Missing Aspect field in line:\n\t%s\n", buf);
			}
			ptr = strstr(buf, "Ann Prec:");
			if(ptr && strlen(ptr) > 9)
			{
				double rval = atof(&ptr[9]);
				staSet.m_AvgPrecip = (float)rval;
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Missing Annual Precip field in line:\n\t%s\n", buf);
			}
		}
		else
		{
			errors++;
			fprintf(logFile, "\tError: Could not find Site, Elevation, Aspect, and Annual Precip line.\n");
		}
		found = false;
		fseek(stream, catStart, SEEK_SET);
		while(!found && !feof(stream))
		{
			if(fgets(buf, 265, stream) == NULL)
				break;
			if(strncmp("   Wind Spd code:", buf, 17) == 0)
				found = true;
		}
		if(found)
		{
			ptr = strstr(buf, "KBDI:");
			if(ptr && strlen(ptr) > 5)
			{
				int ival = atoi(&ptr[5]);
				staSet.m_StartKBDI = ival;
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Missing Start KBDI field in line:\n\t%s\n", buf);
			}
			ptr = strstr(buf, "One/Ten Fl:");
			if(ptr && strlen(ptr) > 12)
			{
				char c = ptr[12];
				staSet.m_FM1Eq10 = (c == 'Y') ? 1 : 0;
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Missing FM1 equals FM10 field in line:\n\t%s\n", buf);
			}
		}
		else
		{
			errors++;
			fprintf(logFile, "\tError: Could not find Start KBDI and One/Ten line.\n");
		}
		//fuel model line
		found = false;
		fseek(stream, catStart, SEEK_SET);
		while(!found && !feof(stream))
		{
			if(fgets(buf, 265, stream) == NULL)
				break;
			if(strncmp("   - -- - --------- --------- - - - -  -- -- --- --- --- ---", buf, 60) == 0)
				found = true;
		}
		if(found)
		{
			//read the first fuel model line and decode it
			fgets(buf, 265, stream);
			if(atoi(buf) == 1)
			{
				bool isNFDRS2016 = false;
				char strFM[6];
				stripCRLF(buf);
				trim(buf);
				strcpy_s(temp, buf);
				char seps[] = " \t", *p;
				p = strtok(buf, seps);//this is the '1'
				p = strtok(NULL, seps);//this is the fuel model
				if (strlen(p) == 2)//old NFDRS
				{
					if (p[0] == '8')
						staSet.m_Use88 = TRUE;
					else
						staSet.m_Use88 = FALSE;
					strFM[0] = p[1];
					strFM[1] = 0;
				}
				else if (strlen(p) == 3)
				{
					isNFDRS2016 = true;
					staSet.m_Use88 = FALSE;
					strFM[0] = p[2];
					strFM[1] = 0;
				}
				else
				{
					staSet.m_Use88 = FALSE;
					strFM[0] = 'Y';
					strFM[1] = 0;
					errors++;
					fprintf(logFile, "\tError: Unable to determine fuel model, set to Y\n");
				}
				//temp[0] = buf[6];
				//temp[1] = 0;
				staSet.m_NFDRSFM = strFM;
				int pLoc;
				strcpy_s(temp, &buf[10]);
				char date1[16] = "";
				p = strtok(temp, seps);
				if(strlen(p) < 6)//no dates given.....
				{
					if (!isNFDRS2016)
					{
						errors++;
						fprintf(logFile, "\tError: No greenup date given.\n");
					}
					pLoc = 11;
				}
				else
				{
					strcpy_s(date1, p);
					p = strtok(NULL, seps);
				}
				if(strlen(p) >= 6) //2 dates given, use second one
				{
					strcpy_s(date1, p);
					p = strtok(NULL, seps);
				}
				if(strlen(date1) > 0 && !isNFDRS2016)//has valid greenup date
				{
					strncpy(temp, date1, 9);
					temp[9] = 0;
					int m, d;
					d = atoi(temp);
					CString mnth;
					mnth.Format("%3.3s", &temp[3]);
					m = MonthAbrevtoInt(mnth);
					if(m > 0)
					{
						COleDateTime gu(2000, m, d, 13, 0, 0);
						if(gu.GetStatus() == 0)
						{
							//CTime tTime(2000, m, d, 13, 0, 0);
							staSet.m_GreenJulian = gu;
						}
						else
						{
							errors++;
							fprintf(logFile, "\tError: Invalid greenup date: %s\n", temp);
						}
					}
					else
					{
						errors++;
						fprintf(logFile, "\tError: Invalid greenup date: %s\n", temp);
					}
				}
				//now get remaining fields
				if(*p == 'D')
					staSet.m_Deciduous = TRUE;
				else
					staSet.m_Deciduous = FALSE;
				p = strtok(NULL, seps);
				int val = atoi(p);
				if(val > 0 && val < 6)
					staSet.m_SlopeCls = val;
				else
				{
					errors++;
					fprintf(logFile, "\tError: Invalid Slope Class: %d\n", val);
				}
				p = strtok(NULL, seps);
				if(*p == 'P')
					staSet.m_HerbAnnual = FALSE;
				else
					staSet.m_HerbAnnual = TRUE;
				p = strtok(NULL, seps);
				val = atoi(p);
				if(val > 0 && val < 5)
				{
					staSet.m_ClimateCls = val;
					//calculate Start 1000
					staSet.m_Start1000 = (float)(10.0 + 5.0 * val);
				}
				else
				{
					if (!isNFDRS2016)
					{
						errors++;
						fprintf(logFile, "\tError: Invalid Climate Class: %d\n", val);
					}
				}
				//All done!
			}
			else
			{
				errors++;
				fprintf(logFile, "\tError: Fuel model line 1 appears invalid.\n");
			}
		}
		else
		{
			errors++;
			fprintf(logFile, "\tError: Could not find fuel model section.\n");
		}

		//need to set defaults for NFDRS2016 fuel model SCM and Humid
		theApp.m_NFDRS2016.iSetFuelModel('V');
		staSet.m_SCM_V = theApp.m_NFDRS2016.GetSCMax();
		staSet.m_MX_Humid_V = theApp.m_NFDRS2016.GetMxdHumid();
		theApp.m_NFDRS2016.iSetFuelModel('W');
		staSet.m_SCM_W = theApp.m_NFDRS2016.GetSCMax();
		staSet.m_MX_Humid_W = theApp.m_NFDRS2016.GetMxdHumid();
		theApp.m_NFDRS2016.iSetFuelModel('X');
		staSet.m_SCM_X = theApp.m_NFDRS2016.GetSCMax();
		staSet.m_MX_Humid_X = theApp.m_NFDRS2016.GetMxdHumid();
		theApp.m_NFDRS2016.iSetFuelModel('Y');
		staSet.m_SCM_Y = theApp.m_NFDRS2016.GetSCMax();
		staSet.m_MX_Humid_Y = theApp.m_NFDRS2016.GetMxdHumid();
		theApp.m_NFDRS2016.iSetFuelModel('Z');
		staSet.m_SCM_Z = theApp.m_NFDRS2016.GetSCMax();
		staSet.m_MX_Humid_Z = theApp.m_NFDRS2016.GetMxdHumid();


		fprintf(logFile, "\t%s station %s\n\t%d errors encountered.\n",
			//"Be sure to enter missing values for the station.\n\t"
			//"Especially Greenup and Freeze dates! (The Grippster, 1999)\n",
			append == true ? "Appended" : "Updated", staSet.m_StationID,
			errors);
		if(append)
		{
			COleDateTime lookF(1999, 10, 15, 14, 0, 0);
			//CTime tTime(1999, 10, 15, 14, 0, 0);
			staSet.m_FreezeJulian = lookF;
		}
		staSet.Update();
		staSet.Close();
		//fclose(stream);
		return errors;
	}

	int CImportDialog::NewImportFWX(CString fileName, FILE *errLog)
	{
		int errors = 0;
		int staProcessed = 0;
		int db;
		if (pCUPDUPData && pCUPDUPData->ShouldTerminate())
			return -1;
		char progText[256];
		//EnableWindow(FALSE);

//		CCancelDialog *cd = NULL;
		
/*		cd = new CCancelDialog();

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
				cd->SetTitle("Importing Weather Data");
			}
		}
		*/
		
		fprintf(errLog, "\n*****************************************************************\nImport File Name: %s\n", fileName);
		if(pDoc->HasTempWx())
		{
			try
			{
				CString strSql = "DROP TABLE [ffpTempWx]";
				regs.m_pDatabase->ExecuteSQL(strSql);
			}
			catch(CDBException* e)
			{
				e->Delete();
			}

			//pDoc->m_pDB->DeleteTableDef("ffpTempWx");
		}
		pDoc->CreateTempWx();
		CList<CString, CString> staList;
		CWxSet twxObs(regs.m_pDatabase);
		twxObs.m_strSort = "[ObsDate]";
		twxObs.Open(CRecordset::dynaset, "ffpTempWx");
		CSIGStationSet stnSet(regs.m_pDatabase);
		CSIGStationSet wrccStnSet(regs.m_pDatabase);
		wrccStnSet.Open();
		stnSet.Open();
		//regs.m_pDatabase->BeginTrans();
		FILE *stream = fopen(fileName, "rb");
		count = 0;
		long updates = 0, rejects = 0, lineNo = 0, raws = 0;
		if(stream)
		{
			char buf[161], temp[16];
			CString sta, oldStationID, seekStr;
			oldStationID = " ";
			int m, d, y;
			int recNo = 0;
			while(!feof(stream) && !pCUPDUPData->ShouldTerminate())//)// && (!cd || !cd->Abort()))
			{
				recNo++;
				sprintf(progText, "Reading record %d", recNo);
				pCUPDUPData->SetProgress(progText);

				fgets(buf, 160, stream);
				if(feof(stream))
					break;
				lineNo++;
				//set record members to values from file
				strncpy(temp, buf, 6);
				temp[6] = 0;
				CString sta = temp;
				sta.MakeLower();   // WRCC IDs are added in lower case
				sta.TrimRight();
				if(sta.GetLength() == 0)//stationID can not be blank!!!!!!
				{
					rejects++;
					errors++;
					fprintf(errLog, "Line number %ld, %s\n", lineNo, errStrings[0]);
					continue;
				}
				//never check for duplicates!!!!!
				strncpy(temp, &buf[6], 2);
				temp[2] = 0;
				if(!isdigit(temp[0]) || !isdigit(temp[1]))
				{
					rejects++;
					errors++;
					fprintf(errLog, "Line number %ld, %s: %s\n", lineNo, errStrings[1], temp);
					continue;
				}
				y = atoi(temp);
				if(y >= PIVOT)
					y += 1900;
				else
					y += 2000;
				strncpy(temp, &buf[8], 2);
				temp[2] = 0;
				m = atoi(temp);
				strncpy(temp, &buf[10], 2);
				temp[2] = 0;
				d = atoi(temp);
				COleDateTime y2kCheck;
				y2kCheck.SetDateTime(y, m, d, 13, 0, 0);
				if(y2kCheck.GetStatus() != 0 || y < 1900)
				{
					fprintf(errLog, "\tLine Number %ld, %s: %d/%d/%04d\n", lineNo, errStrings[1], m, d, y);
					rejects++;
					errors++;
					continue;
				}
				COleDateTime today;
				today = COleDateTime::GetCurrentTime();
				if(!theApp.m_enableFuture)
				{
					if(y2kCheck > today)
					{
						fprintf(errLog, "\tLine Number %ld, %s: %d/%d/%04d\n", lineNo, errStrings[1], m, d, y);
						rejects++;
						errors++;
						continue;
					}
				}
				twxObs.AddNew();
				if(staList.IsEmpty())
					staList.AddTail(sta);
				else
				{
					bool there = false;
					POSITION pos = staList.GetHeadPosition();
					while(pos)
					{
						if(staList.GetNext(pos) == sta)
						{
							there = true;
							break;
						}
					}
					if(!there)
						staList.AddHead(sta);
				}
				twxObs.m_StationID = sta;
				//CTime tTime(y, m, d, 13, 0, 0);
				twxObs.m_ObsDate.SetDateTime(y, m, d, 13, 0, 0);
				twxObs.m_Hour = 13;
				twxObs.m_ObsType = "O";
				strncpy(temp, &buf[12], 1);
				temp[1] = 0;

				if (temp[0] != ' ')
					twxObs.m_SOW = atoi(temp);
				else
					twxObs.SetFieldNull(&twxObs.m_SOW);
				strncpy(temp, &buf[13], 3);
				temp[3] = 0;
				if(strncmp(temp, "   ", 3) != 0)
				{
					db = atoi(temp);
					twxObs.m_Temp = db;
				}
				else
					db = -999;
				strncpy(temp, &buf[16], 3);
				temp[3] = 0;
				char t[2];
				t[0] = buf[60];
				t[1] = 0;
				if (strncmp(temp, "   ", 3) != 0)
				{
					twxObs.m_RH = RH(atoi(t), atoi(temp), db);
					twxObs.m_RH = max(twxObs.m_RH, 1);
				}
				//is this right????
				strncpy(temp, &buf[19], 3);
				temp[3] = 0;
				if(strncmp(temp, "   ", 3) != 0)
					twxObs.m_GreenHerb = atoi(temp);
				strncpy(temp, &buf[27], 1);
				temp[1] = 0;
				if(temp[0] != ' ')
					twxObs.m_WDir = atoi(temp);
				strncpy(temp, &buf[28], 3);
				temp[3] = 0;
				if(strncmp(temp, "   ", 3) != 0)
					twxObs.m_WS = atoi(temp);
				strncpy(temp, &buf[32], 3);
				temp[3] = 0;
				if(strncmp(temp, "   ", 3) != 0 && strncmp(temp, "000", 3) != 0)
					twxObs.m_OMC10 = (float)atof(temp);
				//is this right?
				strncpy(temp, &buf[35], 3);
				temp[3] = 0;
				if(strncmp(temp, "   ", 3) != 0)
					twxObs.m_GreenShrub = atoi(temp);
				strncpy(temp, &buf[38], 3);
				temp[3] = 0;
				if(strncmp(temp, "   ", 3) != 0 && strncmp(temp, "000", 3) != 0)
					twxObs.m_TmpMax = atoi(temp);
				strncpy(temp, &buf[41], 3);
				temp[3] = 0;
				if(strncmp(temp, "   ", 3) != 0 && strncmp(temp, "000", 3) != 0)
					twxObs.m_TmpMin = atoi(temp);
				strncpy(temp, &buf[44], 3);
				temp[3] = 0;
				if(strncmp(temp, "   ", 3) != 0 && strncmp(temp, "000", 3) != 0)
					twxObs.m_RHMax = atoi(temp);
				strncpy(temp, &buf[47], 3);
				temp[3] = 0;
				if(strncmp(temp, "   ", 3) != 0 && strncmp(temp, "000", 3) != 0)
					twxObs.m_RHMin = max(atoi(temp), 1);
				strncpy(temp, &buf[51], 2);
				temp[2] = 0;
				if(strncmp(temp, "  ", 2) != 0)
					twxObs.m_PPTDUR = atoi(temp);
				else
					twxObs.m_PPTDUR = 0;
				strncpy(temp, &buf[53], 4);
				temp[4] = 0;
				if(strchr(temp, 'T') || ((strcmp(temp, "    ") == 0 && twxObs.m_PPTDUR > 0)))
					twxObs.m_PPTAMT = (float)0.005;
				else if(strncmp(temp, "    ", 4) != 0)
				{
					if(strchr(temp, '.'))//it has a decimal point...
						twxObs.m_PPTAMT = (float)(atof(temp));
					else
						twxObs.m_PPTAMT = (float)(atof(temp) / 100.0);
				}
				strncpy(temp, &buf[50], 1);
				temp[1] = 0;
				if(temp[0] != ' ')
				{
					int seas = atoi(temp);
					if(seas >= 1 && seas <= 4)
						twxObs.m_Season = atoi(temp);
				}
				if(strlen(buf) > 61)//possible wet flag set
				{
					strncpy(temp, &buf[61], 1);
					temp[1] = 0;
					if(temp[0] == 'Y' || temp[0] == 'y')//wetflag column set
						twxObs.m_WetFlag = 1;
					else if(temp[0] == 'N' || temp[0] == 'n')//wetflag column set to false
						twxObs.m_WetFlag = 0;
					//else
					//twxObs.SetFieldNull(
				}
				if(strlen(buf) > 62)//possible SR_SOW set
				{
					strncpy(temp, &buf[62], 1);
					temp[1] = 0;
					if(temp[0] != ' ')
						twxObs.m_SR_SOW = atoi(temp);
					//else
					//twxObs.SetFieldNull(
				}
				if(strlen(buf) > 63)//possible SR_WetFlag set
				{
					strncpy(temp, &buf[63], 1);
					temp[1] = 0;
					if(temp[0] == 'Y' || temp[0] == 'y')//SR_WetFlag column set
						twxObs.m_SR_WetFlag = 1;
					else if(temp[0] == 'N' || temp[0] == 'n')//SR_WetFlag column set to false
						twxObs.m_SR_WetFlag = 0;
					//else
					//twxObs.SetFieldNull(
				}
				twxObs.Update();
				count++;
				//if (cd)
				//   cd->Increment();
			}
		}
		fclose(stream);
		CWxSet wxObs(regs.m_pDatabase);
		wxObs.m_strSort = "[ObsDate]";
		wxObs.Open();
		//to maintain referential integrity,
		//must add any new stations to wxSation table
		int staAdded = 0;
		int regHour = 13;
		long edits = 0, appends = 0;//, wetFlagEdits = 0;
		COleDateTime lastDay;
		CString strSQLa, sta;
		//if(!cd || !cd->Abort())
		//{
			//twxObs.m_pDatabase->CommitTrans(); //so we can requery
			/*if (cd)
			{
		   		cd->Reset();
				cd->SetStatusString("Updating Database");
			}*/
			POSITION pos = staList.GetHeadPosition();
			while(pos)// && (!cd || !cd->Abort()))
			{
				bool skipStation = false;
			
				sta = staList.GetNext(pos);
				
				//process for each station
				
				// new behavior 2013 FFP4.1
			// check for WRCC-WIMS station correlation BEFORE creating new station record
			CString wrccSta = sta;
			wrccSta.Trim();
			wrccSta.MakeLower();
			
			
			bool isWRCCstation = false;
			bool correlatedWRCC = false;
			bool existingStation = false;
			
			if (wrccSta.GetLength() < 6)
				isWRCCstation = true;

			// rebuild SQL string for wxObs
			CString strSQLb;
			strSQLb.Format("[StationID] = '%6.6s'", sta);


			if (isWRCCstation)
			{
			   // check to see if there's already a corresponding WIMS station
			   wrccStnSet.m_strFilter.Format("[WRCC_ID] = '%s'", wrccSta);
			   //strSQLb.Format("[StationID] = '%4.4s'", sta);
			   wrccStnSet.Requery();
			   if (!wrccStnSet.IsEOF())
				   if (wrccStnSet.m_StationID.Trim().GetLength() > 4)
				     correlatedWRCC = true;
			}  
				
			// is station in DB already?
		
			stnSet.m_strFilter.Format("[StationID] = '%6.6s'", sta);
			stnSet.Requery();
			if (!stnSet.IsEOF())
				  existingStation = true;
			
			wxObs.m_StationID = sta.Trim();

			CString stationLog;
			if (isWRCCstation)
			   stationLog.Format("Using WRCC station ID: %s\n",sta);
			else
			   stationLog.Format("Using WIMS station ID: %s\n",sta);
			
			// new case for correlated station: give user the option of importing
			// using either WIMS ID or WRCC ID
			if (correlatedWRCC)
			{
				stnSet.m_strFilter.Format("[WRCC_ID] = '%s'", wrccSta);
				stnSet.Requery();
				sta = stnSet.m_StationID;
				wxObs.m_StationID = sta.Trim();

				CString message;
				message.Format("WRCC stationID '%s' matches WIMS stationID '%s - %s'.\n\nDo you want to: ",wrccSta,sta,stnSet.m_Name.Trim());
				CString wimsStation;
				wimsStation.Format("Import data to '%s'",sta);
				CString wrccStation;
				wrccStation.Format("Import data to '%s'",wrccSta);
				CImportWRCCDialog iwd(NULL,  message, wimsStation, wrccStation);
			
				if (iwd.DoModal()!=IDOK)
				{
					stationLog.Format("Skipping station ID: %s\n",sta);  
					skipStation = true;
				   
				} else 
				{

				if (iwd.importWRCC == 1)
				{
					// use the WRCC ID
					
					stnSet.m_strFilter.Format("[StationID] = '%6.6s'", wrccSta);		
					stnSet.Requery();
					wxObs.m_StationID = wrccSta.Trim();
					sta = wrccSta;
					if (stnSet.IsEOF())
						existingStation = false;
					
					

				}  else
					stationLog.Format("Using WIMS station ID: %s\n",sta);
					stnSet.m_strFilter.Format("[StationID] = '%6.6s'", sta);
					stnSet.Requery();
					if (!stnSet.IsEOF())
						existingStation = true;
  
				}
				pCUPDUPData->SetFrontmost();
			}

			fprintf(errLog,"%s",stationLog);

			if (skipStation)
				continue;


			// check to see if we need to add a new station
			if (!existingStation)
			{




				COleDateTime lookG(1999, 4, 15, 14, 0, 0);
				COleDateTime lookF(1999, 10, 15, 14, 0, 0);



				stnSet.AddNew();
				stnSet.m_StationID.Format("%6.6s", sta);

				stnSet.m_Name = _T(" ");

				stnSet.m_NFDRSFM = _T(" ");
				stnSet.m_Use88 = 0;
				stnSet.m_LatDegrees = 0;
				stnSet.m_LonDegrees = 0;
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
				stnSet.m_RegSchdObs = 13;
				theApp.m_NFDRS2016.iSetFuelModel('V');
				stnSet.m_SCM_V = theApp.m_NFDRS2016.GetSCMax();
				stnSet.m_MX_Humid_V = theApp.m_NFDRS2016.GetMxdHumid();
				theApp.m_NFDRS2016.iSetFuelModel('W');
				stnSet.m_SCM_W = theApp.m_NFDRS2016.GetSCMax();
				stnSet.m_MX_Humid_W = theApp.m_NFDRS2016.GetMxdHumid();
				theApp.m_NFDRS2016.iSetFuelModel('X');
				stnSet.m_SCM_X = theApp.m_NFDRS2016.GetSCMax();
				stnSet.m_MX_Humid_X = theApp.m_NFDRS2016.GetMxdHumid();
				theApp.m_NFDRS2016.iSetFuelModel('Y');
				stnSet.m_SCM_Y = theApp.m_NFDRS2016.GetSCMax();
				stnSet.m_MX_Humid_Y = theApp.m_NFDRS2016.GetMxdHumid();
				theApp.m_NFDRS2016.iSetFuelModel('Z');
				stnSet.m_SCM_Z = theApp.m_NFDRS2016.GetSCMax();
				stnSet.m_MX_Humid_Z = theApp.m_NFDRS2016.GetMxdHumid();

				wrccStnSet.m_strFilter.Format("[WRCC_ID] = '%s'", wrccSta);
				wrccStnSet.Requery();
				//wrccStnSet.MoveFirst();
				if (!wrccStnSet.IsEOF())
				{//got a match for a wrcc station (most likely an FPA fwx file)
					//pull in data from station by wrcc_ID
					stnSet.m_Name = _T(wrccStnSet.m_Name);
					stnSet.m_NFDRSFM = wrccStnSet.m_NFDRSFM;
					stnSet.m_Use88 = wrccStnSet.m_Use88;
					stnSet.m_LatDegrees = wrccStnSet.m_LatDegrees;
					stnSet.m_LonDegrees = wrccStnSet.m_LonDegrees;
					stnSet.m_Elevation = wrccStnSet.m_Elevation;
					stnSet.m_ClimateCls = wrccStnSet.m_ClimateCls;
					stnSet.m_SlopeCls = wrccStnSet.m_SlopeCls;
					stnSet.m_HerbAnnual = wrccStnSet.m_HerbAnnual;
					stnSet.m_GreenJulian = wrccStnSet.m_GreenJulian;
					stnSet.m_FreezeJulian = wrccStnSet.m_FreezeJulian;
					stnSet.m_StartKBDI = wrccStnSet.m_StartKBDI;
					stnSet.m_Deciduous = wrccStnSet.m_Deciduous;
					stnSet.m_AvgPrecip = wrccStnSet.m_AvgPrecip;
					stnSet.m_Start1000 = wrccStnSet.m_Start1000;
					stnSet.m_FM1Eq10 = wrccStnSet.m_FM1Eq10;
					stnSet.m_Aspect = wrccStnSet.m_Aspect;
					stnSet.m_PsnOnSlope = wrccStnSet.m_PsnOnSlope;
					stnSet.m_RegSchdObs = wrccStnSet.m_RegSchdObs;
					stnSet.m_StnType = wrccStnSet.m_StnType;
					stnSet.m_ObsAgy = wrccStnSet.m_ObsAgy;
					stnSet.m_ObsUnit = wrccStnSet.m_ObsUnit;
					stnSet.m_Site = wrccStnSet.m_Site;
					stnSet.m_State = wrccStnSet.m_State;
					stnSet.m_FSRegion = wrccStnSet.m_FSRegion;
					stnSet.m_County = wrccStnSet.m_County;
					stnSet.m_WRCC_ID = wrccStnSet.m_WRCC_ID;
					stnSet.m_NESDIS_ID = wrccStnSet.m_NESDIS_ID;
					stnSet.m_UseStick = wrccStnSet.m_UseStick;
					stnSet.m_UseDormant = wrccStnSet.m_UseDormant;
					stnSet.m_DormantJulian = wrccStnSet.m_DormantJulian;



					fprintf(errLog, "Added Station: %s\n\t*Note: New station ID %s matched WRCC ID %s\n"
						"\tPopulating new station record with %s station data.\n",
						sta, sta, wrccStnSet.m_WRCC_ID, wrccStnSet.m_StationID);
				}
				else
					fprintf(errLog, "Added Station: %s\n", sta);

				stnSet.Update();

				staAdded++;
			}

					
				

			

			   staProcessed ++;


				//first, populate hourly precip
			wxObs.m_strFilter.Format("[StationID] = '%6.6s'", sta);
			wxObs.Requery();
			if(!wxObs.IsEOF())
			{
				COleDateTime startPrecip;

				int pcpSkip = wxObs.CalcHourlyPrecip(&startPrecip);
				if(pcpSkip < 0)
					fprintf(errLog, "Station: %s, unable to calculate hourly precip\n", sta);
				else
					fprintf(errLog, "Station: %s, skipped %d records to get to zero precip\n\tHourly precip started on %s\n",
					sta, pcpSkip, startPrecip.Format("%m/%d/%Y %H:%M"));
				wxObs.MoveFirst();
				wxObs.setDailyObs(regHour);
				wxObs.CalcSOW();
			}


			int regHour = 13;
			if(! stnSet.IsEOF())
			{
				regHour = stnSet.m_RegSchdObs;
			}
			stnSet.Close();
			// rebuild SQL string for wxObs
			strSQLa.Format("[StationID] = '%6.6s'", sta);

			

			wxObs.m_strFilter = strSQLa;
			twxObs.m_strFilter = strSQLb;
			wxObs.Requery();
			twxObs.Requery();
			lastDay.SetDateTime(1900,1, 1, 1, 1, 1);//minimum start date
			//twxObs.m_pDatabase->BeginTrans();
				while(!twxObs.IsEOF() && !pCUPDUPData->ShouldTerminate())// && (!cd || !cd->Abort()))
				{
					count ++;
					sprintf(progText, "Updating WxObs %ld", count);
					pCUPDUPData->SetProgress(progText);
					//if (cd)
					//	cd->Increment();
					//while(!twxObs.IsEOF() && twxObs.m_ObsDate <= lastDay)//may be dups in twxObs
					//	twxObs.MoveNext();
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
					//while(!wxObs.IsEOF() && (wxObs.m_ObsDate < twxObs.m_ObsDate))
					while(!wxObs.IsEOF()
						&& (wxObs.m_ObsDate.GetYear() < twxObs.m_ObsDate.GetYear()
						|| (wxObs.m_ObsDate.GetYear() == twxObs.m_ObsDate.GetYear() && wxObs.m_ObsDate.GetDayOfYear() < twxObs.m_ObsDate.GetDayOfYear())))
					{
						wxObs.MoveNext();
					}
					if(!wxObs.IsEOF() && (wxObs.m_ObsDate.GetDayOfYear() == twxObs.m_ObsDate.GetDayOfYear()))
					{
						//wxObs.Edit();
						edits++;
						twxObs.MoveNext();
						continue;
					}
					wxObs.AddNew();
					appends++;
					/*for(int i = 0; i < wxObs.GetFieldCount(); i++)
					{
					COleVariant var;
					twxObs.GetFieldValue(i, var);
					wxObs.SetFieldValue(i, var);
					}*/
					wxObs.m_StationID = twxObs.m_StationID;
					wxObs.m_ObsDate = twxObs.m_ObsDate;
					wxObs.m_Hour = wxObs.m_ObsDate.GetHour();
					wxObs.m_ObsType = twxObs.m_ObsType;
					wxObs.m_UserWX = twxObs.m_UserWX;
					if(twxObs.IsFieldNull(&twxObs.m_UserWX))
						wxObs.SetFieldNull(&wxObs.m_UserWX);
					wxObs.m_SOW = twxObs.m_SOW;
					if(twxObs.IsFieldNull(&twxObs.m_SOW))
						wxObs.SetFieldNull(&wxObs.m_SOW);
					wxObs.m_Temp = twxObs.m_Temp;
					if(twxObs.IsFieldNull(&twxObs.m_Temp))
						wxObs.SetFieldNull(&wxObs.m_Temp);
					wxObs.m_RH = max(twxObs.m_RH, 1);
					if(twxObs.IsFieldNull(&twxObs.m_RH))
						wxObs.SetFieldNull(&wxObs.m_RH);
					wxObs.m_WDir = twxObs.m_WDir;
					if(twxObs.IsFieldNull(&twxObs.m_WDir))
						wxObs.SetFieldNull(&wxObs.m_WDir);
					wxObs.m_WS = twxObs.m_WS;
					if(twxObs.IsFieldNull(&twxObs.m_WS))
						wxObs.SetFieldNull(&wxObs.m_WS);
					wxObs.m_OMC10 = twxObs.m_OMC10;
					if(twxObs.IsFieldNull(&twxObs.m_OMC10))
						wxObs.SetFieldNull(&wxObs.m_OMC10);
					wxObs.m_OMCWood = twxObs.m_OMCWood;
					if(twxObs.IsFieldNull(&twxObs.m_OMCWood))
						wxObs.SetFieldNull(&wxObs.m_OMCWood);
					wxObs.m_OMCWoodDate = twxObs.m_OMCWoodDate;
					if(twxObs.IsFieldNull(&twxObs.m_OMCWoodDate))
						wxObs.SetFieldNull(&wxObs.m_OMCWoodDate);
					wxObs.m_TmpMax = twxObs.m_TmpMax;
					if(twxObs.IsFieldNull(&twxObs.m_TmpMax))
						wxObs.SetFieldNull(&wxObs.m_TmpMax);
					wxObs.m_TmpMin = twxObs.m_TmpMin;
					if(twxObs.IsFieldNull(&twxObs.m_TmpMin))
						wxObs.SetFieldNull(&wxObs.m_TmpMin);
					wxObs.m_RHMax = twxObs.m_RHMax;
					if(twxObs.IsFieldNull(&twxObs.m_RHMax))
						wxObs.SetFieldNull(&wxObs.m_RHMax);
					wxObs.m_RHMin = max(twxObs.m_RHMin, 1);
					if(twxObs.IsFieldNull(&twxObs.m_RHMin))
						wxObs.SetFieldNull(&wxObs.m_RHMin);
					wxObs.m_PPTDUR = twxObs.m_PPTDUR;
					if(twxObs.IsFieldNull(&twxObs.m_PPTDUR))
						wxObs.SetFieldNull(&wxObs.m_PPTDUR);
					wxObs.m_PPTAMT = twxObs.m_PPTAMT;
					if(twxObs.IsFieldNull(&twxObs.m_PPTAMT))
						wxObs.SetFieldNull(&wxObs.m_PPTAMT);
					wxObs.m_Season = twxObs.m_Season;
					if(twxObs.IsFieldNull(&twxObs.m_Season))
						wxObs.SetFieldNull(&wxObs.m_Season);
					wxObs.m_GreenHerb = twxObs.m_GreenHerb;
					if(twxObs.IsFieldNull(&twxObs.m_GreenHerb))
						wxObs.SetFieldNull(&wxObs.m_GreenHerb);
					wxObs.m_GreenShrub = twxObs.m_GreenShrub;
					if(twxObs.IsFieldNull(&twxObs.m_GreenShrub))
						wxObs.SetFieldNull(&wxObs.m_GreenShrub);
					wxObs.m_MC1 = twxObs.m_MC1;
					if(twxObs.IsFieldNull(&twxObs.m_MC1))
						wxObs.SetFieldNull(&wxObs.m_MC1);
					wxObs.m_MC10 = twxObs.m_MC10;
					if(twxObs.IsFieldNull(&twxObs.m_MC10))
						wxObs.SetFieldNull(&wxObs.m_MC10);
					wxObs.m_MC100 = twxObs.m_MC100;
					if(twxObs.IsFieldNull(&twxObs.m_MC100))
						wxObs.SetFieldNull(&wxObs.m_MC100);
					wxObs.m_MC1000 = twxObs.m_MC1000;
					if(twxObs.IsFieldNull(&twxObs.m_MC1000))
						wxObs.SetFieldNull(&wxObs.m_MC1000);
					wxObs.m_X1000 = twxObs.m_X1000;
					if(twxObs.IsFieldNull(&twxObs.m_X1000))
						wxObs.SetFieldNull(&wxObs.m_X1000);
					wxObs.m_MCHerb = twxObs.m_MCHerb;
					if(twxObs.IsFieldNull(&twxObs.m_MCHerb))
						wxObs.SetFieldNull(&wxObs.m_MCHerb);
					wxObs.m_MCWood = twxObs.m_MCWood;
					if(twxObs.IsFieldNull(&twxObs.m_MCWood))
						wxObs.SetFieldNull(&wxObs.m_MCWood);
					wxObs.m_GREN = twxObs.m_GREN;
					if(twxObs.IsFieldNull(&twxObs.m_GREN))
						wxObs.SetFieldNull(&wxObs.m_GREN);
					wxObs.m_HStage = twxObs.m_HStage;
					if(twxObs.IsFieldNull(&twxObs.m_HStage))
						wxObs.SetFieldNull(&wxObs.m_HStage);
					wxObs.m_ROS = twxObs.m_ROS;
					if(twxObs.IsFieldNull(&twxObs.m_ROS))
						wxObs.SetFieldNull(&wxObs.m_ROS);
					wxObs.m_ERC = twxObs.m_ERC;
					if(twxObs.IsFieldNull(&twxObs.m_ERC))
						wxObs.SetFieldNull(&wxObs.m_ERC);
					wxObs.m_BI = twxObs.m_BI;
					if(twxObs.IsFieldNull(&twxObs.m_BI))
						wxObs.SetFieldNull(&wxObs.m_BI);
					wxObs.m_FIL = twxObs.m_FIL;
					if(twxObs.IsFieldNull(&twxObs.m_FIL))
						wxObs.SetFieldNull(&wxObs.m_FIL);
					wxObs.m_WAzimuth = twxObs.m_WAzimuth;
					if(twxObs.IsFieldNull(&twxObs.m_WAzimuth))
						wxObs.SetFieldNull(&wxObs.m_WAzimuth);

					// new FFP4 fields   05/2006
					wxObs.m_SolarRadiation = twxObs.m_SolarRadiation;
					if(twxObs.IsFieldNull(&twxObs.m_SolarRadiation))
						wxObs.SetFieldNull(&wxObs.m_SolarRadiation);

					wxObs.m_WetFlag = twxObs.m_WetFlag;
					if(twxObs.IsFieldNull(&twxObs.m_WetFlag))
						wxObs.SetFieldNull(&wxObs.m_WetFlag);

					wxObs.m_SnowFlag = twxObs.m_SnowFlag;
					if(twxObs.IsFieldNull(&twxObs.m_SnowFlag))
						wxObs.SetFieldNull(&wxObs.m_SnowFlag);

					wxObs.m_GustDir = twxObs.m_GustDir;
					if(twxObs.IsFieldNull(&twxObs.m_GustDir))
						wxObs.SetFieldNull(&wxObs.m_GustDir);

					wxObs.m_GustSpd = twxObs.m_GustSpd;
					if(twxObs.IsFieldNull(&twxObs.m_GustSpd))
						wxObs.SetFieldNull(&wxObs.m_GustSpd);

					//added 12/16/2011 for WFDSS FWX export
					wxObs.m_SR_WetFlag = twxObs.m_SR_WetFlag;
					if(twxObs.IsFieldNull(&twxObs.m_SR_WetFlag))
						wxObs.SetFieldNull(&wxObs.m_SR_WetFlag);
					wxObs.m_SR_SOW = twxObs.m_SR_SOW;
					if(twxObs.IsFieldNull(&twxObs.m_SR_SOW))
						wxObs.SetFieldNull(&wxObs.m_SR_SOW);

					// force stationID to the WIMS ID used for import
					// (twxObs WRCC ID may correlate to wxObs WIMS ID)
			
					wxObs.m_StationID.Format("%6.6s",sta);




					/* 09/2012   set DailyObs field appropriately */
					wxObs.m_DailyObs = 0;
					if (twxObs.m_ObsType.CompareNoCase("O") == 0)
						wxObs.m_DailyObs = 1;
					else
						if ((twxObs.m_ObsType.CompareNoCase("R") == 0)
							&& (twxObs.m_Hour == regHour)
							&& !twxObs.IsFieldNull(&twxObs.m_SOW))
							wxObs.m_DailyObs = 1;

					wxObs.Update();
					LogWxDataWarnings(errLog, &twxObs);
					twxObs.MoveNext();
				}
				//if(!cd || !cd->Abort())
				//{
				pCUPDUPData->AllowCancel(FALSE);
				pCUPDUPData->SetProgress("Setting DailyObs...");
				//twxObs.m_pDatabase->CommitTrans(); //so we can requery
					// populate hourly precip
					wxObs.m_strSort.Format("[ObsDate]");
					wxObs.Requery();
			
					if(!wxObs.IsEOF())
					{
						COleDateTime startPrecip;

						int pcpSkip = wxObs.CalcHourlyPrecip(&startPrecip);
						if(pcpSkip < 0)
							fprintf(errLog, "Station: %s, unable to calculate hourly precip\n", sta);
						else
							fprintf(errLog, "Station: %s, skipped %d records to get to zero precip\n\tHourly precip started on %s\n",
								sta, pcpSkip, startPrecip.Format("%m/%d/%Y %H:%M"));
						wxObs.MoveFirst();
						wxObs.setDailyObs(regHour);
						wxObs.CalcSOW();
						wxObs.Requery();
					}
					pCUPDUPData->AllowCancel(TRUE);

				//} else
				//	twxObs.m_pDatabase->Rollback();

			
				
			}
		//}
		//else
			//twxObs.m_pDatabase->Rollback();
		
		//EnableWindow(TRUE);
		//if(!cd || !cd->Abort())
		//{

			if (raws > count)
				raws = count;

			fprintf(errLog, "Processed %ld weather records for %d stations:\n",count,staProcessed);
			

			if (staProcessed > 0)
			{
				fprintf(errLog,
				"\tProcessed %ld weather records.\n"
				"\tAppended %ld records.\n"
				"\tRejected %ld duplicate records.\n"	
				
				"\tRejected %ld records for bad date field.\n"
				"\tAdded %d new stations.\n",
				count, appends, edits,  rejects, staAdded);

			}
		/*}
		else
		{
			//		wxObs.m_pDatabase->m_pWorkspace->Rollback();
			//fclose(errLog);
			errors = -1;
		}*/
		staList.RemoveAll();
		//if (cd)
		//	cd->DestroyWindow();

		wxObs.Close();
		twxObs.Close();
		stnSet.Close();
		wrccStnSet.Close();
		//pDoc->m_pDB->DeleteTableDef("ffpTempWx");
		try
		{
			CString strSql = "DROP TABLE [ffpTempWx]";
			pDoc->m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		return errors;
		//return 1;
	}

	int CImportDialog::ImportFPL(CString fileName, FILE *errLog, long agencyID)
	{
		//AfxMessageBox("Not yet implemented due to database engine upgrade.");
		int errors = 0;
		fplLineNo = 0;
		CFireFilterDialog ffd(this);
		if(agencyID != 1)//DOI fpl file
		{
			if(ffd.DoModal() != IDOK)
				return -1;
		}
		//pDoc->m_pDB->m_pWorkspace->BeginTrans();
		CWaitCursor wait;
		char buf[270];
		fprintf(errLog, "\n**********************************************\nImport File Name: %s\n", fileName);
		//read the FIRE observations into the database
		EnableWindow(FALSE);

		/*CCancelDialog *cd = NULL;

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
		cd->SetTitle("Importing Fire Data");
	  }
	}*/


		
		
		if(pDoc->HasTempFire())
		{
			try
			{
				pDoc->m_pDB->ExecuteSQL("DROP TABLE [ffpTempFire]");
			}
			catch(CDBException* e)
			{
				e->Delete();
			}
			//pDoc->m_pDB->DeleteTableDef("ffpTempFire");
		}
		pDoc->CreateTempFire();
		CFiresSet tFireset(firesSet.m_pDatabase);
		tFireset.Open(CRecordset::dynaset, "ffpTempFire");
		FILE *stream = fopen(fileName, "rb");
		int result;
		count = 0;
		long updates = 0, noDates = 0, others = 0;
		if(stream)
		{
			//long fireID;
			//firesSet.m_strSort.Format("[FireID]");
			//fireSet.m_strFilter.Format("");
			//fireSet.Requery();
			//if(fireSet.IsBOF() && fireSet.IsEOF())
			//	fireID = 1;
			//else
			//{
			//	fireSet.MoveLast();
			//	fireID = fireSet.m_FireID;
			//}
			while(!feof(stream))// && (!cd || !cd->Abort()))
			{
				if(GetFireRecFPL(buf, stream, agencyID, &ffd) == 0)//wrong type
				{
					others++;
					continue;
				}
				//fgets(buf, 160, stream);
				if(feof(stream))
					break;
				//set record members to values from file
				result = WriteFireRecFPL(buf, agencyID, errLog, &tFireset);
				//if(result == 1)
				//	updates++;
				if(result == -1)
				{
					noDates++;
					errors++;
				}
				else if(result == 0)
					count++;
				else if(result != 1)
				{
					//others++;
					errors++;
				}
				//if (cd)
				//   cd->Increment();
			}
		}
		fclose(stream);
		//now append/update temp recs to real table
		updates = WriteTempFires(&tFireset, agencyID);
		tFireset.Close();
		//dateTime = COleDateTime::GetCurrentTime();
		CString strMsg;
		//if(!cd || !cd->Abort())
		//{
			//fprintf(errLog, "\nImport Completed: %s\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			fprintf(errLog, "Processed %ld fire records\n"
				"\tAppended %ld fire records.\n"
				"\tRejected %ld duplicate fire records.\n"
				"\tRejected %ld records for bad data.\n",
				count + noDates + errors + others, count - updates, updates, noDates + errors);
			if(others > 0)
			{
				fprintf(errLog, "\tRejected %ld unselected record types.\n", others);
			}
		/*}
		else
		{
			errors = -1;
			//fprintf(errLog, "\nImport Cancelled: %s\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			fprintf(errLog, "Import Cancelled by User\n");
		}*/
		//if (cd)
		//	cd->DestroyWindow();
		EnableWindow(TRUE);
		//fclose(errLog);
		//CImportDoneDialog idd(this);
		//idd.statusStr = strMsg;
		//idd.logFileName = errFileName;
		//idd.DoModal();
		//delete[] errFileName;
		//theApp.SetUserDir(dlg.GetPathName());
		//	pDoc->m_pDB->DeleteTableDef("ffpTempFire");

		return errors;
		//return 1;
	}

	int CImportDialog::ImportRAW(CString fileName, FILE *errLog, long agencyID)
	{
		//AfxMessageBox("Not yet implemented due to database engine upgrade.");
		int errors = 0;
		// if user selected a file then open it as a database
		CWaitCursor wait;
		char buf[270];
		fprintf(errLog, "**************************************************\nImport File Name: %s\n", fileName);
		EnableWindow(FALSE);

//		CCancelDialog *cd = NULL;
	
/*	cd = new CCancelDialog();

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
		 cd->SetTitle("Importing Fire Data");
	  }
	}
	*/
	
		
		if(pDoc->HasTempFire())
		{
			try
			{
				pDoc->m_pDB->ExecuteSQL("DROP TABLE [ffpTempFire]");
			}
			catch(CDBException* e)
			{
				e->Delete();
			}
			//pDoc->m_pDB->DeleteTableDef("ffpTempFire");
		}
		pDoc->CreateTempFire();
		CFiresSet tFireset(firesSet.m_pDatabase);
		tFireset.Open(CRecordset::dynaset, "ffpTempFire");
		FILE *stream = fopen(fileName, "rb");
		count = 0;
		long updates = 0, rejects = 0;
		if(stream)
		{
			//fgets(buf, 262, stream);

			fplLineNo = 0;
			long fireID, firstRecSeek;
			int line1 = 0, line2 = 0, line3 = 0;
			bool threeLineFormat = false;
			firstRecSeek = ftell(stream);
			fgets(buf, 262, stream);
			//strip blank friggin lines added in 2006
			//line1 = strlen(stripCRLF(buf));
			CString bLine = buf;
			bLine.Trim(" \r\n\t");
			while(bLine.GetLength() <= 0)
			{
				firstRecSeek = ftell(stream);
				fgets(buf, 262, stream);
				bLine = buf;
				bLine.Trim(" \r\n\t");
			}
			line1 = strlen(stripCRLF(buf));
			if(!feof(stream))
			{
				fgets(buf, 262, stream);
				line2 = strlen(stripCRLF(buf));
			}
			if(!feof(stream))
			{
				fgets(buf, 262, stream);
				line3 = strlen(stripCRLF(buf));
			}
			if(line1 == 80 && line2 == 80 && line3 == 80)
				threeLineFormat = false;
			else
				threeLineFormat = true;
			rewind(stream);
			fseek(stream, firstRecSeek, SEEK_SET);
			//firesSet.m_strSort.Format("[FireID]");
			//fireSet.m_strFilter.Format("");
			/*firesSet.Requery();
			if(fireSet.IsBOF() && fireSet.IsEOF())
				fireID = 1;
			else
			{
				fireSet.MoveLast();
				fireID = fireSet.m_FireID;
			}*/
			while(!feof(stream) )
			{
				//if (cd && cd->Abort())
				//	break;
				GetFireRecRAW(buf, stream, agencyID, threeLineFormat);
				if(feof(stream))
					break;
				//set record members to values from file
				int f = WriteFireRecRAW(buf, agencyID, errLog, &tFireset);
				//if(f >= 0)
				//	updates += f;
				if(f < 0)
				{
					errors++;
					rejects++;
				}
				count++;
				//if (cd)
				 //  cd->Increment();
			
			}
		}
		fclose(stream);
		//dateTime = COleDateTime::GetCurrentTime();
		//CString strMsg;
		updates = WriteTempFires(&tFireset, agencyID);
		tFireset.Close();
		//if(!cd || !cd->Abort())
		//{
			//	pDoc->m_pDB->m_pWorkspace->CommitTrans();
			//fprintf(errLog, "\nImport Completed: %s\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			fprintf(errLog, "Processed %ld fire records\n"
				"\tAppended %ld fire records.\n"
				"\tRejected %ld duplicate fire records.\n"
				"\tRejected %ld fire records for bad data.\n",
				count, count - updates - rejects, updates, rejects);
		/*}
		else
		{
			errors = -1;
			//	pDoc->m_pDB->m_pWorkspace->Rollback();
			//fprintf(errLog, "\nImport Cancelled: %s\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			fprintf(errLog, "Import Cancelled by User.");
		}*/
		tFireset.Close();
		EnableWindow(TRUE);
		//	pDoc->m_pDB->DeleteTableDef("ffpTempFire");
		//if (cd)
		//	cd->DestroyWindow();

		return errors;
		//return 1;
	}

	//long CImportDialog::WriteTempFires(CFireSet *tFires, CCancelDialog *cd, long agencyID)
	long CImportDialog::WriteTempFires(CFiresSet *tFires, long agencyID)
	{
		//	tFires->m_strSort = "[AgencyID],[RegionID],[UnitID],[Year],[FireNumber]";
		//	fireSet.m_strSort = "[AgencyID],[RegionID],[UnitID],[Year],[FireNumber]";
		//AfxMessageBox("Not yet implemented due to database engine upgrade.");
		long updates = 0;// , lastID;
		/*if (cd)
		{
		   cd->Reset();
		   cd->SetStatusString("Writing Records...");
		}*/
		if(tFires->IsBOF())
			return 0;
		firesSet.m_strSort.Format("[FireNumber]");
		firesSet.m_strFilter.Format("");
		firesSet.Requery();
		/*if (fireSet.IsBOF() && fireSet.IsEOF())
			lastID = 1;
		else
		{
			fireSet.MoveLast();
			lastID = fireSet.m_FireID;
		}*/
		//CString lYear, hYear;
		//tFires->m_strSort = "[Year]";
		//tFires->Requery();
		//lYear = hYear = tFires->m_Year;
		tFires->MoveLast();
		//if(!tFires->IsEOF())
		//hYear = tFires->m_Year;
		long lUnit, hUnit,hAgency;
		tFires->m_strSort = "[UnitID]";
		tFires->Requery();
		lUnit = hUnit = tFires->m_UnitID;
		tFires->MoveLast();
		//if(!tFires->IsEOF())
		hUnit = tFires->m_UnitID;
		hAgency = tFires->m_AgencyID;

		tFires->m_strSort = "[UnitID],[FireNumber]";

		firesSet.m_strSort = "[UnitID],[FireNumber]";
		firesSet.m_strFilter.Format("[AgencyID] = %ld and [UnitID] >= %ld and [UnitID] <= %ld",
			(agencyID?agencyID:hAgency), lUnit, hUnit);
		tFires->Requery();
		firesSet.Requery();
		//fireSet.m_pDatabase->BeginTrans();

		//while ((!cd || !cd->Abort()) && !tFires->IsEOF())
		while (!tFires->IsEOF())
		{
			while(!firesSet.IsEOF() && tFires->m_UnitID > firesSet.m_UnitID)
				firesSet.MoveNext();
			//	fireSet.MoveNext();
			while(!firesSet.IsEOF() && tFires->m_UnitID == firesSet.m_UnitID
				&& tFires->m_FireNumber > firesSet.m_FireNumber)
				firesSet.MoveNext();
			//if(!cd || !cd->Abort())
			//{
				//if (cd)
				//   cd->Increment();
				if(!firesSet.IsEOF() && tFires->m_UnitID == firesSet.m_UnitID
					//&& tFires->m_Year == fireSet.m_Year
					&& firesSet.m_FireNumber == tFires->m_FireNumber)
				{
					//fireSet.Edit();
					updates++;
				}
				else
				{
					firesSet.AddNew();
					//fireSet.m_FireID = ++lastID;
					/*for(int i = 1; i < fireSet.GetODBCFieldCount(); i++)
					{
					CDBVariant var;
					tFires->GetFieldValue(i, var);
					//if(var.vt != VT_NULL)
					fireSet.SetFieldValue(i, var);
					}*/
					//fireSet.m_FireID = tFires->m_FireID;	//Local unique key
					//firesSet.m_Year = tFires->m_Year;	//Discovery year
					firesSet.m_AgencyID = tFires->m_AgencyID;	//Protection Agency - Link to FireAgency table
					firesSet.m_RegionID = tFires->m_RegionID;	//Region, broad management unit- Link to FireRegions table
					if(tFires->IsFieldNull(&tFires->m_RegionID))
						firesSet.SetFieldNull(&firesSet.m_RegionID);
					firesSet.m_UnitID = tFires->m_UnitID;	//Report Unit (Forest, BLM District, NPS unit, etc.)- Link to FireUnits table
					if(tFires->IsFieldNull(&tFires->m_UnitID))
						firesSet.SetFieldNull(&firesSet.m_UnitID);
					firesSet.m_SubunitID = tFires->m_SubunitID;	//Subunit - Link to FireSubunits table
					if(tFires->IsFieldNull(&tFires->m_SubunitID))
						firesSet.SetFieldNull(&firesSet.m_SubunitID);
					firesSet.m_FireNumber = tFires->m_FireNumber;	//Fire identifier from report system
					if(tFires->IsFieldNull(&tFires->m_FireNumber))
						firesSet.SetFieldNull(&firesSet.m_FireNumber);
					//firesSet.m_State = tFires->m_State;	//State/Country Code
					//if(tFires->IsFieldNull(&tFires->m_State))
					//	fireSet.SetFieldNull(&fireSet.m_State);
					//fireSet.m_County = tFires->m_County;	//FIPS county number
					//if(tFires->IsFieldNull(&tFires->m_County))
					//	fireSet.SetFieldNull(&fireSet.m_County);
					firesSet.m_TotalAcres = tFires->m_TotalAcres;	//Total Fire Acres
					if(tFires->IsFieldNull(&tFires->m_TotalAcres))
						firesSet.SetFieldNull(&firesSet.m_TotalAcres);
					//fireSet.m_SizeClass = tFires->m_SizeClass;	//Fire Size Class
					//if(tFires->IsFieldNull(&tFires->m_SizeClass))
					//	fireSet.SetFieldNull(&fireSet.m_SizeClass);
					firesSet.m_FireName = tFires->m_FireName;	//Name of the Fire
					if(tFires->IsFieldNull(&tFires->m_FireName))
						firesSet.SetFieldNull(&firesSet.m_FireName);
					firesSet.m_StatisticalCause = tFires->m_StatisticalCause;	//FS Statistical Cause code
					if(tFires->IsFieldNull(&tFires->m_StatisticalCause))
						firesSet.SetFieldNull(&firesSet.m_StatisticalCause);
					/*fireSet.m_SpecificCause = tFires->m_SpecificCause;	//FS Specific cause code
					if(tFires->IsFieldNull(&tFires->m_SpecificCause))
						fireSet.SetFieldNull(&fireSet.m_SpecificCause);
					fireSet.m_GeneralCause = tFires->m_GeneralCause;	//FS General Cause code
					if(tFires->IsFieldNull(&tFires->m_GeneralCause))
						fireSet.SetFieldNull(&fireSet.m_GeneralCause);
					fireSet.m_ClassPeople = tFires->m_ClassPeople;	//FS Class of People code
					if(tFires->IsFieldNull(&tFires->m_ClassPeople))
						fireSet.SetFieldNull(&fireSet.m_ClassPeople);
					fireSet.m_CauseNarr = tFires->m_CauseNarr;	//Free text cause description
					if(tFires->IsFieldNull(&tFires->m_CauseNarr))
						fireSet.SetFieldNull(&fireSet.m_CauseNarr);
					fireSet.m_Township = tFires->m_Township;	//Township code
					if(tFires->IsFieldNull(&tFires->m_Township))
						fireSet.SetFieldNull(&fireSet.m_Township);
					fireSet.m_Range = tFires->m_Range;	//Range code
					if(tFires->IsFieldNull(&tFires->m_Range))
						fireSet.SetFieldNull(&fireSet.m_Range);
					fireSet.m_Section = tFires->m_Section;	//Section
					if(tFires->IsFieldNull(&tFires->m_Section))
						fireSet.SetFieldNull(&fireSet.m_Section);
					fireSet.m_SubSection = tFires->m_SubSection;	//Subsection (smallest first)
					if(tFires->IsFieldNull(&tFires->m_SubSection))
						fireSet.SetFieldNull(&fireSet.m_SubSection);*/
					firesSet.m_latitude = tFires->m_latitude;
					if (tFires->IsFieldNull(&tFires->m_latitude))
						firesSet.SetFieldNull(&firesSet.m_latitude);
					firesSet.m_longitude = tFires->m_longitude;
					if (tFires->IsFieldNull(&tFires->m_longitude))
						firesSet.SetFieldNull(&firesSet.m_longitude);
					/*fireSet.m_LatDD = tFires->m_LatDD;	//Latitude Deg (+ for East, - for West)
					if(tFires->IsFieldNull(&tFires->m_LatDD))
						fireSet.SetFieldNull(&fireSet.m_LatDD);
					fireSet.m_LatMM = tFires->m_LatMM;	//Latitude minutes
					if(tFires->IsFieldNull(&tFires->m_LatMM))
						fireSet.SetFieldNull(&fireSet.m_LatMM);
					fireSet.m_LatSS = tFires->m_LatSS;	//Latitude seconds
					if(tFires->IsFieldNull(&tFires->m_LatSS))
						fireSet.SetFieldNull(&fireSet.m_LatSS);
					fireSet.m_LonDD = tFires->m_LonDD;	//Longitude Deg (+ for North, - for South)
					if(tFires->IsFieldNull(&tFires->m_LonDD))
						fireSet.SetFieldNull(&fireSet.m_LonDD);
					fireSet.m_LonMM = tFires->m_LonMM;	//Longitude minutes
					if(tFires->IsFieldNull(&tFires->m_LonMM))
						fireSet.SetFieldNull(&fireSet.m_LonMM);
					fireSet.m_LonSS = tFires->m_LonSS;	//Longitude seconds
					if(tFires->IsFieldNull(&tFires->m_LonSS))
						fireSet.SetFieldNull(&fireSet.m_LonSS);
					fireSet.m_Other = tFires->m_Other;	//Other location descriptor
					if(tFires->IsFieldNull(&tFires->m_Other))
						fireSet.SetFieldNull(&fireSet.m_Other);
					fireSet.m_WildNum = tFires->m_WildNum;	//Wilderness ID
					if(tFires->IsFieldNull(&tFires->m_WildNum))
						fireSet.SetFieldNull(&fireSet.m_WildNum);
					fireSet.m_Notes = tFires->m_Notes;	//General location notes
					if(tFires->IsFieldNull(&tFires->m_Notes))
						fireSet.SetFieldNull(&fireSet.m_Notes);
					fireSet.m_Ignition = tFires->m_Ignition;	//Date/time of ignition (ofetn estimated)
					if(tFires->IsFieldNull(&tFires->m_Ignition))
						fireSet.SetFieldNull(&fireSet.m_Ignition);*/
					firesSet.m_Discovery = tFires->m_Discovery;	//Date/time of discovery
					if(tFires->IsFieldNull(&tFires->m_Discovery))
						firesSet.SetFieldNull(&firesSet.m_Discovery);
					/*fireSet.m_FirstAttack = tFires->m_FirstAttack;	//Date/time of first attack
					if(tFires->IsFieldNull(&tFires->m_FirstAttack))
						fireSet.SetFieldNull(&fireSet.m_FirstAttack);
					fireSet.m_Reinforcement = tFires->m_Reinforcement;	//Date/time ofreinforcment
					if(tFires->IsFieldNull(&tFires->m_Reinforcement))
						fireSet.SetFieldNull(&fireSet.m_Reinforcement);
					fireSet.m_DeclareWildlandFire = tFires->m_DeclareWildlandFire;	//Date/time of declared wildland
					if(tFires->IsFieldNull(&tFires->m_DeclareWildlandFire))
						fireSet.SetFieldNull(&fireSet.m_DeclareWildlandFire);*/
					firesSet.m_Contain = tFires->m_Contain;	//Date/time of fire contained
					if(tFires->IsFieldNull(&tFires->m_Contain))
						firesSet.SetFieldNull(&firesSet.m_Contain);
					firesSet.m_StrategyMet = tFires->m_StrategyMet;	//Date/time of strategy (usually control) met
					if(tFires->IsFieldNull(&tFires->m_StrategyMet))
						firesSet.SetFieldNull(&firesSet.m_StrategyMet);
					/*fireSet.m_FireOut = tFires->m_FireOut;	//Date/time fire declared out
					if(tFires->IsFieldNull(&tFires->m_FireOut))
						fireSet.SetFieldNull(&fireSet.m_FireOut);
					// new FFP4 fields   05/2006
					fireSet.m_Slope = tFires->m_Slope;
					if(tFires->IsFieldNull(&tFires->m_Slope))
						fireSet.SetFieldNull(&fireSet.m_Slope);

					fireSet.m_Elevation = tFires->m_Elevation;
					if(tFires->IsFieldNull(&tFires->m_Elevation))
						fireSet.SetFieldNull(&fireSet.m_Elevation);

					fireSet.m_Aspect = tFires->m_Aspect;
					if(tFires->IsFieldNull(&tFires->m_Aspect))
						fireSet.SetFieldNull(&fireSet.m_Aspect);

					fireSet.m_FuelModel = tFires->m_FuelModel;
					if(tFires->IsFieldNull(&tFires->m_FuelModel))
						fireSet.SetFieldNull(&fireSet.m_FuelModel);

					fireSet.m_FireType = tFires->m_FireType;
					if(tFires->IsFieldNull(&tFires->m_FireType))
						fireSet.SetFieldNull(&fireSet.m_FireType);*/

					firesSet.Update();
				}
			//}
			tFires->MoveNext();
		}
		//if(!cd || !cd->Abort())
			//fireSet.m_pDatabase->CommitTrans();
		//else
		//	fireSet.m_pDatabase->Rollback();
		return updates;
		//return 0;
	}

	void CImportDialog::LogWxDataWarnings(FILE *logFile, CWxSet *wxSet)
	{
		//precip
		if(wxSet->m_PPTAMT > 2.00)
		{
			fprintf(logFile, "Warning: StationID %s Date: %s Precip Amount = %.2f inches (> 2.00 inches)\n",
				wxSet->m_StationID, wxSet->m_ObsDate.Format("%m/%d/%y %H:%M"), wxSet->m_PPTAMT);
		}
	}

	void CImportDialog::OnFwstxt()
	{
		long agencyID = -1;
		if(agencies.GetCurSel() != CB_ERR)
			agencyID = agencies.GetItemData(agencies.GetCurSel());
		else
		{//report no agency selected
			CString strMsg = "An Agency must be selected in order to import fire data.";
			AfxMessageBox(strMsg);
			return;
		}
		if(agencyID != 5) //FWS check
		{//report wrong agency selected
			CString strMsg = "Error. Make sure FWS is the selected agency.";
			AfxMessageBox(strMsg);
			return;
		}
		chdir(theApp.userDir);
		CFileDialog	dlg (TRUE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
			OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
			_T("FWS Short Fire Record Files (*.txt)|*.txt|All Files (*.*)|*.*||"), this);
		char *mfBuf = new char[MF_BUFSIZE];
		mfBuf[0] = 0;
		dlg.m_ofn.lpstrFile = mfBuf;
		dlg.m_ofn.nMaxFile = MF_BUFSIZE;
		int res = dlg.DoModal();
		nNewSubunits = nNewUnits = 0;
		if (res == IDOK)
		{
			CWaitCursor wait;
			long errors = 0;
			// Get the position of the first filename
			char *logFileName = GetTempFileName();
			FILE * logFile = fopen(logFileName, "wt");
			COleDateTime dateTime = COleDateTime::GetCurrentTime();
			fprintf(logFile, "FireFamily Plus FWS Short File Fire Data Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			POSITION filenamePosition = dlg.GetStartPosition();
			CString fileName;

			// Open each file selected
			int tErrors;
			while ( filenamePosition != NULL )
			{
				fileName = dlg.GetNextPathName( filenamePosition );
				tErrors = ImportFWSShort(fileName, logFile);
				if(tErrors >= 0)
					errors += tErrors;
				else
				{//user cancelled
					errors = -1;
					break;
				}
			}
			dateTime = COleDateTime::GetCurrentTime();
			CString strMsg;
			if(errors >= 0)
			{
				fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
				strMsg.Format("Import Complete.\n%ld errors were encountered.", errors);
				if(nNewUnits > 0)
				{
					CString nuStr;
					nuStr.Format("\n%ld new Fire Units were created.", nNewUnits);
					strMsg = strMsg + nuStr;
				}
				if(nNewSubunits > 0)
				{
					CString nuStr;
					nuStr.Format("\n%ld new Fire SubUnits were created.", nNewSubunits);
					strMsg = strMsg + nuStr;
				}
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
			theApp.SetUserDir(dlg.GetPathName());
		}
		else if(res == IDCANCEL)
		{
			int err = CommDlgExtendedError();
			if(err == FNERR_BUFFERTOOSMALL)
			{
				AfxMessageBox("File Name buffer overrun, select fewer files.");
			}
		}
		delete[] mfBuf;
		chdir(theApp.dbDir);
	}

	int CImportDialog::ImportFWSShort(CString fileName, FILE *errLog)
	{
		//AfxMessageBox("Not yet implemented due to database engine upgrade.");
		int errors = 0;
		fplLineNo = 0;
		CFireFilterDialog ffd(this);
		if(ffd.DoModal() != IDOK)
			return -1;
		long agencyID = 5;
		CWaitCursor wait;
		char buf[160];
		fprintf(errLog, "\n**********************************************\nImport File Name: %s\n FWS Short Format\n", fileName);
		//read the FIRE observations into the database
		EnableWindow(FALSE);

//		CCancelDialog *cd = NULL;
	
/*	cd = new CCancelDialog();

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
		cd->SetTitle("Importing Fire Data");
	  }
	}*/
		
		
		if(pDoc->HasTempFire())
		{
			try
			{
				pDoc->m_pDB->ExecuteSQL("DROP TABLE [ffpTempFire]");
			}
			catch(CDBException* e)
			{
				e->Delete();
			}
			//pDoc->m_pDB->DeleteTableDef("ffpTempFire");
		}
		pDoc->CreateTempFire();
		CFiresSet tFireset(firesSet.m_pDatabase);
		tFireset.Open(CRecordset::dynaset, "ffpTempFire");
		FILE *stream = fopen(fileName, "rb");
		int result;

		count = 0;
		long updates = 0, noDates = 0, others = 0;
		if(fgets(buf, 159, stream))
		{
			stripCRLF(buf);
			if(strcmp(buf, "ORGCODE~UNITIDENTIFIER~FIRENAME~FIRENUM~FIRESUBTYPE~IGNITIONCAUSE~DISCOVERYDATETIME~LATITUDE~LONGITUDE~TOTALACRES") != 0)
			{
				AfxMessageBox("Error. Incorrect Header for FWS Short format.");
				fprintf(errLog, "\nError. Incorrect Header for FWS Short format.\n");
				fclose(errLog);
				fclose(stream);
				return 1;
			}
		}
		if(stream)
		{
			long fireID;
			/*firesSet.m_strSort.Format("[FireID]");
			firesSet.m_strFilter.Format("");
			firesSet.Requery();
			if(firesSet.IsBOF() && firesSet.IsEOF())
				fireID = 1;
			else
			{
				firesSet.MoveLast();
				fireID = firesSet.m_FireID;
			}*/
			while(!feof(stream))// && (!cd || !cd->Abort()))
			{
				if(GetFireRecFWSshort(buf, stream, &ffd) == 0)//wrong type
				{
					others++;
					continue;
				}
				if(feof(stream))
					break;
				//set record members to values from file
				result = WriteFireRecFWSshort(buf, errLog, &tFireset);
				//if(result == 1)
				//	updates++;
				if(result == -1)
				{
					noDates++;
					//errors++;
				}
				else if(result == 0)
					count++;
				else if(result != 1)
				{
					//others++;
					errors++;
				}
				//if (cd)
				//   cd->Increment();
			}
		}
		fclose(stream);
		//now append/update temp recs to real table
		updates = WriteTempFires(&tFireset, agencyID);
		tFireset.Close();
		//dateTime = COleDateTime::GetCurrentTime();
		CString strMsg;
		//if(!cd || !cd->Abort())
		//{
			//fprintf(errLog, "\nImport Completed: %s\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			fprintf(errLog, "Processed %ld fire records\n"
				"\tAppended %ld fire records.\n"
				"\tRejected %ld duplicate fire records.\n"
				"\tRejected %ld records for bad data.\n",
				count + noDates + errors + others, count - updates, updates, noDates + errors);
			if(others > 0)
			{
				fprintf(errLog, "\tRejected %ld records for unselected record type.\n", others);
			}
		/*}
		else
		{
			errors = -1;
			//fprintf(errLog, "\nImport Cancelled: %s\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			fprintf(errLog, "Import Cancelled by User\n");
		}*/
		EnableWindow(TRUE);
		//fclose(errLog);
		//CImportDoneDialog idd(this);
		//idd.statusStr = strMsg;
		//idd.logFileName = errFileName;
		//idd.DoModal();
		//delete[] errFileName;
		//theApp.SetUserDir(dlg.GetPathName());
		//	pDoc->m_pDB->DeleteTableDef("ffpTempFire");
		//if (cd)
		//	cd->DestroyWindow();
		return errors;
		//return 1;
	}

	CString TokenFromFWSShortBuf(char *buf, int tokenNum)
	{
		char temp[80];
		int tok = 0;
		int loc = 0;

		for(int i = 0; i <= strlen(buf); i++)
		{
			if(tok < tokenNum)
			{
				if(buf[i] == '~')
					tok++;
			}
			else
			{
				if(buf[i] != '~' || buf[i] == 0)
					temp[loc++] = buf[i];
				else
				{
					temp[loc] = 0;
					break;
				}
			}
		}
		CString ret = "";
		if(strlen(temp) > 0 && tok == tokenNum)
			ret = temp;
		return ret;
	}

	int CImportDialog::GetFireRecFWSshort(char * buf, FILE * stream, CFireFilterDialog *ffd)
	{
		char temp[160];//
		fgets(temp, 159, stream);
		fplLineNo++;
		//check type first!
		bool goodRec = false;
		//char seps[] = "~", *p;
		//p = strtok(temp, seps);
		CString str = TokenFromFWSShortBuf(temp, 4);
		//for(int i = 0; i < 3 && p; i++)
		//	p = strtok(NULL, seps );
		//if(!p || i != 3)
		if(str.GetLength() <= 0)
			return 0;
		//strncpy(buf, p, 2);
		//buf[2] = 0;
		switch(atoi(str))
		{
		case 11:
			if(ffd->type11)
				goodRec = true;
			break;
		case 12:
			if(ffd->type12)
				goodRec = true;
			break;
		case 13:
			if(ffd->type13)
				goodRec = true;
			break;
		case 14:
			if(ffd->type14)
				goodRec = true;
			break;
		case 15:
			if(ffd->type15)
				goodRec = true;
			break;
		case 16:
			if(ffd->type16)
				goodRec = true;
			break;
		case 19:
			if(ffd->type19)
				goodRec = true;
			break;
		case 21:
			if(ffd->type21)
				goodRec = true;
			break;
		case 22:
			if(ffd->type22)
				goodRec = true;
			break;
		case 23:
			if(ffd->type23)
				goodRec = true;
			break;
		case 25:
			if(ffd->type25)
				goodRec = true;
			break;
		case 26:
			if(ffd->type26)
				goodRec = true;
			break;
		case 37:
			if(ffd->type37)
				goodRec = true;
			break;
		case 48:
			if(ffd->type48)
				goodRec = true;
			break;
		case 49:
			if(ffd->type49)
				goodRec = true;
			break;
		}
		if(!goodRec)
			return 0;
		strcpy_s(buf, 159, temp);//unitid

		return 1;
	}

	int ParseFWSShortBuf(char *buf, CString& strUnit, CString& strName, CString& strFireNum, CString& strSubtype,
		CString& strCause, CString& strDate, double& lat, double& lon, double& acres)
	{
		CString temp;
		//should be eight ~s
		strUnit = TokenFromFWSShortBuf(buf, 0);
		strName = TokenFromFWSShortBuf(buf, 2);
		strFireNum = TokenFromFWSShortBuf(buf, 3);
		strSubtype = TokenFromFWSShortBuf(buf, 4);
		strCause = TokenFromFWSShortBuf(buf, 5);
		strDate = TokenFromFWSShortBuf(buf, 6);
		temp = TokenFromFWSShortBuf(buf, 7);
		lat = atof(temp);
		temp = TokenFromFWSShortBuf(buf, 8);
		lon = atof(temp);
		temp = TokenFromFWSShortBuf(buf, 9);
		acres = atof(temp);
		//if(strUnit.GetLength() <= 0 || strFireNum.GetLength() <= 0 || strCause.GetLength() <= 0
		//	|| strDate.GetLength() <= 0)
		//	return -1;
		return 0;
	}

	int CImportDialog::WriteFireRecFWSshort(char *buf, FILE *errLog, CFiresSet *tFires)
	{
		int ret = 0;
		bool append = true;
		char temp[16];
		//int m, d, y;
		long regID, unitID;//, fireCause;
		double lat, lon, acres;
		CString str, strFireName, strFireNum, strUnit, strUnitID, strSubtype, strCause, strDate;
		//char newUnitStr[64];
		COleDateTime dt;
		utils util(firesSet.m_pDatabase);

		int fail = ParseFWSShortBuf(buf, strUnit, strFireName, strFireNum, strSubtype,
			strCause, strDate, lat, lon, acres);
		if(!fail)
		{
			if(strUnit.GetLength() <= 0)
			{
				fprintf(errLog, "\tRecord Number %ld, could not read UNIT\n", fplLineNo);
				return 2;
			}
			if(strFireNum.GetLength() <= 0)
			{
				fprintf(errLog, "\tRecord Number %ld, could not read FIRENUM\n", fplLineNo);
				return 2;
			}
			if(strCause.GetLength() <= 0)
			{
				fprintf(errLog, "\tRecord Number %ld, could not read IGNITIONCAUSE\n", fplLineNo);
				return 2;
			}
			if(strDate.GetLength() > 0)
			{
				dt.ParseDateTime(strDate);
				if(dt.GetStatus() != COleDateTime::valid)
				{
					fprintf(errLog, "\tRecord Number %ld, invalid date\n", fplLineNo);
					return -1;
				}
				COleDateTime today;
				today = COleDateTime::GetCurrentTime();
				if(dt > today)
				{
					fprintf(errLog, "\tRecord Number %ld, Date is in the furure!\n",
						fplLineNo);
					return -1;
				}
			}
			else
			{
				fprintf(errLog, "\tRecord Number %ld, could not read DISCOVERYDATETIME\n", fplLineNo);
				return 2;
			}
			if(lat < -90.0 || lat > 90.0)
			{
				fprintf(errLog, "\tRecord Number %ld, invalid LATITUDE: %f\n", fplLineNo, lat);
				return 2;
			}
			if(lon < -360.0 || lon > 360.0)
			{
				fprintf(errLog, "\tRecord Number %ld, invalid LONGITUDE: %f\n", fplLineNo, lon);
				return 2;
			}
			if(acres < 0.0)
			{
				fprintf(errLog, "\tRecord Number %ld, invalid TOTALACRES: %f\n", fplLineNo, acres);
				return 2;
			}
		}

		//we got here, write values to tempfires
		//check if record already exists
		//agency == 1, year + reg + unit + firenum is unique
		sprintf(temp, "%c0000", strUnit[0]);
		regID = GetFireRegion(&regs, 5, temp);
		if(regID == 0)
		{
			fprintf(errLog, "\tRecord Number %ld, %s: '%s'\n", fplLineNo, errStrings[2], temp);
			return -2;
		}
		//strncpy(temp, &buf[1], 5);
		//				temp[5] = 0;
		strcpy_s(temp, strUnit);
		unitID = GetFireUnit(&units, 5, regID, temp);
		if(unitID == 0)
		{
			unitID = AddFireUnit(&units, 5, regID, temp);
			fprintf(errLog, "\tRecord Number %ld, Added New Unit: '%5s Unknown'\n", fplLineNo, temp);
			nNewUnits++;
		}
		tFires->AddNew();
		//tFires->m_FireID = ++(*fireID);
		tFires->m_FireNumber = strFireNum;
		tFires->m_AgencyID = 5;
		tFires->m_RegionID = regID;
		tFires->m_UnitID = unitID;
		tFires->SetFieldNull(&tFires->m_SubunitID);
		//CTime tTime(dt.GetYear(), dt.GetMonth(), dt.GetDay(), dt.GetHour(), dt.GetMinute(), 0);
		tFires->m_Discovery = dt;
		str.Format("%d", dt.GetYear());
		//tFires->m_Year = str;
		tFires->m_latitude = lat;
		tFires->m_longitude = lon;
		/*int dd, mm, ss;
		DecimalDegree2DDMMSS(lat, &dd, &mm, &ss);
		tFires->m_LatDD = dd;
		tFires->m_LatMM = mm;
		tFires->m_LatMM = ss;
		DecimalDegree2DDMMSS(lon, &dd, &mm, &ss);
		tFires->m_LonDD = dd;
		tFires->m_LonMM = mm;
		tFires->m_LonMM = ss;*/
		/*	tFires->m_LatDD = (long)lat;
		tFires->m_LatMM = (long) (lat - tFires->m_LatDD) * 60.0;
		tFires->m_LatSS = (long) (((lat - tFires->m_LatDD) * 60.0) - tFires->m_LatMM) * 60.0;
		tFires->m_LonDD = (long)lon;
		tFires->m_LonMM = (long) (lon - tFires->m_LonDD) * 60.0;
		tFires->m_LonSS = (long) (((lon - tFires->m_LonDD) * 60.0)  - tFires->m_LonMM) * 60.0;*/
		tFires->m_TotalAcres = acres;
		strCause.TrimRight();
		if(strCause.GetLength() > 0)
		{
			int causeVal = atoi(strCause);
			bool validCause = util.validateStatCause(causeVal);
			if (validCause)
			   tFires->m_StatisticalCause = atol(strCause);
			else
				{
				fprintf(errLog, "\tRecord Number %ld, invalid statistical cause: %s\n", fplLineNo, strCause);
				return 2;
			   }
		} else
			tFires->m_StatisticalCause = 1;//assume lightning
		//tFires->SetFieldNull(&tFires->m_SpecificCause);
		if(strFireName.GetLength() > 0)
			tFires->m_FireName = strFireName.Trim();
		else
			tFires->SetFieldNull(&tFires->m_FireName);
		/*tFires->SetFieldNull(&tFires->m_Township);
		tFires->SetFieldNull(&tFires->m_Range);
		tFires->SetFieldNull(&tFires->m_Section);
		tFires->SetFieldNull(&tFires->m_State);
		tFires->SetFieldNull(&tFires->m_SubSection);
		tFires->SetFieldNull(&tFires->m_County);
		tFires->SetFieldNull(&tFires->m_Ignition);
		tFires->SetFieldNull(&tFires->m_FirstAttack);
		tFires->SetFieldNull(&tFires->m_Reinforcement);
		tFires->SetFieldNull(&tFires->m_DeclareWildlandFire);*/
		tFires->SetFieldNull(&tFires->m_Contain);
		tFires->SetFieldNull(&tFires->m_StrategyMet);
		/*tFires->SetFieldNull(&tFires->m_FireOut);
		tFires->SetFieldNull(&tFires->m_WildNum);
		tFires->SetFieldNull(&tFires->m_GeneralCause);
		tFires->SetFieldNull(&tFires->m_ClassPeople);
		tFires->SetFieldNull(&tFires->m_SizeClass);

		// new FFP4 fields   05/2006

		tFires->SetFieldNull(&tFires->m_Slope);
		tFires->SetFieldNull(&tFires->m_Elevation);
		tFires->SetFieldNull(&tFires->m_Aspect);
		tFires->SetFieldNull(&tFires->m_FuelModel);
		tFires->SetFieldNull(&tFires->m_FireType);*/

		tFires->Update();
		return 0;//success
	}

	// check for null values, especially from WRCC files

	int CImportDialog::IsNullValue(char *value){
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

	void CImportDialog::OnBnClickedFw21()
	{
		CFW21ImportDlg dlg(this); 
		dlg.pDoc = pDoc;
		dlg.DoModal();
	}


	void CImportDialog::OnBnClickedInformsCsv()
	{
		CFileDialog	dlg(TRUE, _T("InFORMS Fires"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
			OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
			_T("InFORMS Fire Files (*.csv)|*.csv|All Files (*.*)|*.*||"), this);
		char* mfBuf = new char[MF_BUFSIZE];
		mfBuf[0] = 0;
		dlg.m_ofn.lpstrFile = mfBuf;
		dlg.m_ofn.nMaxFile = MF_BUFSIZE;
		int res = dlg.DoModal();

		if (res == IDOK)
		{
			CWaitCursor wait;
			long errors = 0;
			// Get the position of the first filename
			char* logFileName = GetTempFileName();
			FILE* logFile = fopen(logFileName, "wt");
			COleDateTime dateTime = COleDateTime::GetCurrentTime();
			fprintf(logFile, "FireFamily Plus InFORMS Import Log\nStarted: %s\n\n", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
			POSITION filenamePosition = dlg.GetStartPosition();
			CString fileName;

			// Open each file selected
			int tErrors;
			bool didSomething = false;
			while (filenamePosition != NULL)
			{
				fileName = dlg.GetNextPathName(filenamePosition);
				//tErrors = ImportGenericWx(fileName, logFile);
				tErrors = ImportInFORMS(fileName, logFile);


				if (tErrors >= 0)
				{
					errors += tErrors;
					didSomething = true;
				}
				else
				{
					//user cancelled
					//errors = -1;
					break;
				}

			}
			//if(errors >= 0)
			//{
			if (!didSomething)//res == IDCANCEL)
			{
				fclose(logFile);
				delete[] logFileName;
			}
			else {
				dateTime = COleDateTime::GetCurrentTime();
				CString strMsg;
				if (errors >= 0)
				{
					fprintf(logFile, "\nImport Completed: %s\n%ld errors were encountered.", dateTime.Format("%m/%d/%Y at %H:%M:%S"), errors);
					strMsg.Format("Import Complete.\n%ld errors were encountered.", errors);
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
				theApp.SetUserDir(dlg.GetPathName());
			}
		}
		else if (res == IDCANCEL)
		{
			int err = CommDlgExtendedError();
			if (err == FNERR_BUFFERTOOSMALL)
			{
				AfxMessageBox("File Name buffer overrun, select fewer files.");
			}
		}
		delete[] mfBuf;
		chdir(theApp.dbDir);
	}

	//bool regIsEqual(const std::pair<string, int>& element)
	//{
	//	return element.first == User.regionID;
	//}

	std::vector<string> informsFields =
	{
		"UniqueFireIdentifier", 
		"FireDiscoveryDateTime", 
		"ContainmentDateTime", 
		"ControlDateTime",
		"IncidentName",
		"IncidentSize", //5
		"Latitude",
		"Longitude",
		"FORID",
		"RelatedIrwinIDs", 
		"Fire Cause", //10
		"GACC",
		"POOProtectingUnit",
		//"Fire Cause General", "Fire Cause Specific", 
		"Status", 
		"IncidentTypeCategory",
		"Fire Cause General"
	};

	int TranslateInFORMSCause(CString fireCause, CString generalCause)
	{
		if (fireCause.CompareNoCase("Natural") == 0)
			return 1;//lightning
		if(fireCause.CompareNoCase("Undetermined") == 0)
			return 9;//Misc, verified with Matt
		if (fireCause.CompareNoCase("Human") == 0)
		{
			if (generalCause.CompareNoCase("Debris and open burning") == 0)
				return 5;
			if (generalCause.CompareNoCase("Equipment and vehicle use") == 0)
				return 2;
			if (generalCause.CompareNoCase("Misuse of fire by a minor") == 0)
				return 8;
			if (generalCause.CompareNoCase("Smoking") == 0)
				return 3;
			if (generalCause.CompareNoCase("Arson") == 0)
				return 7;
			if (generalCause.CompareNoCase("Railroad operations and maintenance") == 0)
				return 6;
			if (generalCause.CompareNoCase("Recreation and ceremony") == 0)
				return 4;
		}
		return 9;
	}

	int CImportDialog::ImportInFORMS(CString fileName, FILE* errLog)
	{
		long defaultRegion = -1, defaultUnit = -1, defaultSubunit = -1;


		//long errors = 0;
		int nFires = 0;


		fprintf(errLog, "**************************************************\nImport File Name: %s\n", fileName);
		EnableWindow(FALSE);
		if (pDoc->HasTempFire())
		{
			try
			{
				pDoc->m_pDB->ExecuteSQL("DROP TABLE [ffpTempFire]");
			}
			catch (CDBException* e)
			{
				e->Delete();
			}
		}
		pDoc->CreateTempFire();

		ifstream stream;
		stream.open(fileName);
		if (!stream.is_open())
		{
			fprintf(errLog, "Error opening %s as input\n", fileName);
			return -1;
		}
		const int bufSize = 3000;
		char buf[bufSize];
		int fireNumberIdx, discoveryIdx, containIdx, strategyMetIdx, fireNameIdx, totalAcresIdx, latitudeIdx, longitudeIdx,
			foridIdx, irwinidIdx, statisticalCauseIdx, gaccIdx, pooProtectingUnitIdx, statusIdx, typeIdx, generalCauseIdx;
		stream.getline(buf, bufSize);
		string line = buf;
		vector<string> vFields = csv_read_row(line, ',');
		fireNumberIdx = getColIndex(informsFields[0], vFields);
		discoveryIdx = getColIndex(informsFields[1], vFields);
		containIdx = getColIndex(informsFields[2], vFields);
		strategyMetIdx = getColIndex(informsFields[3], vFields);
		fireNameIdx = getColIndex(informsFields[4], vFields);
		totalAcresIdx = getColIndex(informsFields[5], vFields);
		latitudeIdx = getColIndex(informsFields[6], vFields);
		longitudeIdx = getColIndex(informsFields[7], vFields);
		foridIdx = getColIndex(informsFields[8], vFields);
		irwinidIdx = getColIndex(informsFields[9], vFields);
		statisticalCauseIdx = getColIndex(informsFields[10], vFields);
		generalCauseIdx = getColIndex(informsFields[15], vFields);
		gaccIdx = getColIndex(informsFields[11], vFields);
		pooProtectingUnitIdx = getColIndex(informsFields[12], vFields);
		statusIdx = getColIndex(informsFields[13], vFields);
		typeIdx = getColIndex(informsFields[14], vFields);
		int nErrors = 0;
		if (fireNumberIdx < 0 || discoveryIdx < 0 || containIdx < 0 || strategyMetIdx < 0 || fireNameIdx < 0
			|| totalAcresIdx < 0 || latitudeIdx < 0 || longitudeIdx < 0 || foridIdx < 0 || irwinidIdx < 0
			|| statisticalCauseIdx < 0 || gaccIdx < 0 || pooProtectingUnitIdx < 0 || statusIdx < 0 || typeIdx < 0)
		{
			if (fireNumberIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[0]);
				nErrors++;
			}
			if (discoveryIdx < 0)
			{
				nErrors++;
			}
			fprintf(errLog, "Error, field %s not found in header\n", vFields[1]);
			if (containIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[2]);
				nErrors++;
			}
			if (strategyMetIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[3]);
				nErrors++;
			}
			if (fireNameIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[4]);
				nErrors++;
			}
			if (totalAcresIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[5]);
				nErrors++;
			}
			if (latitudeIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[6]);
				nErrors++;
			}
			if (longitudeIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[7]);
				nErrors++;
			}
			if (foridIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[8]);
				nErrors++;
			}
			if (irwinidIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[9]);
				nErrors++;
			}
			if (statisticalCauseIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[10]);
				nErrors++;
			}
			if (generalCauseIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[15]);
				nErrors++;
			}
			if (gaccIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[11]);
				nErrors++;
			}
			if (pooProtectingUnitIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[12]);
				nErrors++;
			}
			if (statusIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[13]);
				nErrors++;
			}
			if (typeIdx < 0)
			{
				fprintf(errLog, "Error, field %s not found in header\n", vFields[14]);
				nErrors++;
			}
			stream.close();
			return -2;
		}
		int maxIdx = fireNumberIdx;
		maxIdx = max(maxIdx, discoveryIdx);
		maxIdx = max(maxIdx, containIdx);
		maxIdx = max(maxIdx, strategyMetIdx);
		maxIdx = max(maxIdx, fireNameIdx);
		maxIdx = max(maxIdx, totalAcresIdx);
		maxIdx = max(maxIdx, latitudeIdx);
		maxIdx = max(maxIdx, longitudeIdx);
		maxIdx = max(maxIdx, foridIdx);
		maxIdx = max(maxIdx, irwinidIdx);
		maxIdx = max(maxIdx, statisticalCauseIdx);
		maxIdx = max(maxIdx, gaccIdx);
		maxIdx = max(maxIdx, pooProtectingUnitIdx);
		maxIdx = max(maxIdx, statusIdx);
		maxIdx = max(maxIdx, typeIdx);
		maxIdx = max(maxIdx, generalCauseIdx);

		vector<std::pair<int, string>> vGaccRegionIds;
		CFireRegionSet regionsSet(firesSet.m_pDatabase);
		regionsSet.m_strFilter.Format("[AgencyID] = %d", m_GACC_Agency_ID);
		regionsSet.Open();
		while (!regionsSet.IsEOF())
		{
			vGaccRegionIds.push_back(std::pair<int, string>(regionsSet.m_RegionID, regionsSet.m_Name));
			regionsSet.MoveNext();
		}
		regionsSet.Close();

		//unordered_multimap <int, int> mapRegionUnits;
		struct RegionUnit
		{
			int regionID;
			string name;
			int unitID;
			bool operator<(const RegionUnit& rhs) const
			{
				return tie(regionID, name, unitID) < tie(rhs.regionID, rhs.name, rhs.unitID);
			}
			bool operator>(const RegionUnit& rhs) const
			{
				return tie(regionID, name, unitID) > tie(rhs.regionID, rhs.name, rhs.unitID);
			}
			bool operator==(const RegionUnit& rhs) const
			{
				return tie(regionID, name, unitID) == tie(rhs.regionID, rhs.name, rhs.unitID);
			}
		};
		vector < RegionUnit> vNewUnitIds;
		std::set<RegionUnit> regionUnitSet;
		//unordered_multimap <int, std::pair<string, int>> mapRegionUnits;
		CFireUnitSet unitSet(firesSet.m_pDatabase); 
		//unitSet.m_strFilter.Format("[RegionID] = %d", m_GA);
		unitSet.Open();
		while (!unitSet.IsEOF())
		{
			int thisReg = unitSet.m_RegionID;
			//vGaccRegionIds.
			if (std::find_if(vGaccRegionIds.begin(), vGaccRegionIds.end(), [&](const auto& pair) { return pair.first == thisReg; }) != vGaccRegionIds.end())
			{
				RegionUnit thisUnit{ unitSet.m_RegionID, unitSet.m_Name, unitSet.m_UnitID };			
				regionUnitSet.insert(thisUnit);
			}
			unitSet.MoveNext();
				//mapRegionUnits.emplace(thisReg, std::pair<string, int>(unitSet.m_Name, unitSet.m_UnitID));
		}
		int nextUnitID = 1;
		unitSet.m_strSort.Format("[UnitID]");
		unitSet.Requery();
		unitSet.MoveLast();
		if (!unitSet.IsEOF())
			nextUnitID = unitSet.m_UnitID + 1;
		unitSet.Close();

		CString fireNumber, status, statCause, discoveryStr, containStr, strategyMetStr,
			fireName, acresStr, latStr, lonStr, foridStr, irwinIdStr, gaccStr, unitStr, typeStr, generalCauseStr;

		//field headers ok, start processing records
		int lineNo = 1;
		int nDuplicates = 0;
		CFiresSet tFires(firesSet.m_pDatabase);
		tFires.Open(CRecordset::dynaset, "ffpTempFire");
		while (stream.good())
		{
			stream.getline(buf, bufSize);
			lineNo++;
			line = buf;
			vFields = csv_read_row(line, ',');
			if (vFields.size() < maxIdx)//something wrong, probably a multiline record
			{
				//fprintf(errLog, "Error, too few fields in line %d\n", lineNo);
				continue;
			}
			fireNumber = vFields[fireNumberIdx].c_str();
			status = vFields[statusIdx].c_str();
			statCause = vFields[statisticalCauseIdx].c_str();
			discoveryStr = vFields[discoveryIdx].c_str();
			containStr = vFields[containIdx].c_str();
			strategyMetStr = vFields[strategyMetIdx].c_str();
			fireName = vFields[fireNameIdx].c_str();
			acresStr = vFields[totalAcresIdx].c_str();
			latStr = vFields[latitudeIdx].c_str();
			lonStr = vFields[longitudeIdx].c_str();
			foridStr = vFields[foridIdx].c_str();
			irwinIdStr = vFields[irwinidIdx].c_str();
			if ( irwinIdStr.GetLength() > 38)
			{
				CString tStr = irwinIdStr;
				irwinIdStr = tStr.Left(38);
			}
			gaccStr = vFields[gaccIdx].c_str();
			unitStr = vFields[pooProtectingUnitIdx].c_str();
			typeStr = vFields[typeIdx].c_str();
			generalCauseStr = vFields[generalCauseIdx].c_str();
			if ((typeStr.CompareNoCase("WF") != 0  && typeStr.CompareNoCase("CX") != 0) || (status.CompareNoCase("Complete") != 0 && status.CompareNoCase("Certified") != 0))
				continue;
			COleDateTime dtDisc, dtContain, dtStrategy;
			dtDisc.SetStatus(COleDateTime::invalid);
			dtContain.SetStatus(COleDateTime::invalid);
			dtStrategy.SetStatus(COleDateTime::invalid);
			if (discoveryStr.GetLength() > 0)
			{
				dtDisc.ParseDateTime(discoveryStr);
				if (dtDisc.GetStatus() != COleDateTime::valid)
				{
					fprintf(errLog, "Error, FireDiscoveryDateTime (%s) is invalid, record %d\n", discoveryStr, lineNo);
					nErrors++;
					continue;
				}
			}
			else
			{
				fprintf(errLog, "Error, FireDiscoveryDateTime is blank, record %d\n", lineNo);
				nErrors++;
				continue;
			}
			if (containStr.GetLength() > 0)
			{
				dtContain.ParseDateTime(containStr);
				if (dtContain.GetStatus() != COleDateTime::valid)
				{
					fprintf(errLog, "Warning, ContainmentDateTime (%s) is invalid, record %d\n", containStr, lineNo);
				}
			}
			if (strategyMetStr.GetLength() > 0)
			{
				dtStrategy.ParseDateTime(strategyMetStr);
				if (dtStrategy.GetStatus() != COleDateTime::valid)
				{
					fprintf(errLog, "Warning, ControlDateTime (%s) is invalid, record %d\n", strategyMetStr, lineNo);
				}
			}
			double acres = atof(acresStr.GetBuffer());
			if (acres <= 0.0)
				acres = 0.1;
			double latitude = 0.0, longitude = 0.0;
			if (latStr.GetLength() > 0)
				latitude = atof(latStr.GetBuffer());
			if (lonStr.GetLength() > 0.0)
				longitude = atof(lonStr.GetBuffer());
			if (gaccStr.GetLength() <= 0)
			{
				fprintf(errLog, "Error, GACC is blank, record %d\n", lineNo);
				nErrors++;
				continue;
			}
			if (unitStr.GetLength() <= 0)
			{
				fprintf(errLog, "Error, POOProtectingUnit is blank, record %d\n", lineNo);
				nErrors++;
				continue;
			}
			//now, see if Unit is in existing data
			//find the regionID
			int regID = -1;
			auto index = std::distance(vGaccRegionIds.begin(), std::find_if(vGaccRegionIds.begin(), vGaccRegionIds.end(), [&](const auto& pair) { return pair.second == gaccStr.GetBuffer(); }));
			if(index >= 0 && index < vGaccRegionIds.size())
				regID = vGaccRegionIds[index].first;
			else
			{
				fprintf(errLog, "Error, GACC %s is unrecognized, record %d\n", gaccStr.GetBuffer(), lineNo);
				nErrors++;
				continue;
			}
			//check to see if unit exists for this region
			int unitID = -1;
			auto it = regionUnitSet.begin();
			for (; it != regionUnitSet.end(); ++it)
			{
				if ((*it).regionID == regID && (*it).name.compare(unitStr.GetBuffer()) == 0)
					break;
			}
			if (it != regionUnitSet.end())
			{
				unitID = (*it).unitID;
			}
			else
			{//need to add unit to new vNewUnitIds and regionUnitSet
				//vNewUnitIds.push_back(std::pair<string, int>(unitStr.GetBuffer(), nextUnitID));
				RegionUnit thisUnit{ regID, unitStr.GetBuffer(), nextUnitID };
				vNewUnitIds.push_back(thisUnit);
				regionUnitSet.emplace(thisUnit);
				fprintf(errLog, "Added new FireUnit \"%s\" for FireRegion \"%s\", UnitID = %d\n", unitStr.GetBuffer(), gaccStr.GetBuffer(), nextUnitID);
				unitID = nextUnitID;
				nextUnitID++;
			}
			tFires.AddNew();
			tFires.m_AgencyID = m_GACC_Agency_ID;
			tFires.m_RegionID = regID;
			tFires.m_UnitID = unitID;
			tFires.SetFieldNull(&tFires.m_SubunitID);
			tFires.m_Discovery = dtDisc;
			if (dtContain.GetStatus() == COleDateTime::valid)
				tFires.m_Contain = dtContain;
			else
				tFires.SetFieldNull(&tFires.m_Contain);
			if(dtStrategy.GetStatus() == COleDateTime::valid)
				tFires.m_StrategyMet = dtStrategy;
			else
				tFires.SetFieldNull(&tFires.m_StrategyMet);
			tFires.m_FireName = fireName;
			tFires.m_FireNumber.Format("%d%02d%02d-%ld-%ld-%ld-0-%s", dtDisc.GetYear(), dtDisc.GetMonth(), dtDisc.GetDay(), m_GACC_Agency_ID, regID, unitID, fireNumber.GetBuffer());
			tFires.m_FORID = foridStr;
			tFires.m_IRWINID = irwinIdStr;
			tFires.m_latitude = latitude;
			tFires.m_longitude = longitude;
			tFires.m_TotalAcres = acres;
			tFires.m_StatisticalCause = TranslateInFORMSCause(statCause, generalCauseStr);
			tFires.Update();
			nFires++;
		}
		stream.close();

		//write the new units to the database

		CFireUnitSet newUnits(firesSet.m_pDatabase);
		newUnits.Open();
		for (auto it = vNewUnitIds.begin(); it != vNewUnitIds.end(); ++it)
		{
			newUnits.AddNew();
			newUnits.m_RegionID = (*it).regionID;
			newUnits.m_UnitID = (*it).unitID;
			newUnits.m_Name = (*it).name.c_str();
			newUnits.Update();
		}
		newUnits.Close();
		int updates = WriteTempFires(&tFires, m_GACC_Agency_ID);
		tFires.Close();
		fprintf(errLog, "\t%d errors encountered\n", nErrors);
		fprintf(errLog, "\t%d fires imported\n", nFires);
		fprintf(errLog, "\t%d fires updated\n", nFires);
		fprintf(errLog, "\t%d new FireUnits created\n", (int)vNewUnitIds.size());

		return nErrors;
	}

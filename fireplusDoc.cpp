// fireplusDoc.cpp : implementation of the CFireplusDoc class
//
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
//#include "datatbl.h"
//#include "datatbl6.h"
//#include "datatbl5.h"

#include "fireplusSet.h"
#include "fireplusDoc.h"
#include <direct.h>
#include <io.h>
#include <sys\stat.h>
#include "UserVarsDialog.h"
#include "UserImportDialog.h"
#include "ReportsSet.h"
#include "ReportOptionsSet.h"
#include "BatchOptionsSet.h"
#include "GraphSet.h"
#include "columnst.h"
#include "UserVarSet.h"
#include "SeasonBinSet.h"
#include "ClimateSet.h"
#include "WxSet.h"
#include "SIGStationSet.h"
#include "DOIUnitSet.h"
#include <oledb.h>
#include "msjetoledb.h"
#include "jetoledb.h"	// for IJetCompact interface
//#include "msjetoledb.h"	// for IJetCompact interface

#include "FireAgencySet.h"
#include "FireRegionSet.h"
#include "FireUnitSet.h"
#include "FireSubunitSet.h"
#include "fireset.h"
#include "fireassocset.h"

#include <atldbcli.h>
#include ".\fireplusdoc.h"
#include "RunsSet.h"
#include "RunClimateOptionsSet.h"
#include "RunDescriptionSet.h"
#include "RunFireAssocSet.h"
#include "RunFireOptionsSet.h"
#include "RunOptionsSet.h"
#include "RunPocketCardSet.h"
#include "RunWxStationSet.h"
#include "BatchRunsSet.h"
#include "BatchSet.h"
#include "CustomInitSet.h"
#include "FFPOptsSet.h"
#include "FFPOverlaysSet.h"
#include "FFPMergesSet.h"
#include "FFPHourlyEventsSet.h"
#include "FFPDailyEventsSet.h"
#include "LFISet.h"
#include "FFPfxOptsSet.h"
#include "MainFrm.h"
#include "TermPercentilesSet.h"
#include "SowThresholdsSet.h"
#include "DOICauseSet.h"
#include "FireSumSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CFireplusApp theApp;
int curr_runID = 1;
int curr_batchID = 1;

bool hasLFI;

/////////////////////////////////////////////////////////////////////////////
// CFireplusDoc

IMPLEMENT_DYNCREATE(CFireplusDoc, CDocument)

BEGIN_MESSAGE_MAP(CFireplusDoc, CDocument)
	//{{AFX_MSG_MAP(CFireplusDoc)
	ON_COMMAND(ID_DATA_USERVARIABLES_MANAGE, OnDataUservariablesManage)
	ON_COMMAND(ID_DATA_USERVARIABLES_IMPORTDATA, OnDataUservariablesImportdata)
	ON_UPDATE_COMMAND_UI(ID_DATA_USERVARIABLES_IMPORTDATA, OnUpdateDataUservariablesImportdata)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_BATCH_ENABLERUNSAVING, OnUpdateBatchEnablerunsaving)
	ON_COMMAND(ID_BATCH_ENABLERUNSAVING, OnBatchEnablerunsaving)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFireplusDoc construction/destruction

CFireplusDoc::CFireplusDoc()
{
	// TODO: add one-time construction code here
	m_pDB = NULL;
	m_fireplusSet = NULL;
	m_bLocked = FALSE;
	doRunSaving = TRUE;
	docDir = "";
	CMainFrame *mainFrame = (CMainFrame *) AfxGetMainWnd();
	if (mainFrame && theApp)
		mainFrame->StatusBarMessage(theApp.getVarSortOrderStr(theApp.varSortOrder));
}

CFireplusDoc::~CFireplusDoc()
{
	if(m_fireplusSet)
	{
		if(m_fireplusSet->IsOpen())
		{
			//m_fireplusSet->Edit();
			//POSITION pos = GetFirstViewPosition();
			//m_view = (CFireplusView *)GetNextView(pos);

			//m_view->UpdateData();
			//m_view->UpdateData();
		//	m_fireplusSet->Update();
			m_fireplusSet->Close();
		}
		delete m_fireplusSet;
	}
	if(m_pDB)
	{
		if(m_pDB->IsOpen())
		{
			m_pDB->Close();
		}
		delete m_pDB;
	}
}

BOOL CFireplusDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CFireplusDoc serialization

void CFireplusDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFireplusDoc diagnostics

#ifdef _DEBUG
void CFireplusDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CFireplusDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFireplusDoc commands

BOOL CFireplusDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	// first check for read only files, and if so try to give write permission
	if(access(lpszPathName, 0) != 0)
	{
		CString msg;
		msg.Format("Error: File does not exist:\n%s", lpszPathName);
		AfxMessageBox(msg);
		return FALSE;
	}
	if(access(lpszPathName, 2) != 0) //no write permission
	{//try to change attributes
		if(chmod(lpszPathName, _S_IWRITE) == -1)
		{//couldn't do it
			CString msg;
			msg.Format("Error: Could not gain write access to the file:\n%s", lpszPathName);
			AfxMessageBox(msg);
			return FALSE;
		}
	}



	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	//docDir = lpszPathName;
	for(int c = docDir.GetLength() - 1; c >= 0; c--)
	{
		if(docDir.GetAt(c) != '\\')
			docDir.SetAt(c, 0);
		else
		{
			docDir.SetAt(c, 0);
			break;
		}
	}
	CDatabase* tmpDB = new CDatabase;

	CString strConnect;
	CString ext = strrchr(lpszPathName,'.');
	if(ext.CompareNoCase(".accdb") == 0)
		//strConnect.Format("Provider=Microsoft.ACE.OLEDB.12.0; Driver={Microsoft Access Driver (*.mdb,*.accdb)};DBQ=%s", lpszPathName);
		strConnect.Format("Driver={Microsoft Access Driver (*.mdb, *.accdb)};DSN='';DBQ=%s", lpszPathName);
	else//assume an mdb file
	{
		//strConnect.Format("Provider=MSDASQL;Driver={Microsoft Access Driver (*.mdb)};"
		//	"Dbq=%s;Uid=admin;Pwd=;", lpszPathName);
		strConnect.Format("Provider=MSDASQL;Driver={Microsoft Access Driver (*.mdb)};"
			"Dbq=%s;Uid=admin;Pwd=;", lpszPathName);
	}
	// Attempt to open the new database before replacing our ptr
	try
	{
		//tmpDB->Open(lpszPathName);
		int exRet = tmpDB->OpenEx(strConnect, CDatabase::noOdbcDialog);
	}
	catch (CDBException* e)
	{
		//if(e->m_pErrorInfo)
		//{
			DisplayDBException(e);
			delete tmpDB;
		//	e->Delete();
			//CDocument::OnCloseDocument();
			return FALSE;
	//	}
	//	e->Delete();
	}

	if (m_pDB)
	{
		if (m_pDB->IsOpen())
			m_pDB->Close();
		delete m_pDB;
	}
	//char wd[256];
	docDir = tmpDB->GetDatabaseName();
	for(int c = docDir.GetLength() - 1; c >= 0; c--)
	{
		if(docDir.GetAt(c) != '\\')
			docDir.SetAt(c, 0);
		else
		{
			docDir.SetAt(c, 0);
			break;
		}
	}
	theApp.SetUserDir(tmpDB->GetDatabaseName());
	//chdir(theApp.workDir);
	theApp.dbDir = docDir;
	chdir(theApp.dbDir);
	m_pDB = tmpDB;
	// added for batch - integrated with existing code
	bool hasUserVars = false;
	bool hasRunDescription = false;
	bool hasRunClimateOptions = false;
	bool hasRunFireAssociations = false;
	bool hasRunFireOptions = false;
	bool hasRunWxStation = false;
	bool hasRuns = false;
	hasLFI = false;
	bool hasReports = false;
	bool hasRunOptions = false;
	bool hasReportOptions = false;
	bool hasBatch = false;
	bool hasBatchRuns = false;
	bool hasBatchOptions = false;
    bool hasRefDOIUnits = false;
	bool hasRefDOICause = false;
	bool hasffpFireRisk = false;
	bool hasffpInit = false;
	bool hasffpRunPocketCard = false;
	bool hasffpGenericFireImports = false;
	bool hasffpGenericFireFields = false;
	bool hasffpGenericWxImports = false;
	bool hasffpGenericWxFields = false;
	bool hasfppOpts = false;
	bool hasffpOverlays = false;
	bool hasffpMerges = false;
	bool hasffpHourlyEvents = false;
	bool hasffpDailyEvents = false;
	bool hasffpCandidates = false;
	bool hasffpFxOpts = false;
	bool hasffpFxVars = false;
	bool hasffpTerm = false;
	bool hasffpTermDates = false;
	bool hasffpTermPercentiles = false;
	bool hasffpSowThresholds = false;
	bool hasffpSowOptions = false;
	///bool hasWxSet2016 = false;
	//bool hasffpNFDRS2016 = false;

	// 09/2012
	// trigger update of Wx Records - after database structure has been changed!
	updateWxObs = false;

	ffp4upgrade = false;  // if we have to add/modify tables to ffp4 standards, display alert
	ffp5upgrade = false;   //

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpSowOptions"))
		{
			hasffpSowOptions = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpSowThresholds"))
		{
			hasffpSowThresholds = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpTermPercentiles"))
		{
			hasffpTermPercentiles = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpTermDates"))
		{
			hasffpTermDates = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpTerm"))
		{
			hasffpTerm = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpFxOpts"))
	//	if(trecs.Open(CRecordset::dynaset, "ffpUserVars"))
		{
			hasffpFxOpts = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpFxVars"))
	//	if(trecs.Open(CRecordset::dynaset, "ffpUserVars"))
		{
			hasffpFxVars = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpUserVars"))
	//	if(trecs.Open(CRecordset::dynaset, "ffpUserVars"))
		{
			hasUserVars = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpRunDescription"))
		{
			hasRunDescription = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpRunClimateOptions"))
		{
			hasRunClimateOptions = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpRunFireAssociations"))
		{
			hasRunFireAssociations = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpRunFireOptions"))
		{
			hasRunFireOptions = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpRunWxStation"))
		{
			hasRunWxStation = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpRuns"))
		{
			hasRuns = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpLFI"))
		{
			hasLFI = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpReports"))
		{
			hasReports = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpRunOptions"))
		{
			hasRunOptions = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpReportOptions"))
		{
			hasReportOptions = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpBatchRuns"))
		{
			hasBatchRuns = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpBatch"))
		{
			hasBatch = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{//ignore
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpBatchOptions"))
		{
			hasBatchOptions = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM refDOIUnits"))
		{
			hasRefDOIUnits = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM refDOICause"))
		{
			hasRefDOICause = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	//hasffpCandidates

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpCandidates"))
		{
			hasffpCandidates = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpFireRisk"))
		{
			hasffpFireRisk = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpInit"))
		{
			hasffpInit = true;
			trecs.Close();
			//SPECIAL CASE (inter-Beta release change), check for StartUpDay column field
			bool hasStartUpDay = false;

		
			// added 2015
			bool hasFFMC = false;
			bool hasDMC = false;
			bool hasDC = false;

			bool updateInit = false;

			try
			{
				CColumns columns(m_pDB);
				columns.m_strTableNameParam = "ffpInit";
				columns.Open();
				while (!columns.IsEOF())
				{
					if(columns.m_strColumnName.CompareNoCase("StartUpDay") == 0)
						hasStartUpDay = true;
					if(columns.m_strColumnName.CompareNoCase("FFMC") == 0)
						hasFFMC = true;
					if(columns.m_strColumnName.CompareNoCase("DMC") == 0)
						hasDMC = true;
					if(columns.m_strColumnName.CompareNoCase("DC") == 0)
						hasDC = true;
					columns.MoveNext();
				}
				columns.Close();
			}
			catch(CDBException* e)
			{//ignore
				e->Delete();
			}
			if(!hasStartUpDay)
			{
				updateInit = true;
				try
				{
					CString strSql;
					strSql = "ALTER TABLE [ffpInit] ADD [StartUpDay] DATETIME";
					m_pDB->ExecuteSQL(strSql);
				
				}
				catch(CDBException* e)
				{
					e->Delete();
				}
			}

			if(!hasFFMC)
			{
				updateInit = true;
				try
				{
					CString strSql;
					strSql = "ALTER TABLE [ffpInit] ADD [FFMC] NUMBER";
					m_pDB->ExecuteSQL(strSql);
				
				}
				catch(CDBException* e)
				{
					e->Delete();
				}
			}

			if(!hasDMC)
			{
				updateInit = true;
				try
				{
					CString strSql;
					strSql = "ALTER TABLE [ffpInit] ADD [DMC] NUMBER";
					m_pDB->ExecuteSQL(strSql);
				
				}
				catch(CDBException* e)
				{
					e->Delete();
				}
			}

			
			if(!hasDC)
			{
				updateInit = true;
				try
				{
					CString strSql;
					strSql = "ALTER TABLE [ffpInit] ADD [DC] NUMBER";
					m_pDB->ExecuteSQL(strSql);
				
				}
				catch(CDBException* e)
				{
					e->Delete();
				}
			}



			if (updateInit)
			{
				try
				{
					CCustomInitSet initSet(m_pDB);
					initSet.Open();
					while(!initSet.IsEOF())
					{
						initSet.Edit();
						initSet.m_StartUpDay = initSet.m_GreenJulian;
						initSet.m_FFMC = 0.0;
						initSet.m_DMC = 0.0;
						initSet.m_DC = 0.0;
						initSet.Update();
						initSet.MoveNext();
					}
						
				}
				catch(CDBException* e)
				{
					e->Delete();
				}

			}
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpRunPocketCard"))
		{
			hasffpRunPocketCard = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpGenericFireImports"))
		{
			hasffpGenericFireImports = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpGenericFireFields"))
		{
			hasffpGenericFireFields = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpGenericWxImports"))
		{
			hasffpGenericWxImports = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpGenericWxFields"))
		{
			hasffpGenericWxFields = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpOpts"))
		{
			hasfppOpts = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpOverlays"))
		{
			hasffpOverlays = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpMerges"))
		{
			hasffpMerges = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpHourlyEvents"))
		{
			hasffpHourlyEvents = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpDailyEvents"))
		{
			hasffpDailyEvents = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
			e->Delete();
	}

	
/*	try
	{
		CRecordset trecs(m_pDB);
		if (trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpNFDRS2016DFM"))
		{
			hasffpNFDRS2016 = true;
			trecs.Close();
		}
	}
	catch (CDBException* e)
	{
		e->Delete();
	}*/
	/*CDaoTableDefInfo tInfo;
	for(int i = 0; i < m_pDB->GetTableDefCount(); i++)
	{
		m_pDB->GetTableDefInfo(i, tInfo);

	    if(tInfo.m_strName.CompareNoCase("ffpUserVars") == 0)
			hasUserVars = true;
	    if(tInfo.m_strName.CompareNoCase("ffpRunDescription") == 0)
			hasRunDescription = true;
	    if(tInfo.m_strName.CompareNoCase("ffpRunClimateOptions") == 0)
			hasRunClimateOptions = true;
	    if(tInfo.m_strName.CompareNoCase("ffpRunFireAssociations") == 0)
			hasRunFireAssociations = true;
	    if(tInfo.m_strName.CompareNoCase("ffpRunFireOptions") == 0)
			hasRunFireOptions = true;
	    if(tInfo.m_strName.CompareNoCase("ffpRunWxStation") == 0)
			hasRunWxStation = true;
    	if(tInfo.m_strName.CompareNoCase("ffpRuns") == 0)
			hasRuns = true;
	    if(tInfo.m_strName.CompareNoCase("ffpReports") == 0)
			hasReports = true;
	    if(tInfo.m_strName.CompareNoCase("ffpRunOptions") == 0)
			hasRunOptions = true;
		if(tInfo.m_strName.CompareNoCase("ffpReportOptions") == 0)
			hasReportOptions = true;
		if(tInfo.m_strName.CompareNoCase("ffpBatchRuns") == 0)
			hasBatchRuns = true;
		if(tInfo.m_strName.CompareNoCase("ffpBatch") == 0)
			hasBatch = true;
		if(tInfo.m_strName.CompareNoCase("ffpBatchOptions") == 0)
			hasBatchOptions = true;
	}*/
	if(!hasffpSowOptions)
	{
		if(!this->CreateffpSowOptionsTable())
		{
			AfxMessageBox("Error: Could not create SowOptions table.");
		}
	}
	if(!hasffpSowThresholds)
	{
		if(!this->CreateffpSowThresholdsTable())
		{
			AfxMessageBox("Error: Could not create SowThresholds table.");
		}
	}
	if(!hasffpTerm)
	{
		if(!this->CreateffpTermTable())
		{
			AfxMessageBox("Error: Could not create Term table.");
		}
	}
	if(!hasffpTermDates)
	{
		if(!this->CreateffpTermDatesTable())
		{
			AfxMessageBox("Error: Could not create Term table.");
		}
	}
	if(!hasffpTermPercentiles)
	{
		if(!this->CreateffpTermPercentilesTable())
		{
			AfxMessageBox("Error: Could not create Term table.");
		}
	}
	if(!hasUserVars)
	{
		if(!CreateUserVarTable())
		{
			AfxMessageBox("Error: Could not create User Variable table.");
		}
	}
	if(!hasffpGenericFireImports)
	{
		if(!CreateGenericFireImportsTable())
		{
			AfxMessageBox("Error: Could not create GenericFireImports table.");
		}
	}
	if(!hasffpGenericFireFields)
	{
		if(!CreateGenericFireFieldsTable())
		{
			AfxMessageBox("Error: Could not create GenericFireFields table.");
		}
	}
	if(!hasffpGenericWxImports)
	{
		if(!CreateGenericWxImportsTable())
		{
			AfxMessageBox("Error: Could not create GenericWxImports table.");
		}
	}
	if(!hasffpGenericWxFields)
	{
		if(!CreateGenericWxFieldsTable())
		{
			AfxMessageBox("Error: Could not create GenericWxFields table.");
		//	return;
		}
	}
	if(!hasfppOpts)
	{
		if(!CreateffpOptsTable())
		{
			AfxMessageBox("Error: Could not create ffpOpts table.");
		}
	}
	if(!hasffpOverlays)
	{
		if(!CreateffpOverlaysTable())
		{
			AfxMessageBox("Error: Could not create ffpOverlays table.");
		}
	}
	if(!hasffpMerges)
	{
		if(!CreateffpMergesTable())
		{
			AfxMessageBox("Error: Could not create ffpMerges table.");
		}
	}
	if(!hasffpHourlyEvents)
	{
		if(!CreateffpHourlyEventsTable())
		{
			AfxMessageBox("Error: Could not create ffpHourlyEvents table.");
		}
	}
	if(!hasffpDailyEvents)
	{
		if(!CreateffpDailyEventsTable())
		{
			AfxMessageBox("Error: Could not create ffpDailyEvents table.");
		}
	}
	if(!hasffpCandidates)
	{
		if(!CreateffpCandidatesTable())
		{
			AfxMessageBox("Error: Could not create ffpCandidates table.");
		}
	}
	if(!hasffpFxOpts)
	{
		if(!CreateffpFxOptsTable())
		{
			AfxMessageBox("Error: Could not create ffpFxOpts table.");
		}
	}
	if(!hasffpFxVars)
	{
		if(!CreateffpFxVarsTable())
		{
			AfxMessageBox("Error: Could not create ffpFxVars table.");
		}
	}

    /* 8/2006 change
     make checks for ffp4upgrade here */
	if (! hasRunDescription)
		ffp4upgrade = true;
	if (! hasRunClimateOptions)
		ffp4upgrade = true;
    if(!hasRunFireAssociations)
	    ffp4upgrade = true;
    if(!hasRunFireOptions)
	    ffp4upgrade = true;
	if(!hasRunWxStation)
	    ffp4upgrade = true;
	if(!hasRuns)
        ffp4upgrade = true;
	if(!hasLFI)
        ffp4upgrade = true;
	if(!hasReports)
		ffp4upgrade = true;
	if(!hasRunOptions)
		ffp4upgrade = true;
	if(!hasReportOptions)
		ffp4upgrade = true;
	if(!hasBatch)
		ffp4upgrade = true;
	if(!hasBatchRuns)
		ffp4upgrade = true;
	if(!hasBatchOptions)
		ffp4upgrade = true;
    if(!hasRefDOIUnits)
		ffp4upgrade = true;
	
	if(!hasffpInit)
		ffp4upgrade = true;
	if(!hasffpRunPocketCard)
		ffp4upgrade = true;
	if(!hasffpCandidates)
		ffp4upgrade = true;
	CString v3_name;

	if (ffp4upgrade){
          CUpgradeDialog ud(this);
		  if(ud.DoModal() != IDOK){
			  return FALSE;
		  }
          // save original version
		  CString src;
		  src = m_pDB->GetDatabaseName();

		  v3_name.Format("%s_v3",src);
          src.Append(".mdb");
		  v3_name.Append(".mdb");

		  CopyFile(src,v3_name,false);
	}

	//CCancelDialog *cd = NULL;
	bool noCancel = true;// false;
	/*cd = new CCancelDialog(AfxGetMainWnd());

	if (cd != NULL)
	{
		BOOL ret = 0;// cd->Create(IDD_CANCELDIALOG);

      if (!ret)   //Create failed.
      {
         AfxMessageBox(_T("FFP will now perform database upgrade checks..."));
		 noCancel = true;
		 cd = 0;
      } else
	  {
	     cd->Reset();
         cd->ShowWindow(SW_SHOW);
	     cd->SetTitle("Checking v4 tables...");
	  }
	}*/
	

	if(!hasRunDescription)
	{
		if(!CreateffpRunDescriptionTable())
		{
			AfxMessageBox("Error: Could not create batch run Description table.");
		//	return;
		}
	}

	if(!hasRunClimateOptions)
	{
		if(!CreateffpRunClimateOptionsTable())
		{
			AfxMessageBox("Error: Could not create batch run Climate Options table.");
		//	return;
		}
	}
	if(!hasRunFireAssociations)
	{
		if(!CreateffpRunFireAssociationsTable())
		{
			AfxMessageBox("Error: Could not create batch run Fire Associations table.");
		//	return;
		}
	}
	if(!hasRunFireOptions)
	{
		if(!CreateffpRunFireOptionsTable())
		{
			AfxMessageBox("Error: Could not create batch run Fire Options table.");
		//	return;
		}
	}
	if(!hasRunWxStation)
	{
		if(!CreateffpRunWxStationTable())
		{
			AfxMessageBox("Error: Could not create batch run Wx Station table.");
		//	return;
		}
	}
	if(!hasRuns)
	{
		if(!CreateffpRunsTable())
		{
			AfxMessageBox("Error: Could not create batch run Runs table.");
		//	return;
		}
	}
	if(!hasLFI)
	{
		if(!CreateffpLFITable())
		{
			AfxMessageBox("Error: Could not create ffpLFI table.");
		//	return;
		}
	}
	if(!hasReports)
	{
		if(!CreateffpReportsTable())
		{
			AfxMessageBox("Error: Could not create Reports table.");
		//	return;
		}
	}
	if(!hasRunOptions)
	{
		if(!CreateffpRunOptionsTable())
		{
			AfxMessageBox("Error: Could not create batch run Run Options table.");
		//	return;
		}
	}

	if(!hasReportOptions)
	{
		if(!CreateffpReportOptionsTable())
		{
			AfxMessageBox("Error: Could not create batch run Report Options table.");
		//	return;
		}
	}

	if(!hasBatch)
	{
		if(!CreateffpBatchTable())
		{
			AfxMessageBox("Error: Could not create batch run Batch table.");
		//	return;
		}
	}

	if(!hasBatchRuns)
	{
		if(!CreateffpBatchRunsTable())
		{
			AfxMessageBox("Error: Could not create batch run BatchRuns table.");
		}
	}

	if(!hasBatchOptions)
	{
		if(!CreateffpBatchOptionsTable())
		{
			AfxMessageBox("Error: Could not create batch run BatchOptions table.");
		}
	}
	CheckFireTable();

    if (! hasRefDOIUnits)
	{
        if (! CreateRefDOIUnitsTable())
		{
			AfxMessageBox("Error: Could not create RefDOIUnits table.");
		}
	}

	 if (! hasRefDOICause)
	{
        if (! CreateRefDOICauseTable())
		{
			AfxMessageBox("Error: Could not create RefDOICause table.");
		}
	}

	if (! hasffpFireRisk)
	{
        if (! this->CreateffpFriskTable())
		{
			AfxMessageBox("Error: Could not create ffpFireRisk table.");
		}
	}

	if (! hasffpInit)
	{
        if (! this->CreateffpInitTable())
		{
			AfxMessageBox("Error: Could not create ffpInit table.");
		}
	}

	if (! hasffpRunPocketCard)
	{
		if (! this->CreateffpRunPocketCardTable())
		{
			AfxMessageBox("Error: Could not create ffpRunPocketCard table.");
		}
	}

	NewCheckClimateOptionsTable();
	CheckSeasonBinsTable();
	CheckGraphOptionsTable();
	CheckRunsTable();
	CheckBatchOptionsTable();
	CheckPocketCardTable();
	CheckFireOptionsTable();
	CheckWxObsTable();
	CheckWxStationTable();
	CheckffpRunWxStationTable();
	CheckffpRunClimateOptionsTable();
	CheckOptsTable();
	CheckLFITable();
	// end added for batch


	//the below section for adjusting for NASF fires was fubar.
	//we're going to try the trick of incrementing All agencies, region and units by a set amount (subunits not used by NASF)
	//then we will adjust fires as well (this was never done with the old way, thus losing link to all fires

	// new 2014/Oct:  add additional NASF units under 
		//  agency 6

	// first check for existing agency 6, and renumber accordingly
	bool hadNASF = true;
	CFireSet tFires(m_pDB);
	CFireAssocSet tAssoc(m_pDB);
	tFires.Open();
	tAssoc.Open();
			CFireAgencySet currAgency(m_pDB);
			currAgency.m_strFilter.Format("([AgencyID] = 6)");
			currAgency.Open();
			if (! currAgency.IsEOF())
			{
				if (currAgency.m_Name != "NASF") // renumber
				{
					hadNASF = false;
					

					// renumber region
					CFireRegionSet currReg(m_pDB);
					currReg.m_strFilter.Format("([AgencyID] = 6)");
					currReg.Open();
					while (! currReg.IsEOF()){
						// renumber units
						CFireUnitSet currUnit(m_pDB);
						currUnit.m_strFilter.Format("([RegionID] = %d)",currReg.m_RegionID);
						currUnit.Open();
						while (! currUnit.IsEOF()){

							// renumber subunit
							CFireSubunitSet currSub(m_pDB);
							currSub.m_strFilter.Format("([UnitID] = %d)",currUnit.m_UnitID);
							currSub.Open();
							while (! currSub.IsEOF()){
								currSub.Edit();
								currSub.m_UnitID = currSub.m_UnitID + 10000;
								
								currSub.Update();
								currSub.MoveNext();
							}
							//need to change Assoc and Fires UnitID as well
							tAssoc.m_strFilter.Format("[UnitID] = %d", currUnit.m_UnitID);
							tAssoc.Requery();
							while (!tAssoc.IsEOF())
							{
								tAssoc.Edit();
								tAssoc.m_UnitID = currUnit.m_UnitID + 10000;
								tAssoc.Update();
								tAssoc.MoveNext();
							}
							tFires.m_strFilter.Format("[UnitID] = %d", currUnit.m_UnitID);
							tFires.Requery();
							while (!tFires.IsEOF())
							{
								tFires.Edit();
								tFires.m_UnitID = currUnit.m_UnitID + 10000;
								tFires.Update();
								tFires.MoveNext();
							}
							//end Assoc and Fires for UnitID
							currUnit.Edit();
							currUnit.m_UnitID = currUnit.m_UnitID + 10000;
							currUnit.m_RegionID = currUnit.m_RegionID + 500;
							currUnit.Update();
							
							currSub.Close();
							currUnit.MoveNext();

						}
						//need to change Assoc and Fires RegionID as well
						tAssoc.m_strFilter.Format("[RegionID] = %d", currReg.m_RegionID);
						tAssoc.Requery();
						while (!tAssoc.IsEOF())
						{
							tAssoc.Edit();
							tAssoc.m_RegionID = currReg.m_RegionID + 500;
							tAssoc.Update();
							tAssoc.MoveNext();
						}
						tFires.m_strFilter.Format("[RegionID] = %d", currReg.m_RegionID);
						tFires.Requery();
						while (!tFires.IsEOF())
						{
							tFires.Edit();
							tFires.m_RegionID = currReg.m_RegionID + 500;
							tFires.Update();
							tFires.MoveNext();
						}
						//end Assoc and Fires for RegionID
						currUnit.Close();
						currReg.Edit();
						currReg.m_RegionID = currReg.m_RegionID + 500;
						currReg.m_AgencyID = 206;
						currReg.Update();
						currReg.MoveNext();
					}
					currReg.Close();
					currAgency.Edit();
					currAgency.m_AgencyID = 206;
					currAgency.Update();
					currAgency.Close();
					//need to change Assoc and Fires AgencyID as well
					tAssoc.m_strFilter.Format("[AgencyID] = 6");
					tAssoc.Requery();
					while (!tAssoc.IsEOF())
					{
						tAssoc.Edit();
						tAssoc.m_AgencyID = 206;
						tAssoc.Update();
						tAssoc.MoveNext();
					}
					tFires.m_strFilter.Format("[AgencyID] = 6");
					tFires.Requery();
					while (!tFires.IsEOF())
					{
						tFires.Edit();
						tFires.m_AgencyID = 206;
						tFires.Update();
						tFires.MoveNext();
					}
					//end Assoc and Fires

				}
				

			}
			tFires.Close();
			tAssoc.Close();


			


			//CFireUnitSet currFU(m_pDB);
		
			//currFU.m_strFilter.Format("[RegionID] >= 53");   // look for NASF units
			//currFU.Open();
			//if (!currFU.IsEOF())   
			//	currFU.Close();
			//else                    // only perform this step if we don't already have NASF agency units
			if(!hadNASF)
			{
				//currFU.Close();

				 CString master_path;
				master_path.Format("%s\\ffplus5.ffp",theApp.workDir);

			 if(access(master_path, 0) != 0)
				{
					CString msg;
					msg.Format("Error: Master Database does not exist:\n%s", master_path);
					AfxMessageBox(msg);
					return FALSE;
				}

				CDatabase masterDB;

				CString strConnect;
				strConnect.Format("Provider=MSDASQL;Driver={Microsoft Access Driver (*.mdb)};"
					"Dbq=%s;Uid=admin;Pwd=;", master_path);
				// Attempt to open the new database before replacing our ptr
				try
				{
					//tmpDB->Open(lpszPathName);
					masterDB.OpenEx(strConnect, CDatabase::noOdbcDialog);
				}
				catch (CDBException* e)
				{
		
					DisplayDBException(e);
			
					e->Delete();
					return FALSE;
	
				}

				CFireAgencySet masterFA(&masterDB);				
				CFireAgencySet newFA(m_pDB);				
				newFA.m_strFilter.Format("");
				newFA.Open();
			
				masterFA.m_strFilter.Format("[AgencyID] >= 6");   // add new NASF agency 6
				masterFA.Open();
			

			while (! masterFA.IsEOF()){
				newFA.AddNew();
				newFA.m_AgencyID = masterFA.m_AgencyID;
				newFA.m_Name = masterFA.m_Name;
				newFA.m_Permanent = masterFA.m_Permanent;	  
				newFA.Update();

				masterFA.MoveNext();
			}
			masterFA.Close();
			newFA.Close();


			CFireRegionSet masterFR(&masterDB);
			CFireRegionSet newFR(m_pDB);
			masterFR.m_strFilter.Format("[AgencyID] >= 6");   // add new NASF agency 6
			masterFR.Open();
			newFR.Open();

			while (! masterFR.IsEOF()){
				newFR.AddNew();
				newFR.m_AgencyID = masterFR.m_AgencyID;
				newFR.m_RegionID = masterFR.m_RegionID;
				newFR.m_Name = masterFR.m_Name;
		  
				newFR.Update();

				masterFR.MoveNext();
			}
			masterFR.Close();
			newFR.Close();


			
			CFireUnitSet newFU(m_pDB);
			CFireUnitSet masterFU(&masterDB);
			masterFU.m_strFilter.Format("[RegionID] >= 53");   // add new NASF agency 6
			masterFU.Open();
			newFU.Open();

			while (! masterFU.IsEOF()){
				newFU.AddNew();
				newFU.m_RegionID = masterFU.m_RegionID;
				newFU.m_UnitID = masterFU.m_UnitID;
			
				newFU.m_Name = masterFU.m_Name;
		  
				newFU.Update();

				masterFU.MoveNext();
			}
			masterFU.Close();
			newFU.Close();

	        masterDB.Close();

		}


	if (ffp4upgrade){
	    // recalc running 24 values (MinRH, MaxRH, MinTemp,MaxTemp,PPTAMT/PPTDUR/HourlyPrecip) in WxObs
        //if (cd)
		//   cd->Reset();
  
        CString tempstr;

		tempstr.Format("Updating Fires...");
		//if (cd)
		//   cd->SetStatusString(tempstr);
        CFireSet tempfire(m_pDB);
        tempfire.Open();
		int iFire = 0;
		CWaitCursor wait;

		// find number of records for progress bar
		int iNumFires = 0;
		while (! tempfire.IsEOF())
		{
           iNumFires++;
		   tempfire.MoveNext();
		}
		if (! tempfire.IsEOF())
		   tempfire.MoveFirst();

		int iStep = max(1,iNumFires / 10);

		while (! tempfire.IsEOF()){
			if (tempfire.m_GeneralCause != tempfire.m_StatisticalCause){
				iFire ++;
                 tempfire.Edit();
				 if (tempfire.m_GeneralCause > 0)
					 tempfire.m_StatisticalCause = tempfire.m_GeneralCause;
				 else
					 tempfire.m_GeneralCause = tempfire.m_StatisticalCause;
				 tempfire.Update();
				/* if (! (iFire % 10))
				 {
					 if (cd)
				        cd->Add(10);
				 }
				 if (! (iFire % iStep))
					 if (cd)
				        cd->Step();*/
			}
            tempfire.MoveNext();
		}
		tempfire.Close();

	}
	CFFPOptsSet ffpOpts(m_pDB);
	ffpOpts.Open();
	if(!ffpOpts.IsEOF())
	{
		//general
		doRunSaving = ffpOpts.m_RunSaveOn;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MaxMissWx))
			theApp.maxMissingWx = ffpOpts.m_MaxMissWx;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_PctWxVarID))
			theApp.rerapVarID = ffpOpts.m_PctWxVarID;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_CP1Color))
			theApp.m_OverallCP1Color = ffpOpts.m_CP1Color;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_CP1))
			theApp.m_OverallCP1 = ffpOpts.m_CP1;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_CP2Color))
			theApp.m_OverallCP2Color = ffpOpts.m_CP2Color;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_CP2))
			theApp.m_OverallCP2 = ffpOpts.m_CP2;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_LastGfxFmt))
			theApp.lastGraphicFormat = (GFile) ffpOpts.m_LastGfxFmt;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_EnableFuture))
			theApp.m_enableFuture = ffpOpts.m_EnableFuture;

		//overlays
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayYear1))
			theApp.overLayYears[0] = ffpOpts.m_OverlayYear1;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayYear2))
			theApp.overLayYears[1] = ffpOpts.m_OverlayYear2;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayYear3))
			theApp.overLayYears[2] = ffpOpts.m_OverlayYear3;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayWidth1))
			theApp.lineWidths[0] = ffpOpts.m_OverlayWidth1;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayWidth2))
			theApp.lineWidths[1] = ffpOpts.m_OverlayWidth2;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayWidth3))
			theApp.lineWidths[2] = ffpOpts.m_OverlayWidth3;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayStyle1))
			theApp.lineStyles[0] = ffpOpts.m_OverlayStyle1;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayStyle2))
			theApp.lineStyles[1] = ffpOpts.m_OverlayStyle2;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayStyle3))
			theApp.lineStyles[2] = ffpOpts.m_OverlayStyle3;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayColor1))
			theApp.overLayColors[0] = ffpOpts.m_OverlayColor1;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayColor2))
			theApp.overLayColors[1] = ffpOpts.m_OverlayColor2;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_OverlayColor3))
			theApp.overLayColors[2] = ffpOpts.m_OverlayColor3;

		//Merges
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeColor1))
			theApp.mergeColors[0] = ffpOpts.m_MergeColor1;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeColor2))
			theApp.mergeColors[1] = ffpOpts.m_MergeColor2;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeColor3))
			theApp.mergeColors[2] = ffpOpts.m_MergeColor3;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeColor4))
			theApp.mergeColors[3] = ffpOpts.m_MergeColor4;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeWidth1))
			theApp.mergeWidths[0] = ffpOpts.m_MergeWidth1;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeWidth2))
			theApp.mergeWidths[1] = ffpOpts.m_MergeWidth2;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeWidth3))
			theApp.mergeWidths[2] = ffpOpts.m_MergeWidth3;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeWidth4))
			theApp.mergeWidths[3] = ffpOpts.m_MergeWidth4;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeWidth1))
			theApp.mergeStyles[0] = ffpOpts.m_MergeStyle1;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeStyle2))
			theApp.mergeStyles[1] = ffpOpts.m_MergeStyle2;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeStyle3))
			theApp.mergeStyles[2] = ffpOpts.m_MergeStyle3;
		if(!ffpOpts.IsFieldNull(&ffpOpts.m_MergeStyle4))
			theApp.mergeStyles[3] = ffpOpts.m_MergeStyle4;

		if(!ffpOpts.IsFieldNull(&ffpOpts.m_VarSortOrder))
			theApp.varSortOrder = ffpOpts.m_VarSortOrder;
		// quick check for valid sort order
		if (theApp.varSortOrder > 2 || theApp.varSortOrder < 0)
		{
			theApp.varSortOrder = 0;
			ffpOpts.Edit();
			ffpOpts.m_VarSortOrder = 0;
			ffpOpts.Update();
		}
	}
	else
		doRunSaving = FALSE;
	ffpOpts.Close();
	//read daily events query rows

	// check paths
	CheckPaths();

	// update WxObs table if necessary
	// fix on [Hour] field

	if (updateWxObs){
		     int iWx = 0;
			 //if (cd)
             //   cd->Reset();
             CString tempstr;
		     tempstr.Format("Checking WxObs table...");
			// if (cd)
		     //   cd->SetStatusString(tempstr);
			 CWxSet wxSet(m_pDB);
			 wxSet.Open();
             CWaitCursor wait;
			 // find number of records for progress bar
		     int iNumRecs = 0;
			 if (! wxSet.IsEOF())
			 {
				 while (! wxSet.IsEOF())
				 {
					iNumRecs++;
					wxSet.MoveNext();
				 }
				 wxSet.MoveFirst();
			 
				int iStep = max(1,iNumRecs / 20);

				while(!wxSet.IsEOF())
				{
					iWx ++;
					/*if (! (iWx % 10))
						if (cd)
						  cd->Add(10);
					if (! (iWx % iStep))
						if (cd)
							cd->Step();*/

					wxSet.Edit();
					wxSet.m_Hour = wxSet.m_ObsDate.GetHour();
					wxSet.Update();
					wxSet.MoveNext();
				}

				wxSet.Close();
				setDailyObs();

			 } else
			wxSet.Close();

			
	}

	//read hourly events query rows
	

	//tempstr.Format("Performing run maintenance tasks...");
	//if (cd)
	//	  cd->SetStatusString(tempstr);
	DoRunMaintenance();
	m_fireplusSet = new CFireplusSet(m_pDB);
	//CDaoQueryDef qd(m_pDB);
	//qd.Create(NULL, "SELECT * FROM ffpDescription");
	
	// 2013: make sure ffpDescription.Description field is 100 chars max (upgraded from 50)
	CColumns columns(m_pDB);
	columns.m_strTableNameParam = "ffpDescription";
	columns.Open();
	bool desc100chars = false;
	while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("Description") == 0)
			{
				if (columns.m_lPrecision == 100)
					desc100chars = true;

			}
			columns.MoveNext();

		}
	columns.Close();

	if (! desc100chars)
	   m_pDB->ExecuteSQL("ALTER TABLE [ffpDescription] ALTER COLUMN Description TEXT(100)");
	
    m_fireplusSet->Open(CRecordset::dynaset, "SELECT * FROM ffpDescription");

	//RefreshViews();
    if (ffp4upgrade || ffp5upgrade)
	{
		
		try
		{
			CString strSql;
			strSql = "CREATE INDEX iyear ON Fire (Year)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		
		try
		{
			CString strSql;
			strSql = "CREATE INDEX iFireNumber ON Fire (FireNumber)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}

			try
		{
			CString strSql;
			strSql = "CREATE INDEX iStatisticalCause ON Fire (StatisticalCause)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}

		
	  
		 CString msg;
	     msg.Format("Updated database: %s\n\nThis database has been updated to FireFamilyPlus v5 ",m_pDB->GetDatabaseName());
	     AfxMessageBox(msg);


	}
/*	try
	{
		CString strSql;
		strSql = "CREATE INDEX iStationID ON WXObs (StationID)";
		m_pDB->ExecuteSQL(strSql);
	}
	catch (CDBException* e)
	{
		e->Delete();
	}
	try
	{
		CString strSql;
		strSql = "CREATE INDEX iObsDate ON WXObs (ObsDate)";
		m_pDB->ExecuteSQL(strSql);
	}
	catch (CDBException* e)
	{
		e->Delete();
	}
	try
	{
		CString strSql;
		strSql = "CREATE INDEX iStationIDObsDate ON WXObs (StationID, ObsDate)";
		m_pDB->ExecuteSQL(strSql);
	}
	catch (CDBException* e)
	{
		e->Delete();
	}
*/
/*	if (cd)
		{
			cd->DestroyWindow();
			cd = NULL;
		}*/

	return TRUE;
}

int CFireplusDoc::CreateRefDOIUnitsTable()
{
	CWaitCursor wait;
    try
	{
	CString sqlStr = "CREATE TABLE [refDOIUnits] ("
		                    "[WFMIcode] CHAR(6),"
							"[AgencyID] INTEGER,"
							"[RegionID] INTEGER,"
							"[UnitID] INTEGER,"
							"[SubunitID] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}

   // 8/2006  now populate the table from ffplus4.ffp (the prototype database //
   // 9/2006  also re-populate FireUnits and FireRegions tables (which now have the new DOI units)

   CString master_path;
   master_path.Format("%s\\ffplus5.ffp",theApp.workDir);

   if(access(master_path, 0) != 0)
	{
		CString msg;
		msg.Format("Error: Master Database does not exist:\n%s", master_path);
		AfxMessageBox(msg);
		return FALSE;
	}

	CDatabase masterDB;

	CString strConnect;
	strConnect.Format("Provider=MSDASQL;Driver={Microsoft Access Driver (*.mdb)};"
        "Dbq=%s;Uid=admin;Pwd=;", master_path);
	// Attempt to open the new database before replacing our ptr
	try
	{
		//tmpDB->Open(lpszPathName);
		masterDB.OpenEx(strConnect, CDatabase::noOdbcDialog);
	}
	catch (CDBException* e)
	{
		//if(e->m_pErrorInfo)
		//{
			DisplayDBException(e);
			//delete tmpDB;
			e->Delete();
			return FALSE;
	//	}
	//	e->Delete();
	}
    CDOIUnitSet masterDOI(&masterDB);
	CDOIUnitSet newDOI(m_pDB);
	masterDOI.Open();
	newDOI.Open();

	while (! masterDOI.IsEOF()){
       newDOI.AddNew();
	   newDOI.m_UnitID = masterDOI.m_UnitID;
	   newDOI.m_RegionID = masterDOI.m_RegionID;
	   newDOI.m_AgencyID = masterDOI.m_AgencyID;
	   newDOI.m_SubunitID = masterDOI.m_SubunitID;
	   newDOI.m_WFMIcode = masterDOI.m_WFMIcode;
	   newDOI.Update();

	   masterDOI.MoveNext();
	}
    masterDOI.Close();
	newDOI.Close();

    CFireSubunitSet subSet(m_pDB);
	subSet.m_strSort = "[SubunitID]";
	subSet.Open();
    CFireSubunitSet mastersubSet(&masterDB);
	mastersubSet.m_strSort = "[SubunitID]";
	mastersubSet.Open();
    CFireUnitSet unitSet(m_pDB);
	unitSet.m_strSort = "[UnitID]";
	unitSet.Open();
	CFireUnitSet masterunitSet(&masterDB);
	masterunitSet.m_strSort = "[UnitID]";
    masterunitSet.Open();
	// delete existing  units - only "factory" units, not user defined
	int lastUnitID = 1000, lastRegID = 100, lastSubID = 100;
	int unitOffset = 1000, regOffset = 100, subOffset = 100;
	if(!mastersubSet.IsEOF())
	{
		mastersubSet.MoveLast();
		lastSubID = mastersubSet.m_SubunitID;
		mastersubSet.MoveFirst();
	}
	if(!masterunitSet.IsEOF())
	{
		masterunitSet.MoveLast();
		lastUnitID = masterunitSet.m_UnitID;
		masterunitSet.MoveFirst();
	}
    CFireRegionSet regionSet(m_pDB);
	regionSet.m_strSort = "[RegionID]";
	regionSet.Open();
	CFireRegionSet masterregionSet(&masterDB);
	masterregionSet.m_strSort = "[RegionID]";
    masterregionSet.Open();
	if(!masterregionSet.IsEOF())
	{
		masterregionSet.MoveLast();
		lastRegID = masterregionSet.m_RegionID;
		masterregionSet.MoveFirst();
	}
	unitOffset = lastUnitID;
	regOffset = lastRegID;
	subOffset = lastSubID;
	//check to see if existing ID's exceed new factory ID's, and if so increment offsets
	if(!subSet.IsEOF())
	{
		subSet.MoveLast();
		if(subSet.m_SubunitID > lastSubID)
		{
			lastSubID = subOffset = subSet.m_SubunitID + 1;
		}
		subSet.MoveFirst();
	}
	if(!unitSet.IsEOF())
	{
		unitSet.MoveLast();
		if(unitSet.m_UnitID > lastUnitID)
		{
			lastUnitID = unitOffset = unitSet.m_UnitID + 1;
		}
		unitSet.MoveFirst();
	}
	if(!regionSet.IsEOF())
	{
		regionSet.MoveLast();
		if(regionSet.m_RegionID > lastRegID)
		{
			lastRegID = regOffset = regionSet.m_RegionID + 1;
		}
		regionSet.MoveFirst();
	}
	while(!subSet.IsEOF())
	{
		if(subSet.m_SubunitID > 660)
		{
			int oldSub = subSet.m_SubunitID, newSub = subSet.m_SubunitID + subOffset;
			subSet.Edit();
			subSet.m_SubunitID = newSub;
			subSet.Update();
			//adjust fires
			CFireSet fireSet(m_pDB);
			fireSet.m_strFilter.Format("[SubunitID] = %d", oldSub);
			fireSet.Open();
			while(!fireSet.IsEOF())
			{
				fireSet.Edit();
				fireSet.m_SubunitID = newSub;
				fireSet.Update();
				fireSet.MoveNext();
			}
			fireSet.Close();
			//adjust associations
			CFireAssocSet assocs(m_pDB);
			assocs.m_strFilter.Format("[SubunitID] = %d", oldSub);
			assocs.Open();
			while(!assocs.IsEOF())
			{
				assocs.Edit();
				assocs.m_SubunitID = newSub;
				assocs.Update();
				assocs.MoveNext();
			}
			assocs.Close();
		}
		else
			subSet.Delete();
		subSet.MoveNext();
	}
	while (! unitSet.IsEOF()){
	   //if (unitSet.m_RegionID <= 49)   // 49 is last "factory" region
		if (unitSet.m_UnitID <= 1230)   // 49 is last "factory" region
          unitSet.Delete();
	   else
	   {
			int oldUnit = unitSet.m_UnitID, newUnit = unitSet.m_UnitID + unitOffset;
			unitSet.Edit();
			unitSet.m_UnitID = newUnit;
			unitSet.Update();
			//adjust subUnits
			subSet.m_strFilter.Format("[UnitID] = %d", oldUnit);
			subSet.Requery();
			while(!subSet.IsEOF())
			{
				subSet.Edit();
				subSet.m_UnitID = newUnit;
				subSet.Update();
				subSet.MoveNext();
			}
			//adjust fires
			CFireSet fireSet(m_pDB);
			fireSet.m_strFilter.Format("[UnitID] = %d", oldUnit);
			fireSet.Open();
			while(!fireSet.IsEOF())
			{
				fireSet.Edit();
				fireSet.m_UnitID = newUnit;
				fireSet.Update();
				fireSet.MoveNext();
			}
			fireSet.Close();
			//adjust associations
			CFireAssocSet assocs(m_pDB);
			assocs.m_strFilter.Format("[UnitID] = %d", oldUnit);
			assocs.Open();
			while(!assocs.IsEOF())
			{
				assocs.Edit();
				assocs.m_UnitID = newUnit;
				assocs.Update();
				assocs.MoveNext();
			}
			assocs.Close();

		   // need to bump up regionID and unitID
		  /* int oldUnit = unitSet.m_UnitID;
		   int oldReg = unitSet.m_RegionID;
		   unitSet.Edit();
		   unitSet.m_RegionID += regOffset;
		   unitSet.m_UnitID += unitOffset;
		   unitSet.Update();
		   // now modify any existing custom subunits for this unit
		   CFireSubunitSet subSet(m_pDB);
		   subSet.m_strFilter.Format("[UnitID]=%d",oldUnit);//unitSet.m_UnitID);
		   subSet.Open();
		   while (! subSet.IsEOF()){
                 subSet.Edit();
				 subSet.m_UnitID = oldUnit + unitOffset;//unitSet.m_UnitID;//+= 1000;
				 subSet.Update();
				 subSet.MoveNext();
		   }
		   subSet.Close();
		   // now modify any existing fires for this unit
		   CFireSet modfireSet(m_pDB);
		   modfireSet.m_strFilter.Format("[UnitID]=%d", oldUnit);//unitSet.m_UnitID);
		   modfireSet.Open();
		   while (!modfireSet.IsEOF()){
               modfireSet.Edit();
			   modfireSet.m_UnitID = oldUnit + unitOffset;//unitSet.m_UnitID;// += 1000;
			   modfireSet.m_RegionID = oldReg + regOffset;//unitSet.m_RegionID;// += 100;
			   modfireSet.Update();
               modfireSet.MoveNext();
		   }
		   modfireSet.Close();
           // now modify any existing fire associations for this unit
           CFireAssocSet modfaSet(m_pDB);
		   modfaSet.m_strFilter.Format("[UnitID]=%d", oldUnit);//, unitSet.m_UnitID);
		   modfaSet.Open();
		   while (!modfaSet.IsEOF()){
               modfaSet.Edit();
			   modfaSet.m_UnitID = oldUnit + unitOffset;//unitSet.m_UnitID;// += 1000;
			   modfaSet.m_RegionID = oldReg + regOffset;//unitSet.m_RegionID;// += 100;
			   modfaSet.Update();
               modfaSet.MoveNext();
		   }
		   modfaSet.Close();*/
	   }

	   unitSet.MoveNext();
	}
	while(!regionSet.IsEOF())
	{
		if (regionSet.m_RegionID <= 49)   // 49 is last "factory" region
          regionSet.Delete();
	   else
	   {
			int oldReg = regionSet.m_RegionID, newReg = regionSet.m_RegionID + regOffset;
			regionSet.Edit();
			regionSet.m_RegionID = newReg;
			regionSet.Update();
			//adjust Units
			unitSet.m_strFilter.Format("[RegionID] = %d", oldReg);
			unitSet.Requery();
			while(!unitSet.IsEOF())
			{
				unitSet.Edit();
				unitSet.m_RegionID = newReg;
				unitSet.Update();
				unitSet.MoveNext();
			}
			//adjust fires
			CFireSet fireSet(m_pDB);
			fireSet.m_strFilter.Format("[RegionID] = %d", oldReg);
			fireSet.Open();
			while(!fireSet.IsEOF())
			{
				fireSet.Edit();
				fireSet.m_RegionID = newReg;
				fireSet.Update();
				fireSet.MoveNext();
			}
			fireSet.Close();
			//adjust associations
			CFireAssocSet assocs(m_pDB);
			assocs.m_strFilter.Format("[RegionID] = %d", oldReg);
			assocs.Open();
			while(!assocs.IsEOF())
			{
				assocs.Edit();
				assocs.m_RegionID = newReg;
				assocs.Update();
				assocs.MoveNext();
			}
			assocs.Close();
	   }
	   regionSet.MoveNext();
	}
	//unitSet.Close();
    //unitSet.Open();
	// re-populate from master
	while(!masterregionSet.IsEOF())
	{
		regionSet.m_strFilter.Format("[RegionID] = %d", masterregionSet.m_RegionID);
		regionSet.Requery();
		if(!regionSet.IsEOF())
			regionSet.Edit();
		else
			regionSet.AddNew();
		regionSet.m_RegionID = masterregionSet.m_RegionID;
		regionSet.m_AgencyID = masterregionSet.m_AgencyID;
		regionSet.m_Name = masterregionSet.m_Name;
		regionSet.Update();
		masterregionSet.MoveNext();
	}
	while (! masterunitSet.IsEOF())
	{
		unitSet.m_strFilter.Format("[UnitID] = %d", masterunitSet.m_UnitID);
		unitSet.Requery();
		if(!unitSet.IsEOF())
			unitSet.Edit();
		else
			unitSet.AddNew();
		unitSet.m_UnitID = masterunitSet.m_UnitID;
		unitSet.m_RegionID = masterunitSet.m_RegionID;
		unitSet.m_Name = masterunitSet.m_Name;
		unitSet.Update();
		masterunitSet.MoveNext();
	}
	while (! mastersubSet.IsEOF())
	{
		subSet.m_strFilter.Format("[UnitID] = %d", mastersubSet.m_SubunitID);
		subSet.Requery();
		if(!subSet.IsEOF())
			subSet.Edit();
		else
			subSet.AddNew();
		subSet.m_SubunitID = mastersubSet.m_SubunitID;
		subSet.m_UnitID = mastersubSet.m_UnitID;
		subSet.m_Name = mastersubSet.m_Name;
		subSet.Update();
		mastersubSet.MoveNext();
	}
    masterregionSet.Close();
    mastersubSet.Close();
    masterunitSet.Close();
    subSet.Close();
    unitSet.Close();
    regionSet.Close();

	//CFireRegionSet masterregionSet(&tmpDB);
   // masterregionSet.Open();

	// delete existing regions - only "factory" regions, not user defined
	/*regionSet.MoveFirst();
	while (! regionSet.IsEOF()){
	   if (regionSet.m_RegionID <= 52) // 52 is last "factory" region
          regionSet.Delete();
	   else { // bump up custom regionID so it doesn't interfere with factory IDs
		   regionSet.Edit();
		   regionSet.m_RegionID += regOffset;
		   regionSet.Update();
	   }
	   regionSet.MoveNext();
	}
	regionSet.Close();
    regionSet.Open();

	// re-populate from master
	while (! masterregionSet.IsEOF()){
        regionSet.AddNew();
		regionSet.m_AgencyID = masterregionSet.m_AgencyID;
		regionSet.m_RegionID = masterregionSet.m_RegionID;
		regionSet.m_Name = masterregionSet.m_Name;

		regionSet.Update();
		masterregionSet.MoveNext();
	}
    masterregionSet.Close();
    regionSet.Close();*/

	masterDB.Close();
	//delete tmpDB;

    return 1;
}

int CFireplusDoc::CreateRefDOICauseTable()
{
	CWaitCursor wait;
    try
	{
	CString sqlStr = "CREATE TABLE [refDOICause] ("
		                    "[GeneralCause] INTEGER,"
							"[SpecificCause] INTEGER,"
							"[GeneralDescription] CHAR(15),"
							"[SpecificDescription] CHAR(35),"
							"[Abbr] CHAR(4))";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}

   CString master_path;
   master_path.Format("%s\\ffplus5.ffp",theApp.workDir);

   if(access(master_path, 0) != 0)
	{
		CString msg;
		msg.Format("Error: Master Database does not exist:\n%s", master_path);
		AfxMessageBox(msg);
		return FALSE;
	}

	CDatabase tmpDB;

	CString strConnect;
	strConnect.Format("Provider=MSDASQL;Driver={Microsoft Access Driver (*.mdb)};"
        "Dbq=%s;Uid=admin;Pwd=;", master_path);
	// Attempt to open the new database before replacing our ptr
	try
	{
		//tmpDB->Open(lpszPathName);
		tmpDB.OpenEx(strConnect, CDatabase::noOdbcDialog);
	}
	catch (CDBException* e)
	{
		//if(e->m_pErrorInfo)
		//{
			DisplayDBException(e);
			e->Delete();
			return FALSE;
	//	}
	//	e->Delete();
	}
    CDOICauseSet masterDOI(&tmpDB);
	CDOICauseSet newDOI(m_pDB);
	masterDOI.Open();
	newDOI.Open();


	while (! masterDOI.IsEOF()){
       newDOI.AddNew();
	   newDOI.m_GeneralCause = masterDOI.m_GeneralCause;
	   newDOI.m_SpecificCause = masterDOI.m_SpecificCause;
	   newDOI.m_GeneralDescription = masterDOI.m_GeneralDescription;
	   newDOI.m_SpecificDescription = masterDOI.m_SpecificDescription;
	   newDOI.m_Abbr = masterDOI.m_Abbr;
	   newDOI.Update();

	   masterDOI.MoveNext();
	}
    masterDOI.Close();
	newDOI.Close();

	return 1;

}



int CFireplusDoc::CreateffpFxOptsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpFxOpts] ("
							"[AddType] INTEGER,"
							"[TempPct] DOUBLE,"
							"[RHPct] DOUBLE,"
							"[WindPct] DOUBLE,"
							"[LineColor] INTEGER,"
							"[LineWidth] INTEGER,"
							"[LineStyle] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
		//now populate with default members
		CFFPfxOptsSet fxOpts(m_pDB);
		fxOpts.Open();
		fxOpts.AddNew();
		fxOpts.m_AddType = 0;
		fxOpts.m_TempPct = 90.0;
		fxOpts.m_RHPct = 10.0;
		fxOpts.m_WindPct = 90.0;
		fxOpts.m_LineColor = 0;
		fxOpts.m_LineStyle = 1;
		fxOpts.m_LineWidth = 1;
		fxOpts.Update();
		fxOpts.Close();
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpFxVarsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpFxVars] ("
			"[VarID] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

void CFireplusDoc::CheckLFITable()
{
	bool hasHerbDaysAvg = false,
		hasWoodyDaysAvg = false,
		hasHerbMaxGSI = false,
		hasHerbGreenup = false,
		hasWoodyMaxGSI = false,
		hasWoodyGreenup = false,
		hasMaxHerb = false,
		hasMinHerb = false,
		hasMaxWoody = false,
		hasMinWoody = false,
		hasHerbTMinMin = false,
		hasHerbTMinMax = false,
		hasHerbVPDMin = false,
		hasHerbVPDMax = false,
		hasHerbDaylenMin = false,
		hasHerbDaylenMax = false,
		hasHerbUseVPDAvg = false,
		hasWoodyTMinMin = false,
		hasWoodyTMinMax = false,
		hasWoodyVPDMin = false,
		hasWoodyVPDMax = false,
		hasWoodyDaylenMin = false,
		hasWoodyDaylenMax = false,
		hasWoodyUseVPDAvg = false,
		hasSigStation  = false,
		hasPcpDays = false,
		hasHerbPcpDays = false,
		hasWoodyPcpDays = false,
		hasPcpMin = false,
		hasPcpMax = false,
		hasHerbPcpMin = false,
		hasHerbPcpMax = false,
		hasWoodyPcpMin = false,
		hasWoodyPcpMax = false;

	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "ffpLFI";
		columns.Open();
		while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("HerbDaysAvg") == 0)
				hasHerbDaysAvg = true;
			if(columns.m_strColumnName.CompareNoCase("HerbMaxGSI") == 0)
				hasHerbMaxGSI = true;
			if(columns.m_strColumnName.CompareNoCase("HerbGreenup") == 0)
				hasHerbGreenup = true;
			if(columns.m_strColumnName.CompareNoCase("HerbMax") == 0)
				hasMaxHerb = true;
			if(columns.m_strColumnName.CompareNoCase("HerbMin") == 0)
				hasMinHerb = true;
			if(columns.m_strColumnName.CompareNoCase("HerbTMinMax") == 0)
				hasHerbTMinMax = true;
			if(columns.m_strColumnName.CompareNoCase("HerbTMinMin") == 0)
				hasHerbTMinMin = true;
			if(columns.m_strColumnName.CompareNoCase("HerbVPDMax") == 0)
				hasHerbVPDMax = true;
			if(columns.m_strColumnName.CompareNoCase("HerbVPDMin") == 0)
				hasHerbVPDMin = true;
			if(columns.m_strColumnName.CompareNoCase("HerbDaylenMax") == 0)
				hasHerbDaylenMax = true;
			if(columns.m_strColumnName.CompareNoCase("HerbDaylenMin") == 0)
				hasHerbDaylenMin = true;
			if(columns.m_strColumnName.CompareNoCase("HerbUseVPDAvg") == 0)
				hasHerbUseVPDAvg = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyDaysAvg") == 0)
				hasWoodyDaysAvg = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyMaxGSI") == 0)
				hasWoodyMaxGSI = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyGreenup") == 0)
				hasWoodyGreenup = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyMax") == 0)
				hasMaxWoody = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyMin") == 0)
				hasMinWoody = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyTMinMax") == 0)
				hasWoodyTMinMax = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyTMinMin") == 0)
				hasWoodyTMinMin = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyVPDMax") == 0)
				hasWoodyVPDMax = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyVPDMin") == 0)
				hasWoodyVPDMin = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyDaylenMax") == 0)
				hasWoodyDaylenMax = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyDaylenMin") == 0)
				hasWoodyDaylenMin = true;
			if(columns.m_strColumnName.CompareNoCase("WoodyUseVPDAvg") == 0)
				hasWoodyUseVPDAvg = true;

			if (columns.m_strColumnName.CompareNoCase("PcpDays") == 0)
				hasPcpDays = true;
			if (columns.m_strColumnName.CompareNoCase("HerbPcpDays") == 0)
				hasHerbPcpDays = true;
			if (columns.m_strColumnName.CompareNoCase("WoodyPcpDays") == 0)
				hasWoodyPcpDays = true;
			if (columns.m_strColumnName.CompareNoCase("PcpMin") == 0)
				hasPcpMin = true;
			if (columns.m_strColumnName.CompareNoCase("PcpMax") == 0)
				hasPcpMax = true;
			if (columns.m_strColumnName.CompareNoCase("HerbPcpMin") == 0)
				hasHerbPcpMin = true;
			if (columns.m_strColumnName.CompareNoCase("HerbPcpMax") == 0)
				hasHerbPcpMax = true;
			if (columns.m_strColumnName.CompareNoCase("WoodyPcpMin") == 0)
				hasWoodyPcpMin = true;
			if (columns.m_strColumnName.CompareNoCase("WoodyPcpMax") == 0)
				hasWoodyPcpMax = true;


			if(columns.m_strColumnName.CompareNoCase("SIG_Station") == 0)
				hasSigStation = true;
			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if(!hasHerbDaysAvg)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbDaysAvg] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasHerbMaxGSI)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbMaxGSI] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasHerbGreenup)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbGreenup] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMaxHerb)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMinHerb)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasHerbTMinMax)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbTMinMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasHerbTMinMin)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbTMinMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasHerbVPDMax)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbVPDMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasHerbVPDMin)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbVPDMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasHerbDaylenMax)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbDaylenMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasHerbDaylenMin)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbDaylenMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasHerbUseVPDAvg)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbUseVPDAvg] YESNO";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}

	//woody moisture fields
	if(!hasWoodyDaysAvg)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyDaysAvg] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasWoodyMaxGSI)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyMaxGSI] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasWoodyGreenup)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyGreenup] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMaxWoody)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMinWoody)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasWoodyTMinMax)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyTMinMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasWoodyTMinMin)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyTMinMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasWoodyVPDMax)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyVPDMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasWoodyVPDMin)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyVPDMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasWoodyDaylenMax)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyDaylenMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasWoodyDaylenMin)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyDaylenMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasWoodyUseVPDAvg)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyUseVPDAvg] YESNO";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}

	//pcp days for all three types...
	if (!hasPcpDays)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [PcpDays] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasHerbPcpDays)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbPcpDays] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasWoodyPcpDays)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyPcpDays] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}

	if (!hasPcpMin)
	{

		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [PcpMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasPcpMax)
	{

		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [PcpMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasHerbPcpMin)
	{

		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbPcpMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasHerbPcpMax)
	{

		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [HerbPcpMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasWoodyPcpMin)
	{

		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyPcpMin] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasWoodyPcpMax)
	{

		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [WoodyPcpMax] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}

	if(!hasSigStation)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpLFI] ADD [SIG_Station] CHAR(20)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasLFI)
	{
	 // populate with LFI defaults
		try
		{
		   CLFISet lfiSet(m_pDB);
		   lfiSet.Open();
		   lfiSet.AddNew();
		   lfiSet.m_LFIdaysAvg  = 21;
		   lfiSet.m_TMinMin = -2.0;
		   lfiSet.m_TMinMax = 5.0;
		   lfiSet.m_VPDMin = 900.0;
		   lfiSet.m_VPDMax = 4100.0;
		   lfiSet.m_DaylenMin = 36000;
		   lfiSet.m_DaylenMax = 39600;
		   lfiSet.m_UseVPDAvg = FALSE;
		  // lfiSet.m_PcpDays = 30;
		   //lfiSet.m_HerbPcpDays = 30;
		  // lfiSet.m_WoodyPcpDays = 30;
		   lfiSet.Update();
		   lfiSet.Close();
		}
		catch( CDBException* e )
		{
			DisplayDBException(e);
			e->Delete();
		}
	}

	if (!hasHerbDaysAvg)
	{
		CLFISet lfiSet(m_pDB);
		if (! lfiSet.IsOpen())
			lfiSet.Open();
		if(lfiSet.IsEOF())//need a record
		{
			lfiSet.AddNew();
			//must populate original LFI fields
		   lfiSet.m_LFIdaysAvg  = 21;
		   lfiSet.m_TMinMin = -2.0;
		   lfiSet.m_TMinMax = 5.0;
		   lfiSet.m_VPDMin = 900.0;
		   lfiSet.m_VPDMax = 4100.0;
		   lfiSet.m_DaylenMin = 36000;
		   lfiSet.m_DaylenMax = 39600;
		   lfiSet.m_UseVPDAvg = TRUE;
		  // lfiSet.m_PcpMin = 0.5;
		  // lfiSet.m_PcpMax = 1.5;
		   //lfiSet.m_PcpDays = 30;
		}
		else
			lfiSet.Edit();
		lfiSet.m_SIG_Station = "######"; //was only record, set as defaults
		//lfiSet.m_PcpDays = 30;
		//herb moistures
		lfiSet.m_HerbDaysAvg = 21;
		lfiSet.m_HerbMax = 250.0;
		lfiSet.m_HerbMin = 30;
		lfiSet.m_HerbMaxGSI = 1.0;
		lfiSet.m_HerbGreenup = 0.5;
		lfiSet.m_HerbTMinMin = -2.0;
		lfiSet.m_HerbTMinMax = 5.0;
		lfiSet.m_HerbVPDMin = 900.0;
		lfiSet.m_HerbVPDMax = 4100.0;
		lfiSet.m_HerbDaylenMin = 36000.0;
		lfiSet.m_HerbDaylenMax = 39600.0;
		lfiSet.m_HerbUseVPDAvg = TRUE;
		//lfiSet.m_HerbPcpMin = 0.5;
		//lfiSet.m_HerbPcpMax = 1.5;
		//woody moistures
		lfiSet.m_WoodyDaysAvg = 21;
		lfiSet.m_WoodyMax = 200.0;
		lfiSet.m_WoodyMin = 50.0;
		lfiSet.m_WoodyMaxGSI = 1.0;
		lfiSet.m_WoodyGreenup = 0.5;
		lfiSet.m_WoodyTMinMin = -2.0;
		lfiSet.m_WoodyTMinMax = 5.0;
		lfiSet.m_WoodyVPDMin = 900.0;
		lfiSet.m_WoodyVPDMax = 4100.0;
		lfiSet.m_WoodyDaylenMin = 36000.0;
		lfiSet.m_WoodyDaylenMax = 39600.0;
		lfiSet.m_WoodyUseVPDAvg = TRUE;
		//lfiSet.m_WoodyPcpMin = 0.5;
		//lfiSet.m_WoodyPcpMax = 1.5;
		lfiSet.Update();
		lfiSet.Close();
	}
	if (!hasPcpDays)
	{
		CLFISet lfiSet(m_pDB);
		if (!lfiSet.IsOpen())
			lfiSet.Open();
		/*if (lfiSet.IsEOF())//need a record
		{
			lfiSet.AddNew();
		}
		else
			lfiSet.Edit();*/
		while (!lfiSet.IsEOF())
		{
			lfiSet.Edit();
			lfiSet.m_PcpDays = 30;
			lfiSet.m_HerbPcpDays = 30;
			lfiSet.m_WoodyPcpDays = 30;
			lfiSet.Update();
			lfiSet.MoveNext();
		}
		lfiSet.Close();
	}
	if (!hasPcpMin)
	{
		CLFISet lfiSet(m_pDB);
		if (!lfiSet.IsOpen())
			lfiSet.Open();
		/*if (lfiSet.IsEOF())//need a record
		{
			lfiSet.AddNew();
		}
		else
			lfiSet.Edit();*/
		while (!lfiSet.IsEOF())
		{
			lfiSet.Edit();
			lfiSet.m_PcpMin = 0.5;
			lfiSet.m_PcpMax = 1.5;
			lfiSet.m_HerbPcpMin = 0.5;
			lfiSet.m_HerbPcpMax = 1.5;
			lfiSet.m_WoodyPcpMin = 0.5;
			lfiSet.m_WoodyPcpMax = 1.5;
			lfiSet.Update();
			lfiSet.MoveNext();
		}
		lfiSet.Close();
	}
}

int CFireplusDoc::CreateffpOptsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpOpts] ("
							"[CP1] INTEGER,"
							"[CP1Color] INTEGER,"
							"[CP2] INTEGER,"
							"[CP2Color] INTEGER,"
							"[RunSaveOn] INTEGER,"
							"[PctWxVarID] INTEGER,"
							"[MaxMissWx] INTEGER,"
							"[LastGfxFmt] INTEGER,"
							"[EventHours] INTEGER,"
							"[EventDays] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
		CheckOptsTable();
		//now populate with theApp members (if available)
		CFFPOptsSet optsSet(this->m_pDB);
		optsSet.Open();
		optsSet.AddNew();
		optsSet.m_CP1 = theApp.m_OverallCP1;
		optsSet.m_CP1Color = theApp.m_OverallCP1Color;
		optsSet.m_CP2 = theApp.m_OverallCP2;
		optsSet.m_CP2Color = theApp.m_OverallCP2Color;
		optsSet.m_RunSaveOn = 0;
		optsSet.m_PctWxVarID = theApp.rerapVarID;
		optsSet.m_MaxMissWx = theApp.maxMissingWx;
		optsSet.m_LastGfxFmt = theApp.lastGraphicFormat;
		optsSet.m_EventHours = 1;
		optsSet.m_EventDays = 1;
		optsSet.Update();
		optsSet.Close();
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}
int CFireplusDoc::CreateffpOverlaysTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpOverlays] ("
							"[runID] INTEGER,"
							"[Year] INTEGER,"
							"[Color] INTEGER,"
							"[Width] INTEGER,"
							"[Style] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
		//populate from theApp for general overlays
		CFFPOverlaysSet overlaysSet(this->m_pDB);
		overlaysSet.Open();
		for(int i = 0; i < OVERLAYS; i++)
		{
			overlaysSet.AddNew();
			overlaysSet.m_runID = 0;
			overlaysSet.m_Year = theApp.overLayYears[i];
			overlaysSet.m_Color = theApp.overLayColors[i];
			overlaysSet.m_Width = theApp.lineWidths[i];
			overlaysSet.m_Style = theApp.lineStyles[i];
			overlaysSet.Update();
		}
		overlaysSet.Close();
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}
int CFireplusDoc::CreateffpMergesTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpMerges] ("
							"[runID] INTEGER,"
							"[Color] INTEGER,"
							"[Width] INTEGER,"
							"[Style] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
		//populate from theApp for general overlays
		CFFPMergesSet mergesSet(this->m_pDB);
		mergesSet.Open();
		for(int i = 0; i < OVERLAYS; i++)
		{
			mergesSet.AddNew();
			mergesSet.m_runID = 0;
			mergesSet.m_Color = theApp.mergeColors[i];
			mergesSet.m_Width = theApp.mergeWidths[i];
			mergesSet.m_Style = theApp.mergeStyles[i];
			mergesSet.Update();
		}
		mergesSet.Close();
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}
int CFireplusDoc::CreateffpHourlyEventsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpHourlyEvents] ("
							"[SIG_Station] CHAR(20),"
							"[Row] INTEGER,"
							"[Category] INTEGER,"
							"[RowOp] INTEGER,"
							"[ValOp] INTEGER,"
							"[ValType] INTEGER,"
							"[Value] DOUBLE,"
							"[VarID] INTEGER,"
							"[VarString] CHAR(30))";
		m_pDB->ExecuteSQL(sqlStr);
		CFFPHourlyEventsSet hourlySet(m_pDB);
		hourlySet.Open();
		hourlySet.AddNew();
		hourlySet.m_SIG_Station = "######";
		hourlySet.m_Row = 0;
		hourlySet.m_Category = Daily;//theApp.queryRows[i].cat;
		hourlySet.m_RowOp = 2;//theApp.queryRows[i].rowOp;
		hourlySet.m_ValOp = GE;//theApp.queryRows[i].valOp;
		hourlySet.m_ValType = Value;//theApp.queryRows[i].valType;
		hourlySet.m_Value = 1.0;//theApp.queryRows[i].value;
		hourlySet.m_VarID = 8;//temp //theApp.queryRows[i].varID;
		hourlySet.m_VarString = "Precipitation Amount";//theApp.queryRows[i].varString;
		hourlySet.Update();
		hourlySet.Close();
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}
int CFireplusDoc::CreateffpDailyEventsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpDailyEvents] ("
							"[SIG_Station] CHAR(20),"
							"[Row] INTEGER,"
							"[Category] INTEGER,"
							"[RowOp] INTEGER,"
							"[ValOp] INTEGER,"
							"[ValType] INTEGER,"
							"[Value] DOUBLE,"
							"[VarID] INTEGER,"
							"[VarString] CHAR(30))";
		m_pDB->ExecuteSQL(sqlStr);
		CFFPDailyEventsSet dailySet(m_pDB);
		dailySet.Open();
		dailySet.AddNew();
		dailySet.m_SIG_Station = "######";
		dailySet.m_Row = 0;
		dailySet.m_Category = Sum;//theApp.queryRows[i].cat;
		dailySet.m_RowOp = 2;
		dailySet.m_ValOp = GE;//theApp.queryRows[i].valOp;
		dailySet.m_ValType = Value;//theApp.queryRows[i].valType;
		dailySet.m_Value = 2.0;//theApp.queryRows[i].value;
		dailySet.m_VarID = 8;//precip //theApp.queryRows[i].varID;
		dailySet.m_VarString = "Precipitation Amount";//theApp.queryRows[i].varString;
		dailySet.Update();
		dailySet.Close();
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpSowOptionsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpSowOptions] ("
							"[SIG_Station] CHAR(20),"
							"[SOW_Usage] INTEGER,"
							"[WetFlag_Usage] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpSowThresholdsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpSowThresholds] ("
							"[SIG_Station] CHAR(20),"
							"[PCNT_Clear] INTEGER,"
							"[PCNT_Scattered] INTEGER,"
							"[PCNT_Broken] INTEGER,"
							"[1HR_Drizzle] DOUBLE,"
							"[1HR_Rain] DOUBLE,"
							"[1HR_Showers] DOUBLE,"
							"[3HR_DUR_WetFlag] INTEGER,"
							"[3HR_AMT_WetFlag] DOUBLE,"
							"[24HR_DUR_WetFlag] INTEGER,"
							"[24HR_AMT_WetFlag] DOUBLE)";
		m_pDB->ExecuteSQL(sqlStr);
		CSowThresholdsSet sowSet(m_pDB);
		sowSet.Open();
		sowSet.AddNew();
		sowSet.m_SIG_Station = "######";
		sowSet.m_PCNT_Clear = 85;
		sowSet.m_PCNT_Scattered = 75;
		sowSet.m_PCNT_Broken = 50;
		sowSet.m_1HR_Drizzle = 0.05;
		sowSet.m_1HR_Rain = 0.1;
		sowSet.m_1HR_Showers = 0.25;
		sowSet.m_3HR_DUR_WetFlag = 3;
		sowSet.m_3HR_AMT_WetFlag = 0.5;
		sowSet.m_24HR_DUR_WetFlag = 12;
		sowSet.m_24HR_AMT_WetFlag = 1.0;
		sowSet.Update();
		sowSet.Close();
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

/*int CFireplusDoc::CreateTimeZoneTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [TimeZone] ("
			"[Name] CHAR(20),"
			"[ABBREV] CHAR(5)"
			"[Offset] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
		//now populate
		CTimeZones tz;
	}
	catch (CDBException* e)
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}*/

int CFireplusDoc::CreateffpTermTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpTerm] ("
							"[TermID] INTEGER,"
							"[Name] CHAR(40),"
							"[SIG_Station] CHAR(20),"
							"[StartDate] DATETIME,"
							"[Comment] MEMO,"
							"[Coeff1] DOUBLE,"
							"[Coeff2] DOUBLE,"
							"[Coeff3] DOUBLE)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpTermDatesTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpTermDates] ("
							"[TermID] INTEGER,"
							"[Year] INTEGER,"
							"[Date] DATETIME,"
							"[Comment] CHAR(150))";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpTermPercentilesTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpTermPercentiles] ("
							"[TermID] INTEGER,"
							"[Percentile] DOUBLE)";
		m_pDB->ExecuteSQL(sqlStr);
		CTermPercentilesSet tps(this->m_pDB);
		tps.Open();
		tps.AddNew();
		tps.m_TermID = -1;
		tps.m_Percentile = 0.25;
		tps.Update();
		tps.AddNew();
		tps.m_TermID = -1;
		tps.m_Percentile = 0.50;
		tps.Update();
		tps.AddNew();
		tps.m_TermID = -1;
		tps.m_Percentile = 0.75;
		tps.Update();
		tps.AddNew();
		tps.m_TermID = -1;
		tps.m_Percentile = 0.90;
		tps.Update();
		tps.AddNew();
		tps.m_TermID = -1;
		tps.m_Percentile = 0.99;
		tps.Update();
		tps.Close();
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateGenericFireImportsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpGenericFireImports] ("
							"[AgencyID] INTEGER,"
							"[Delimiter] INTEGER,"
							"[DateFormat] INTEGER,"
							"[TimeFormat] INTEGER,"
							"[UseRegion] INTEGER,"
							"[DefaultRegionID] INTEGER,"
							"[UseUnit] INTEGER,"
							"[DefaultUnitID] INTEGER,"
							"[UseSubunit] INTEGER,"
							"[DefaultSubunitID] INTEGER,"
							"[DupHandling] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateGenericFireFieldsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpGenericFireFields] ("
							"[AgencyID] INTEGER,"
							"[FieldNum] INTEGER,"
							"[Rank] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateGenericWxImportsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpGenericWxImports] ("
							"[UseStation] INTEGER,"
							"[DefStation] CHAR(6),"
							"[UseObsType] INTEGER,"
							"[DefObsType] INTEGER,"
							"[UseObsTime] INTEGER,"
							"[DefObsTime] DATETIME,"
							"[Delimiter] INTEGER,"
							"[DateFormat] INTEGER,"
							"[TimeFormat] INTEGER,"
							"[DupHandling] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateGenericWxFieldsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpGenericWxFields] ("
							"[FieldNum] INTEGER,"
							"[Rank] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}
int CFireplusDoc::CreateUserVarTable()
{
	try
	{
		/*CDaoTableDef vt(m_pDB);
		vt.Create("ffpUserVars");
		vt.CreateField("VarID", dbByte, 1, dbUpdatableField | dbFixedField);
		vt.CreateField("UserVarID", dbByte, 1, dbUpdatableField | dbFixedField);
		vt.CreateField("Variable Name", dbText, 30, dbUpdatableField | dbFixedField);
///		vt.CreateField("Short Name", dbText, 20, dbUpdatableField | dbFixedField);
		vt.CreateField("Abbrev", dbText, 5, dbUpdatableField | dbFixedField);
		vt.CreateField("Decimals", dbInteger, 2, dbUpdatableField | dbFixedField);
		vt.Append();
		vt.Close();*/
		CString sqlStr = "CREATE TABLE [ffpUserVars] ("
							"[VarID] INTEGER,"
							"[UserVarID] INTEGER,"
							"[Variable Name] CHAR(30),"
							"[Abbrev] CHAR(5),"
							"[Decimals] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateUserValTable()
{
	try
	{
/*		CDaoTableDef vt(m_pDB);
		vt.Create("ffpUserVals");
	//	vt.CreateField("UserVarID", dbByte, 1, dbUpdatableField | dbFixedField);
		vt.CreateField("StationID", dbText, 6, dbUpdatableField | dbFixedField);
		vt.CreateField("ObsDate", dbDate, 8, dbUpdatableField | dbFixedField);
		//vt.CreateField("Value", dbDouble, 8, dbUpdatableField | dbFixedField);
		vt.Append();
		vt.Close();*/
		CString sqlStr = "CREATE TABLE [ffpUserVals] ("
							"[StationID] CHAR(6),"
							"[ObsDate] DATETIME)";
							//"[1] INTEGER,"
							//"[2] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpCandidatesTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpCandidates] ("
									"[SIG_Station] CHAR(20),"
									"[MODEL] CHAR(10),"
									"[VarName] CHAR(10),"
									"[StartYear] INTEGER,"
									"[EndYear] INTEGER,"
									"[StartMonth] CHAR(12),"
									"[EndMonth] CHAR(12),"
									"[StartDay] INTEGER,"
									"[EndDay] INTEGER,"
									"[GreenJulian] DATETIME,"
									"[FreezeJulian] DATETIME,"
									"[FD_Type] CHAR(10),"
									"[FD_R2] DOUBLE,"
									"[FD_Chi2] DOUBLE,"
									"[FD_PVal] DOUBLE,"
									"[FD_PMin] DOUBLE,"
									"[FD_PMax] DOUBLE,"
									"[FD_Beta0] DOUBLE,"
									"[FD_Beta1] DOUBLE,"
									"[LFD_Acres] CHAR(20),"
									"[LFD_R2] DOUBLE,"
									"[LFD_Chi2] DOUBLE,"
									"[LFD_PVal] DOUBLE,"
									"[LFD_PMin] DOUBLE,"
									"[LFD_PMax] DOUBLE,"
									"[LFD_Beta0] DOUBLE,"
									"[LFD_Beta1] DOUBLE,"
									"[MFD_NumFires] CHAR(20),"
									"[MFD_R2] DOUBLE,"
									"[MFD_Chi2] DOUBLE,"
									"[MFD_PVal] DOUBLE,"
									"[MFD_PMin] DOUBLE,"
									"[MFD_PMax] DOUBLE,"
									"[MFD_Beta0] DOUBLE,"
									"[MFD_Beta1] DOUBLE,"
									"[Comment] CHAR(80))";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

// added for batch

int CFireplusDoc::CreateffpRunDescriptionTable()
{
	try
	{
	/*	CDaoTableDef vt(m_pDB);
		vt.Create("ffpRunDescription");

		vt.CreateField("runID", dbInteger,10, dbUpdatableField | dbFixedField);

		vt.CreateField("Description", dbText, 50, dbUpdatableField | dbFixedField);
        vt.CreateField("SIG_Station", dbText, 20, dbUpdatableField | dbFixedField);
		vt.CreateField("StartYear", dbInteger, 4, dbUpdatableField | dbFixedField);
        vt.CreateField("EndYear", dbInteger, 4, dbUpdatableField | dbFixedField);
        vt.CreateField("StartMonth", dbText, 12, dbUpdatableField | dbFixedField);
        vt.CreateField("EndMonth", dbText, 12, dbUpdatableField | dbFixedField);
        vt.CreateField("StartDay", dbInteger, 2, dbUpdatableField | dbFixedField);
        vt.CreateField("EndDay", dbInteger, 2, dbUpdatableField | dbFixedField);
        vt.CreateField("PeriodLength", dbText, 20, dbUpdatableField | dbFixedField);
        vt.CreateField("Use78", dbBoolean, 1, dbUpdatableField | dbFixedField);
        vt.CreateField("Use88", dbBoolean, 1, dbUpdatableField | dbFixedField);
        vt.CreateField("UseCanadian", dbBoolean, 1, dbUpdatableField | dbFixedField);

		vt.Append();
		vt.Close();*/
		CString sqlStr = "CREATE TABLE [ffpRunDescription] ("
									"[runID] INTEGER,"
									"[Description] CHAR(100),"
									"[SIG_Station] CHAR(20),"
									"[StartYear] INTEGER,"
									"[EndYear] INTEGER,"
									"[StartMonth] CHAR(12),"
									"[EndMonth] CHAR(12),"
									"[StartDay] INTEGER,"
									"[EndDay] INTEGER,"
									"[PeriodLength] CHAR(20),"
									"[Use78] YESNO NOT NULL,"
									"[Use88] YESNO NOT NULL,"
									"[UseCanadian] YESNO NOT NULL)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpRunClimateOptionsTable()
{
	try
	{
		CString sqlStr =
			"CREATE TABLE [ffpRunClimateOptions] ("
				"[runID] INTEGER,"
				"[Variable Name] CHAR(30),"
				"[ShortName] CHAR(20),"
				"[VarID] INTEGER,"
				"[BinSize] INTEGER,"
				"[OptionType] INTEGER,"
				"[Stats Table] YESNO NOT NULL,"
				"[Stats Graph] YESNO NOT NULL,"
				"[Daily Freqs] YESNO NOT NULL,"
				"[Period Mins] YESNO NOT NULL,"
				"[Period Maxs] YESNO NOT NULL,"
				"[Data Count] YESNO NOT NULL,"
				"[CriticalPercentile] INTEGER,"
				"[CriticalPercentile2] INTEGER,"
				"[Daily list] INTEGER,"
				"[FilterValue] DOUBLE)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpRunFireAssociationsTable()
{
	try
	{
	/*	CDaoTableDef vt(m_pDB);
		vt.Create("ffpRunFireAssociations");

		vt.CreateField("runID", dbInteger,10, dbUpdatableField | dbFixedField);

		vt.CreateField("SIG_Station", dbText, 20, dbUpdatableField | dbFixedField);

        vt.CreateField("AgencyID", dbLong, 8, dbUpdatableField | dbFixedField);
        vt.CreateField("RegionID", dbLong, 8, dbUpdatableField | dbFixedField);
		vt.CreateField("UnitID", dbLong, 8, dbUpdatableField | dbFixedField);
        vt.CreateField("SubunitID", dbLong, 8, dbUpdatableField | dbFixedField);

		vt.Append();
		vt.Close();*/
		CString sqlStr =
			"CREATE TABLE [ffpRunFireAssociations] ("
				"[runID] INTEGER,"
				"[SIG_Station] CHAR(20),"
				"[AgencyID] INTEGER,"
				"[RegionID] INTEGER,"
				"[UnitID] INTEGER,"
				"[SubunitID] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpRunFireOptionsTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpRunFireOptions] ("
							"[runID] INTEGER,"
							"[SIG_Station] CHAR(20),"
							"[Lightning] YESNO NOT NULL,"
							"[Human] YESNO NOT NULL,"
							"[All] YESNO NOT NULL,"
							"[Cumulative] YESNO NOT NULL,"
							"[Probability] YESNO NOT NULL,"
							"[Both] YESNO NOT NULL,"
							"[LFAcres] INTEGER,"
							"[MFDFires] INTEGER,"
							"[CauseFlag] INTEGER,"
							"[VarID] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	try
		{
			CString strSql;
			strSql = "UPDATE [ffpRunFireOptions] SET [CauseFlag] = 1022"; // all human causes
			m_pDB->ExecuteSQL(strSql);
		}
	catch(CDBException* e)
		{		
			e->Delete();			
		}


	return 1;
}

int CFireplusDoc::CreateffpRunWxStationTable()
{
	try
	{
		/*CDaoTableDef vt(m_pDB);
		vt.Create("ffpRunWxStation");

		vt.CreateField("runID", dbInteger,10, dbUpdatableField | dbFixedField);

		vt.CreateField("StationID", dbText, 6, dbUpdatableField | dbFixedField);
        vt.CreateField("Name", dbText, 20, dbUpdatableField | dbFixedField);
        vt.CreateField("NFDRSFM", dbText, 1, dbUpdatableField | dbFixedField);

		vt.CreateField("Use88", dbInteger, 1, dbUpdatableField | dbFixedField);
        vt.CreateField("LatDegrees", dbInteger, 8, dbUpdatableField | dbFixedField);
        vt.CreateField("LongDegrees", dbInteger, 8, dbUpdatableField | dbFixedField);
		vt.CreateField("Elevation", dbInteger, 8, dbUpdatableField | dbFixedField);
        vt.CreateField("ClimateCls", dbInteger, 8, dbUpdatableField | dbFixedField);
		vt.CreateField("SlopeCls", dbInteger, 8, dbUpdatableField | dbFixedField);

        vt.CreateField("HerbAnnual", dbInteger, 8, dbUpdatableField | dbFixedField);
		vt.CreateField("GreenJulian", dbDate, 8, dbUpdatableField | dbFixedField);
        vt.CreateField("FreezeJulian", dbDate, 8, dbUpdatableField | dbFixedField);

        vt.CreateField("StartGreenHerb", dbInteger, 1, dbUpdatableField | dbFixedField);
        vt.CreateField("StartGreenShrub", dbInteger, 1, dbUpdatableField | dbFixedField);
        vt.CreateField("StartKBDI", dbInteger, 1, dbUpdatableField | dbFixedField);
        vt.CreateField("Deciduous", dbInteger, 1, dbUpdatableField | dbFixedField);
        vt.CreateField("AvgPrecip", dbSingle, 1, dbUpdatableField | dbFixedField);
        vt.CreateField("Start1000", dbSingle, 1, dbUpdatableField | dbFixedField);
		vt.CreateField("FM1Eq10", dbInteger, 1, dbUpdatableField | dbFixedField);

		vt.CreateField("Aspect", dbText, 1, dbUpdatableField | dbFixedField);
		vt.CreateField("PsnOnSlope", dbText, 1, dbUpdatableField | dbFixedField);

		vt.CreateField("StnType", dbLong, 8, dbUpdatableField | dbFixedField);
		vt.CreateField("Site", dbLong, 8, dbUpdatableField | dbFixedField);

        vt.CreateField("State", dbText, 2, dbUpdatableField | dbFixedField);
        vt.CreateField("County", dbLong, 8, dbUpdatableField | dbFixedField);
        vt.CreateField("ObsAgy", dbLong, 8, dbUpdatableField | dbFixedField);
		vt.CreateField("ObsUnit", dbText, 20, dbUpdatableField | dbFixedField);
		vt.CreateField("FSRegion", dbLong, 8, dbUpdatableField | dbFixedField);

		vt.Append();
		vt.Close();*/
		CString sqlStr =
			"CREATE TABLE [ffpRunWxStation] ("
				"[runID] INTEGER,"
				"[StationID] CHAR(6),"
				"[Name] CHAR(20),"
				"[NFDRSFM] CHAR(1),"
				"[Use88] INTEGER,"
				"[LatDegrees] DOUBLE,"
				"[LongDegrees] DOUBLE,"
				"[Elevation] INTEGER,"
				"[ClimateCls] INTEGER,"
				"[SlopeCls] INTEGER,"
				"[HerbAnnual] INTEGER,"
				"[GreenJulian] DATETIME,"
				"[FreezeJulian] DATETIME,"
				"[StartGreenHerb] INTEGER,"
				"[StartGreenShrub] INTEGER,"
				"[StartKBDI] INTEGER,"
				"[Deciduous] INTEGER,"
				"[AvgPrecip] REAL,"
				"[Start1000] REAL,"
				"[FM1Eq10] INTEGER,"
				"[Aspect] CHAR(1),"
				"[PsnOnSlope] CHAR(1),"
				"[StnType] INTEGER,"
				"[Site] INTEGER,"
				"[State] CHAR(2),"
				"[County] INTEGER,"
				"[ObsAgy] INTEGER,"
				"[ObsUnit] CHAR(20),"
				"[FSRegion] INTEGER,"
				"[WRCC_ID] CHAR(4))";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}
int CFireplusDoc::CreateffpLFITable()
{
	try
	{
		CString sqlStr =
			"CREATE TABLE [ffpLFI] ("
				"[LFIdaysAvg] INTEGER,"
				"[TminMin] DOUBLE,"
				"[TminMax] DOUBLE,"
				"[VPDMin] DOUBLE,"
				"[VPDMax] DOUBLE,"
				"[DaylenMin] DOUBLE,"
				"[DaylenMax] DOUBLE,"
				"[UseVPDAvg] YESNO NOT NULL)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}

	return 1;
}

int CFireplusDoc::CreateffpRunsTable()
{
	try
	{
		CString sqlStr =
			"CREATE TABLE [ffpRuns] ("
				"[runID] INTEGER,"
				"[CreatedOn] CHAR(20),"
				"[LastRun] CHAR(20),"
				"[Description] CHAR(120),"
				"[BatchInc] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpFriskTable()
{
	try
	{
		CString sqlStr =
			"CREATE TABLE [ffpFireRisk] ("
				"[StartYear] INTEGER,"
				"[StartMonth] INTEGER,"
				"[StartDay] INTEGER,"
				"[StartHour] INTEGER,"
				"[StartMin] INTEGER,"
				"[EndYear] INTEGER,"
				"[EndMonth] INTEGER,"
				"[EndDay] INTEGER,"
				"[EndHour] INTEGER,"
				"[EndMin] INTEGER,"
				"[WindType] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpReportsTable()
{
	try
	{
		/*CDaoTableDef vt(m_pDB);
		vt.Create("ffpReports");

		vt.CreateField("reportID", dbInteger,10, dbUpdatableField | dbFixedField);
		vt.CreateField("reportName", dbText, 50, dbUpdatableField | dbFixedField);

		vt.Append();
		vt.Close();*/
		CString sqlStr =
			"CREATE TABLE [ffpReports] ("
				"[reportID] INTEGER,"
				"[reportName] CHAR(50))";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	// populate with report names
    try
    {
	   CReportsSet reportSet(m_pDB);
	   reportSet.Open();

	   for (int i=0; i < NUM_REPORTS; i++){
             reportSet.AddNew();
	    	 reportSet.m_reportID = i;
		     reportSet.m_reportName = ReportNames[i];

			 reportSet.Update();
	   }
	   reportSet.Close();
    }
    catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpRunOptionsTable()
{
	try
	{
		/*CDaoTableDef vt(m_pDB);
		vt.Create("ffpRunOptions");

		vt.CreateField("runID", dbInteger,10, dbUpdatableField | dbFixedField);
		vt.CreateField("reportID",dbInteger,10,dbUpdatableField | dbFixedField);
        vt.CreateField("row", dbInteger,8, dbUpdatableField | dbFixedField);

		vt.CreateField("Variable", dbText, 30, dbUpdatableField | dbFixedField);
		vt.CreateField("Value", dbText, 30, dbUpdatableField | dbFixedField);

		vt.Append();
		vt.Close();*/
		CString sqlStr =
			"CREATE TABLE [ffpRunOptions] ("
				"[runID] INTEGER,"
				"[reportID] INTEGER,"
				"[row] INTEGER,"
				"[Variable] CHAR(30),"
				"[Value] CHAR(30))";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpReportOptionsTable()
{
	try
	{
		/*CDaoTableDef vt(m_pDB);
		vt.Create("ffpReportOptions");

		vt.CreateField("WorkingSet", dbBoolean,1, dbUpdatableField | dbFixedField);

		vt.CreateField("AnnualFilter", dbBoolean, 1, dbUpdatableField | dbFixedField);
		vt.CreateField("SIGStation", dbBoolean, 1, dbUpdatableField | dbFixedField);

		vt.CreateField("Addtl", dbBoolean, 1, dbUpdatableField | dbFixedField);
		vt.CreateField("Delimiter",dbInteger,10, dbUpdatableField | dbFixedField);

		vt.Append();
		vt.Close();*/
		CString sqlStr =
			"CREATE TABLE [ffpReportOptions] ("
				"[WorkingSet] YESNO NOT NULL,"
				"[AnnualFilter] YESNO NOT NULL,"
				"[SIGStation] YESNO NOT NULL,"
				"[Addtl] YESNO NOT NULL,"
				"[Delimiter] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}

	// populate with default values
    try
    {
		CReportOptionsSet reportSet(m_pDB);
		reportSet.Open();
		reportSet.AddNew();
	    reportSet.m_WorkingSet = 1;
		reportSet.m_AnnualFilter = 1;
		reportSet.m_SIGStation = 1;
		reportSet.m_Addtl = 1;
		reportSet.m_Delimiter = 0;
		reportSet.Update();
		reportSet.Close();
    }
    catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

// end added for batch

int CFireplusDoc::CreateffpBatchTable()
{
	try
	{
		/*CDaoTableDef vt(m_pDB);
		vt.Create("ffpBatch");

		vt.CreateField("batchID", dbInteger,10, dbUpdatableField | dbFixedField);

		vt.CreateField("CreatedOn", dbText, 20, dbUpdatableField | dbFixedField);
		vt.CreateField("LastRun", dbText, 20, dbUpdatableField | dbFixedField);

		vt.CreateField("Description", dbText, 50, dbUpdatableField | dbFixedField);

		vt.Append();
		vt.Close();*/
		CString sqlStr =
			"CREATE TABLE [ffpBatch] ("
				"[batchID] INTEGER,"
				"[CreatedOn] CHAR(20),"
				"[LastRun] CHAR(20),"
				"[Description] CHAR(100))";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpBatchRunsTable()
{
	try
	{
		/*CDaoTableDef vt(m_pDB);
		vt.Create("ffpBatchRuns");

		vt.CreateField("batchID", dbInteger,10, dbUpdatableField | dbFixedField);
		vt.CreateField("runID", dbInteger,10, dbUpdatableField | dbFixedField);

		vt.Append();
		vt.Close();*/
		CString sqlStr =
			"CREATE TABLE [ffpBatchRuns] ("
			"[batchID] INTEGER,"
			"[runID] INTEGER)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpBatchOptionsTable()
{
	try
	{
		/*CDaoTableDef vt(m_pDB);
		vt.Create("ffpBatchOptions");

		vt.CreateField("maintScheme", dbInteger,10, dbUpdatableField | dbFixedField);
		vt.CreateField("maintValue", dbInteger,10, dbUpdatableField | dbFixedField);
		vt.CreateField("runDesc",dbText,80,dbUpdatableField | dbFixedField);
		vt.Append();
		vt.Close();*/
		CString sqlStr =
			"CREATE TABLE [ffpBatchOptions] ("
				"[maintScheme] INTEGER,"
				"[maintValue] INTEGER,"
				"[runDesc] CHAR(80),"
				"[PromptDesc] INTEGER )";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}

    // populate with batch option defaults
    try
    {
	   CBatchOptionsSet boSet(m_pDB);
	   boSet.Open();
	   boSet.AddNew();
	   boSet.m_maintScheme = tNone;
	   boSet.m_maintValue = 500;
	   boSet.m_runDesc="";
	   boSet.m_PromptDesc = 0;
	   boSet.Update();
	   boSet.Close();
    }
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

int CFireplusDoc::CreateffpInitTable()
{
	try
	{
	CString sqlStr = "CREATE TABLE [ffpInit] ("
							"[SIG_Station] CHAR(20),"
							"[Year] INTEGER,"
							"[GreenJulian] DATETIME,"
							"[StartKBDI] INTEGER,"
							"[Start1000] REAL,"
							"[FFMC] REAL,"
							"[DMC] REAL,"
							"[DC] REAL,"
							"[Enabled] YESNO NOT NULL,"
							"[StartUpDay] DATETIME )";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

void CFireplusDoc::OnDataUservariablesManage()
{
	bool hasUserVars = false, hasUserVals = false;
	CRecordset trecs(m_pDB);
	try
	{
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpUserVars"))
		{
			hasUserVars = true;
			trecs.Close();
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	try
	{
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM  ffpUserVals"))
		{
			hasUserVals = true;
			trecs.Close();
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	/*CDaoTableDefInfo tInfo;
	for(int i = 0; i < m_pDB->GetTableDefCount(); i++)
	{
		m_pDB->GetTableDefInfo(i, tInfo);
		if(tInfo.m_strName.CompareNoCase("ffpUserVars") == 0)
			hasUserVars = true;
		if(tInfo.m_strName.CompareNoCase("ffpUserVals") == 0)
			hasUserVals = true;
	}*/
	if(!hasUserVars)
	{
		if(!CreateUserVarTable())
		{
			AfxMessageBox("Error: Could not create User Variable table.");
			return;
		}
	}
	if(!hasUserVals)
	{
		if(!CreateUserValTable())
		{
			AfxMessageBox("Error: Could not create User Values table.");
			return;
		}
	}
	CUserVarsDialog uvd;
	uvd.pDB = m_pDB;
	uvd.DoModal();
}

void CFireplusDoc::OnDataUservariablesImportdata()
{
	if(!HasUserTables(m_pDB))
		AfxMessageBox("No User Variables Defined.\nSelect \"Data - User Variables - Manage\"");
	else
	{
		CUserImportDialog uid;
		uid.pDB = m_pDB;
		uid.DoModal();
	}
}

void CFireplusDoc::OnUpdateDataUservariablesImportdata(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

bool HasUserTables(CDatabase *_pDB)
{
	bool hasUserVars = false, hasUserVals = false;
	try
	{
		CRecordset trecs(_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpUserVars"))
		{
			hasUserVars = true;
			trecs.Close();
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	try
	{
		CRecordset trecs(_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM  ffpUserVals"))
		{
			hasUserVals = true;
			trecs.Close();
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if(hasUserVars && hasUserVals)
		return true;
	return false;
}

bool CFireplusDoc::HasTempWx()
{
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpTempWx"))
		{
			trecs.Close();
			return true;
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	/*CDaoTableDefInfo tInfo;
	for(int i = 0; i < m_pDB->GetTableDefCount(); i++)
	{
		m_pDB->GetTableDefInfo(i, tInfo);
		if(tInfo.m_strName.CompareNoCase("ffpTempWx") == 0)
			return true;
	}*/
	return false;
}

// new 2013
bool CFireplusDoc::HasDLOutput()
{
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM DL_Output"))
		{
			trecs.Close();
			return true;
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	
	return false;
}

// new 2015
bool CFireplusDoc::HasHLOutput()
{
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM HL_Output"))
		{
			trecs.Close();
			return true;
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	
	return false;
}

bool CFireplusDoc::HasTempFires()
{
	try
	{
		CRecordset trecs(m_pDB);
		if(trecs.Open(CRecordset::dynaset, "SELECT * FROM ffpTempFire"))
		{
			trecs.Close();
			return true;
		}
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	/*CDaoTableDefInfo tInfo;
	for(int i = 0; i < m_pDB->GetTableDefCount(); i++)
	{
		m_pDB->GetTableDefInfo(i, tInfo);
		if(tInfo.m_strName.CompareNoCase("ffpTempFire") == 0)
			return true;
	}*/
	return false;
}

void CFireplusDoc::CreateTempWx()
{
/*	try
	{
		CDaoFieldInfo fInfo;
		CDaoTableDef wx(m_pDB);
		wx.Open("WXObs");
		CDaoTableDef vt(m_pDB);
		vt.Create("ffpTempWx");

		for(int i = 0; i < wx.GetFieldCount(); i++)
		{
			wx.GetFieldInfo(i, fInfo, AFX_DAO_PRIMARY_INFO);//AFX_DAO_ALL_INFO);
		//	fInfo.
			//vt.CreateField(fInfo);
			vt.CreateField(fInfo.m_strName, fInfo.m_nType, fInfo.m_lSize, fInfo.m_lAttributes);
		}
		wx.Close();
		vt.Append();
		vt.Close();
	}
	catch( CDaoException* e )
	{
		DisplayDaoException(e);
		e->Delete();
	}*/
	try
	{
		CString strSql =
			"CREATE TABLE [ffpTempWx] ("
			"[StationID] CHAR(6),"
			"[ObsDate] DATETIME NOT NULL,"
			"[ObsType] CHAR(1),"
			"[UserWX] INTEGER,"
			"[SOW] INTEGER,"
			"[Temp] INTEGER,"
			"[RH] INTEGER,"
			"[WDir] INTEGER,"
			"[WS] INTEGER,"
			"[OMC10] REAL,"
			"[OMCWood] REAL,"
			"[OMCWoodDate] DATETIME,"
			"[TmpMax] INTEGER,"
			"[TmpMin] INTEGER,"
			"[RHMax] INTEGER,"
			"[RHMin] INTEGER,"
			"[PPTDUR] INTEGER,"
			"[PPTAMT] REAL,"
			"[Season] INTEGER,"
			"[GreenHerb] INTEGER,"
			"[GreenShrub] INTEGER,"
			"[MC1] REAL,"
			"[MC10] REAL,"
			"[MC100] REAL,"
			"[MC1000] REAL,"
			"[X1000] REAL,"
			"[MCHerb] REAL,"
			"[MCWood] REAL,"
			"[GREN] INTEGER,"
			"[HStage] INTEGER,"
			"[ROS] REAL,"
			"[ERC] REAL,"
			"[BI] INTEGER,"
			"[FIL] INTEGER,"
			"[CORCT2] INTEGER,"
			"[SolarRadiation] INTEGER,"
			"[WetFlag] INTEGER,"
			"[GustDir] INTEGER,"
			"[GustSpd] INTEGER,"
			"[HourlyPrecip] REAL,"
			"[SR_SOW] INTEGER,"
			"[SR_WetFlag] INTEGER,"
			"[SnowFlag] INTEGER,"
			"[Hour] INTEGER,"
			"[DailyObs] INTEGER,"
			"[FM_1] REAL,"
			"[FM_10] REAL,"
			"[FM_100] REAL,"
			"[FM_1000] REAL,"
			"[FuelTemperature] REAL)";
			m_pDB->ExecuteSQL(strSql);
	}
	catch(CDBException* e)
	{
		DisplayDBException(e);
		e->Delete();
	}
}
// new 2013
// see ClimAnalysis->DailyListing()
void CFireplusDoc::CreateDLOutput()
{

	try
	{
		CString strSql =
			"CREATE TABLE [DL_Output] ("
            "[ID] INTEGER,"
			"[ObsDate] VARCHAR(30) NOT NULL,"
			"CONSTRAINT IDConstraint PRIMARY KEY ([ID]))";
		
		m_pDB->ExecuteSQL(strSql);
	}
	catch(CDBException* e)
	{
		DisplayDBException(e);
		e->Delete();
	}
}

// new 2015
void CFireplusDoc::CreateHLOutput()
{

	try
	{
		CString strSql =
			"CREATE TABLE [HL_Output] ("
            "[ID] INTEGER,"
			"[ObsDate] VARCHAR(30) NOT NULL,"
			"CONSTRAINT IDConstraint PRIMARY KEY ([ID]))";
		
		m_pDB->ExecuteSQL(strSql);
	}
	catch(CDBException* e)
	{
		DisplayDBException(e);
		e->Delete();
	}
}


void CFireplusDoc::CreateTempFires()
{
/*	try
	{
		CDaoFieldInfo fInfo;
		CDaoTableDef fire(m_pDB);
		fire.Open("Fire");
		CDaoTableDef vt(m_pDB);
		vt.Create("ffpTempFire");

		for(int i = 0; i < fire.GetFieldCount(); i++)
		{
			fire.GetFieldInfo(i, fInfo, AFX_DAO_PRIMARY_INFO);
			vt.CreateField(fInfo.m_strName, fInfo.m_nType, fInfo.m_lSize, fInfo.m_lAttributes);
		}
		fire.Close();
		vt.Append();
		vt.Close();
	}
	catch( CDaoException* e )
	{
		DisplayDaoException(e);
		e->Delete();
	}
	*/
	try
	{
		CString strSql =
			"CREATE TABLE [ffpTempFire] ("
				"[FireID] INTEGER ,"
				"[Year] CHAR(4),"
				"[AgencyID] INTEGER ,"
				"[RegionID] INTEGER ,"
				"[UnitID] INTEGER ,"
				"[SubunitID] INTEGER ,"
				"[FireNumber] CHAR(12),"
				"[State] CHAR(2),"
				"[County] INTEGER ,"
				"[TotalAcres] DOUBLE ,"
				"[SizeClass] CHAR(1),"
				"[FireName] CHAR(25),"
				"[StatisticalCause] INTEGER ,"
				"[SpecificCause] INTEGER ,"
				"[GeneralCause] INTEGER ,"
				"[ClassPeople] INTEGER ,"
				"[CauseNarr] CHAR(50),"
				"[Township] CHAR(5),"
				"[Range] CHAR(5),"
				"[Section] INTEGER ,"
				"[SubSection] CHAR(4),"
				"[LatDD] INTEGER ,"
				"[LatMM] INTEGER ,"
				"[LatSS] INTEGER ,"
				"[LonDD] INTEGER ,"
				"[LonMM] INTEGER ,"
				"[LonSS] INTEGER ,"
				"[Other] CHAR(50),"
				"[WildNum] INTEGER ,"
				"[Notes] CHAR(50),"
				"[Ignition] DATETIME,"
				"[Discovery] DATETIME,"
				"[FirstAttack] DATETIME,"
				"[Reinforcement] DATETIME,"
				"[DeclareWildlandFire] DATETIME,"
				"[Contain] DATETIME,"
				"[StrategyMet] DATETIME,"
				"[FireOut] DATETIME,"
				"[Slope] CHAR(1),"
				"[Elevation] CHAR(1),"
				"[Aspect] CHAR(1),"
				"[FuelModel] CHAR(1),"
				"[FireType] CHAR(1),"
				"CONSTRAINT PrimaryKey PRIMARY KEY ([FireID]))";

		m_pDB->ExecuteSQL(strSql);
	}
	catch(CDBException* e)
	{
		DisplayDBException(e);
		e->Delete();
	}
}

// added 9/2012
// currently just checks the batch autosave path
void CFireplusDoc::CheckPaths()
{
    CGraphSet rOpts(m_pDB);
	rOpts.Open();
	//BOOL autoSave = rOpts.m_AutoSave;
	CString savePath = theApp.dbDir;//docDir;//theApp.workDir;
	if(!rOpts.IsEOF())
	{
		savePath = rOpts.m_AutoSaveLocation;
	}

	savePath.Trim();
	rOpts.Close();

	// maintain current dir
	char dirBuf[255];
	_getcwd(dirBuf,255);
	if (chdir(savePath) != 0)
	{
	   // problem with saveDir
	   CString strMsg;

	   savePath.Trim();
	   strMsg.Format(
			_T("ERROR: The Batch AutoSave directory '%s' was invalid.\n\nBatch files will now be autosaved to '%s'.\n\nThis location may be changed in the Batch AutoSave options menu.\n\n"),savePath,theApp.dbDir);
		AfxMessageBox(strMsg);
		savePath = theApp.dbDir;
	    savePath.Trim();
		rOpts.Open();

		if(!rOpts.IsEOF())
		{
			rOpts.Edit();
			rOpts.m_AutoSaveLocation = savePath;
			rOpts.Update();
		}

		rOpts.Close();
	}

    chdir(dirBuf);
}

// added for batch2
void CFireplusDoc::CheckGraphOptionsTable()
{
	// adds new fields to ffpGraphOptions table if necessary
	bool hasAutoSave = false, hasAutoSaveWidth = false, hasAutoSaveHeight = false, hasAutoSaveFileType = false, hasAutoSaveLocation = false;
	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "ffpGraphOptions";
		columns.Open();
		while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("AutoSave") == 0)
				hasAutoSave = true;
			if(columns.m_strColumnName.CompareNoCase("AutoSaveWidth") == 0)
				hasAutoSaveWidth = true;
			if(columns.m_strColumnName.CompareNoCase("AutoSaveHeight") == 0)
				hasAutoSaveHeight = true;
			if(columns.m_strColumnName.CompareNoCase("AutoSaveFileType") == 0)
				hasAutoSaveFileType = true;
			if(columns.m_strColumnName.CompareNoCase("AutoSaveLocation") == 0)
				hasAutoSaveLocation = true;
			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if(!hasAutoSave)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpGraphOptions] ADD [AutoSave] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
		//td.CreateField("AutoSave", dbBoolean, 1, dbFixedField | dbUpdatableField);
	}
	if(!hasAutoSaveWidth)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpGraphOptions] ADD [AutoSaveWidth] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}

		ffp4upgrade = true;
		//td.CreateField("AutoSaveWidth", dbInteger,4 , dbFixedField | dbUpdatableField);
	}
	if(!hasAutoSaveHeight)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpGraphOptions] ADD [AutoSaveHeight] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
		//td.CreateField("AutoSaveHeight", dbInteger, 4 , dbFixedField | dbUpdatableField);
	}
	if(!hasAutoSaveFileType)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpGraphOptions] ADD [AutoSaveFileType] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
		//td.CreateField("AutoSaveFileType", dbInteger, 2 , dbFixedField | dbUpdatableField);
	}
	if(!hasAutoSaveLocation)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpGraphOptions] ADD [AutoSaveLocation] CHAR(255)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
		//td.CreateField("AutoSaveLocation", dbText, 120 , dbFixedField | dbUpdatableField);
	}
	if (!hasAutoSave){
		CGraphSet goSet(m_pDB);
		if (! goSet.IsOpen())
			goSet.Open();
		goSet.MoveFirst();

		goSet.Edit();
		goSet.m_AutoSave = 0;
		goSet.m_AutoSaveWidth = 640;
		goSet.m_AutoSaveHeight = 480;
		goSet.m_AutoSaveFileType = 0;
		goSet.m_AutoSaveLocation = ".";
		goSet.Update();
		goSet.Close();
		ffp4upgrade = true;
	}
/*	CDaoTableDef td(m_pDB);
	td.Open("ffpGraphOptions");
	bool hasAutoSave = false, hasAutoSaveWidth = false, hasAutoSaveHeight = false, hasAutoSaveFileType = false, hasAutoSaveLocation = false;

	for(int f = 0; f < td.GetFieldCount(); f++)
	{
		CDaoFieldInfo fInfo;
		td.GetFieldInfo(f, fInfo);
		if(fInfo.m_strName.CompareNoCase("AutoSave") == 0)
			hasAutoSave = true;
		if(fInfo.m_strName.CompareNoCase("AutoSaveWidth") == 0)
			hasAutoSaveWidth = true;
		if(fInfo.m_strName.CompareNoCase("AutoSaveHeight") == 0)
			hasAutoSaveHeight = true;
		if(fInfo.m_strName.CompareNoCase("AutoSaveFileType") == 0)
			hasAutoSaveFileType = true;
		if(fInfo.m_strName.CompareNoCase("AutoSaveLocation") == 0)
			hasAutoSaveLocation = true;
	}
	if(!hasAutoSave)
		td.CreateField("AutoSave", dbBoolean, 1, dbFixedField | dbUpdatableField);
	if(!hasAutoSaveWidth)
		td.CreateField("AutoSaveWidth", dbInteger,4 , dbFixedField | dbUpdatableField);
	if(!hasAutoSaveHeight)
		td.CreateField("AutoSaveHeight", dbInteger, 4 , dbFixedField | dbUpdatableField);
	if(!hasAutoSaveFileType)
		td.CreateField("AutoSaveFileType", dbInteger, 2 , dbFixedField | dbUpdatableField);

	if(!hasAutoSaveLocation)
		td.CreateField("AutoSaveLocation", dbText, 120 , dbFixedField | dbUpdatableField);

	td.Close();

	if (!hasAutoSave){
		CGraphSet goSet(m_pDB);
		if (! goSet.IsOpen())
			goSet.Open();
		goSet.MoveFirst();

		goSet.Edit();
		goSet.m_AutoSave = 0;
		goSet.m_AutoSaveWidth = 640;
		goSet.m_AutoSaveHeight = 480;
		goSet.m_AutoSaveFileType = 3;
		goSet.m_AutoSaveLocation = ".";
		goSet.Update();
		goSet.Close();
	}
*/
}
// end added for batch2

void CFireplusDoc::CheckOptsTable()
{
	//add fields previously stored in DocOpts test file if not already present
	bool hasOverlayYear1 = false,
		hasOverlayYear2 = false,
		hasOverlayYear3 = false,
		hasOverlayColor1 = false,
		hasOverlayColor2 = false,
		hasOverlayColor3 = false,
		hasOverlayWidth1 = false,
		hasOverlayWidth2 = false,
		hasOverlayWidth3 = false,
		hasOverlayStyle1 = false,
		hasOverlayStyle2 = false,
		hasOverlayStyle3 = false,
		hasMergeColor1 = false,
		hasMergeColor2 = false,
		hasMergeColor3 = false,
		hasMergeColor4 = false,
		hasMergeStyle1 = false,
		hasMergeStyle2 = false,
		hasMergeStyle3 = false,
		hasMergeStyle4 = false,
		hasMergeWidth1 = false,
		hasMergeWidth2 = false,
		hasMergeWidth3 = false,
		hasMergeWidth4 = false,
		hasEnableFuture = false,
		hasVarSortOrder = false,
		hasDLExport = false;

	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "ffpOpts";
		columns.Open();
		while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("OverlayYear1") == 0)
				hasOverlayYear1 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayYear2") == 0)
				hasOverlayYear2 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayYear3") == 0)
				hasOverlayYear3 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayColor1") == 0)
				hasOverlayColor1 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayColor2") == 0)
				hasOverlayColor2 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayColor3") == 0)
				hasOverlayColor3 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayWidth1") == 0)
				hasOverlayWidth1 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayWidth2") == 0)
				hasOverlayWidth2 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayWidth3") == 0)
				hasOverlayWidth3 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayStyle1") == 0)
				hasOverlayStyle1 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayStyle2") == 0)
				hasOverlayStyle2 = true;
			if(columns.m_strColumnName.CompareNoCase("OverlayStyle3") == 0)
				hasOverlayStyle3 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeColor1") == 0)
				hasMergeColor1 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeColor2") == 0)
				hasMergeColor2 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeColor3") == 0)
				hasMergeColor3 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeColor4") == 0)
				hasMergeColor4 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeStyle1") == 0)
				hasMergeStyle1 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeStyle2") == 0)
				hasMergeStyle2 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeStyle3") == 0)
				hasMergeStyle3 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeStyle4") == 0)
				hasMergeStyle4 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeWidth1") == 0)
				hasMergeWidth1 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeWidth2") == 0)
				hasMergeWidth2 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeWidth3") == 0)
				hasMergeWidth3 = true;
			if(columns.m_strColumnName.CompareNoCase("MergeWidth4") == 0)
				hasMergeWidth4 = true;
			if(columns.m_strColumnName.CompareNoCase("EnableFuture") == 0)
				hasEnableFuture = true;
			if(columns.m_strColumnName.CompareNoCase("VarSortOrder") == 0)
				hasVarSortOrder = true;
			if(columns.m_strColumnName.CompareNoCase("DL_Export") == 0)
				hasDLExport = true;
			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if(!hasOverlayYear1)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayYear1] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayYear2)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayYear2] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayYear3)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayYear3] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayColor1)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayColor1] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayColor2)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayColor2] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayColor3)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayColor3] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayWidth1)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayWidth1] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayWidth2)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayWidth2] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayWidth3)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayWidth3] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayStyle1)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayStyle1] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayStyle2)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayStyle2] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasOverlayStyle3)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [OverlayStyle3] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeColor1)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeColor1] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeColor2)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeColor2] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeColor3)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeColor3] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeColor4)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeColor4] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeStyle1)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeStyle1] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeStyle2)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeStyle2] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeStyle3)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeStyle3] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeStyle4)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeStyle4] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeWidth1)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeWidth1] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeWidth2)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeWidth2] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeWidth3)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeWidth3] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasMergeWidth4)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [MergeWidth4] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasEnableFuture)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [EnableFuture] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasVarSortOrder)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [VarSortOrder] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasDLExport)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpOpts] ADD [DL_Export] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}

		try
		{
			CString strSql;
			strSql = "UPDATE [ffpOpts] SET [DL_Export]=0 WHERE [VarSortOrder] >= 0";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
}

void CFireplusDoc::CheckSeasonBinsTable()
{
	CSeasonBinSet binSet(m_pDB);
	CClimateSet climSet(m_pDB);
//	climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
	binSet.Open();
	climSet.Open();
	while(!binSet.IsEOF())
	{
		climSet.m_strFilter.Format("[VarID] = %d", binSet.m_VarID);
		climSet.Requery();
		if(climSet.IsEOF() && climSet.IsBOF())
			binSet.Delete();
		binSet.MoveNext();
	}
	binSet.Close();
	climSet.Close();
}

bool CFireplusDoc::NewCheckClimateOptionsTable()
{//this version replaces previous version, using the ffpClimateOptions table in the default database as a template

	bool addedColumns = false;
	CString master_path;
	master_path.Format("%s\\ffplus5.ffp", theApp.workDir);

	if (access(master_path, 0) != 0)
	{
		CString msg;
		msg.Format("Error: Master Database does not exist:\n%s", master_path);
		AfxMessageBox(msg);
		return false;
	}

	CDatabase masterDB;

	CString strConnect;
	strConnect.Format("Provider=MSDASQL;Driver={Microsoft Access Driver (*.mdb)};"
		"Dbq=%s;Uid=admin;Pwd=;", master_path);
	try
	{
		masterDB.OpenEx(strConnect, CDatabase::noOdbcDialog);
	}
	catch (CDBException* e)
	{

		DisplayDBException(e);
		e->Delete();
		return false;
	}
	//first, check all the fields to make sure thay match
	try
	{
		CColumns masterColumns(&masterDB);
		masterColumns.m_strTableNameParam = "ffpClimateOptions";
		masterColumns.Open();
		while (!masterColumns.IsEOF())
		{
			CString findColumn = masterColumns.m_strColumnName;
			bool foundColumn = false;
			CColumns columns(m_pDB);
			columns.m_strTableNameParam = "ffpClimateOptions";
			columns.Open();
			while (!columns.IsEOF())
			{
				if (findColumn.CompareNoCase(columns.m_strColumnName) == 0)
				{
					foundColumn = true;
					break;
				}
				columns.MoveNext();
			}
			columns.Close();
			if (!foundColumn)
			{
				try
				{
					CString strSql;
					strSql.Format("ALTER TABLE [ffpClimateOptions] ADD [%s] %s", findColumn, masterColumns.m_strTypeName);
					m_pDB->ExecuteSQL(strSql);
					addedColumns = true;
				}
				catch (CDBException* e)
				{
					e->Delete();
					return false;
				}
			}
			masterColumns.MoveNext();
		}
		masterColumns.Close();
	}
	catch (CDBException* e)
	{
		e->Delete();
		return false;
	}


	//ok, columns are now correct. Check to ensure records match regarding Variable Name and VarID!!!!! (as well as number of records!)
	bool misMatch = false;
	if (addedColumns)
		misMatch = true;
	CClimateSet masterClimSet(&masterDB);
	CClimateSet climSet(m_pDB);
	masterClimSet.m_strSort = "[VarID]";
	masterClimSet.Open();
	climSet.Open();
	while (!masterClimSet.IsEOF() && !misMatch)
	{
		climSet.m_strFilter.Format("[VarID] = %d", masterClimSet.m_VarID);
		climSet.Requery();
		if (!climSet.IsEOF())
		{
			if (masterClimSet.m_Variable_Name.Compare(climSet.m_Variable_Name) != 0)
				misMatch = true;
		}
		else
			misMatch = true;
		masterClimSet.MoveNext();
	}

	if (misMatch)
	{
		//here we've determined a difference in either structure or fields, let's just copy the master Data into local ClimSet
		//first, delete all the local records
		climSet.m_strFilter.Format("");
		climSet.Requery();
		while (!climSet.IsEOF())
		{
			climSet.Delete();
			climSet.MoveNext();
		}
		masterClimSet.MoveFirst();
		while (!masterClimSet.IsEOF())
		{
			climSet.AddNew();
			climSet.m_VarID = masterClimSet.m_VarID;
			climSet.m_Variable_Name = masterClimSet.m_Variable_Name;
			climSet.m_ShortName = masterClimSet.m_ShortName;
			climSet.m_BinSize = masterClimSet.m_BinSize;
			climSet.m_OptionType = masterClimSet.m_OptionType;
			climSet.m_Stats_Table = FALSE;
			climSet.m_Stats_Graph = FALSE;
			climSet.m_Daily_Freqs = FALSE;
			climSet.m_Period_Mins = FALSE;
			climSet.m_Period_Maxs = FALSE;
			climSet.m_Data_Count = FALSE;
			climSet.m_CriticalPercentile = masterClimSet.m_CriticalPercentile;
			climSet.m_cp2 = masterClimSet.m_cp2;
			climSet.m_Daily_List = 0;
			climSet.m_FilterValue = masterClimSet.m_FilterValue;
			climSet.Update();
			masterClimSet.MoveNext();
		}
		//also, need to copy ffpSeasonBin table
		CSeasonBinSet binSet(m_pDB);
		binSet.Open();
		while (!binSet.IsEOF())
		{
			binSet.Delete();
			binSet.MoveNext();
		}
		CSeasonBinSet masterBinSet(&masterDB);
		masterBinSet.Open();
		while (!masterBinSet.IsEOF())
		{
			binSet.AddNew();
			binSet.m_VarID = masterBinSet.m_VarID;
			binSet.m_Bin1Max = masterBinSet.m_Bin1Max;
			binSet.m_Bin2Max = masterBinSet.m_Bin2Max;
			binSet.m_Bin3Max = masterBinSet.m_Bin3Max;
			binSet.m_Bin4Max = masterBinSet.m_Bin4Max;
			binSet.m_Bin5Max = masterBinSet.m_Bin5Max;
			binSet.m_Bin6Max = masterBinSet.m_Bin6Max;
			binSet.m_Bin1Min = masterBinSet.m_Bin1Min;
			binSet.m_Bin2Min = masterBinSet.m_Bin2Min;
			binSet.m_Bin3Min = masterBinSet.m_Bin3Min;
			binSet.m_Bin4Min = masterBinSet.m_Bin4Min;
			binSet.m_Bin5Min = masterBinSet.m_Bin5Min;
			binSet.m_Bin6Min = masterBinSet.m_Bin6Min;
			binSet.Update();
			masterBinSet.MoveNext();
		}
		masterBinSet.Close();
		binSet.Close();
	}
	climSet.Close();
	masterClimSet.Close();
	masterDB.Close();
	return true;
}

void CFireplusDoc::CheckClimateOptionsTable()
{
	//this function adds new wxObs fields records to ClimateOptions: WetFlag, SolarRadiation, GustDir and GustSpd
	//STATICVARIDS has been increased to allow room for these fields
	//Before adding these variables and assigning varIDS, must deal with any previously created user variables
	bool hasSolarRadiation = false,
		hasWetFlag = false,
		hasGustDir = false,
		hasGustSpd = false,
		hasDPT = false,
		hasVPDmax = false,
		hasVPDavg = false,
		hasLFI = false,
		hasLfiHerb = false,
		hasLfiWoody = false,
		hasFFWI = false,
		hasHourlyPcp = false,
		hasWindAzimuth = false,
		hasGSI = false,
		hasSnowFlag = false,
		hasFilterValue = false;
//first, check structure for new FilterValue field
	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "ffpClimateOptions";
		columns.Open();
		while (!columns.IsEOF())
		{
			if (columns.m_strColumnName.CompareNoCase("FilterValue") == 0)
				hasFilterValue = true;

			columns.MoveNext();
		}
		columns.Close();
	}
	catch (CDBException* e)
	{
		e->Delete();
	}
	if (!hasFilterValue)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpClimateOptions] ADD [FilterValue] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
		//now need to populate
		CClimateSet climSet(m_pDB);
		climSet.Open();
		while (!climSet.IsEOF())
		{
			/*int cp1 = climSet.m_CriticalPercentile;
			int cp2 = climSet.m_cp2;
			if (cp2 < cp1)
			{
				climSet.Edit();
				climSet.m_FilterValue = 1000.0;
				climSet.Update();
			}
			else
			{
				climSet.Edit();
				climSet.m_FilterValue = -999.0;
				climSet.Update();
			}*/
			climSet.Edit();
			climSet.SetFieldNull(&climSet.m_FilterValue);
			climSet.MoveNext();
		}
		climSet.Close();
	}

	//then, must adjust any user vars...
	int nextVarID = STATICVARIDS + 1;
	CUserVarSet uVarSet(m_pDB);
	uVarSet.m_strSort = "[VarID]";
	uVarSet.Open();
	if(!uVarSet.IsEOF())
	{
		uVarSet.MoveLast();
		if(uVarSet.m_VarID > STATICVARIDS)
			nextVarID = uVarSet.m_VarID + 1;
	}
	uVarSet.m_strFilter.Format("[VarID] > %d AND [VarID] <= %d", STATICVARIDS - 12, STATICVARIDS);
	uVarSet.Requery();
	while(!uVarSet.IsEOF())
	{//user variable that must get a new VarID
		int oldVar = uVarSet.m_VarID;
		//deal with linked tables to maintain referential integrity
		CSeasonBinSet binSet(m_pDB);
		binSet.m_strFilter.Format("[VarID] = %d", oldVar);
		binSet.Open();
		if(!binSet.IsEOF())
		{
			binSet.Edit();
			binSet.m_VarID = nextVarID;
			binSet.Update();
		}
		binSet.Close();
		CClimateSet climSet(m_pDB);
		climSet.m_strSort = "[VarID]";
		climSet.m_strFilter.Format("[VarID] = %d", oldVar);
		climSet.Open();
		if(!climSet.IsEOF())
		{
			climSet.Edit();
			climSet.m_VarID = nextVarID;
			climSet.Update();
		}
		climSet.Close();
		uVarSet.Edit();
		uVarSet.m_VarID = nextVarID;
		uVarSet.Update();
		uVarSet.Requery();
		nextVarID++;
	}
	uVarSet.Close();

	//ok, user variables dealt with, must now check for
	//and add (if necessary) records to ClimateOptions for new static variables
	CSeasonBinSet binSet(m_pDB);
	binSet.Open();
	CClimateSet climSet(m_pDB);
	climSet.m_strSort = "[VarID]";

	//GustDir
	climSet.m_strFilter = "[VarID] = 33";
	climSet.Open();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Gust Direction") != 0)
		{//replace with gust direction
			climSet.Edit();
		}
		else
			hasGustDir = true;
	}
	else
		climSet.AddNew();
	if(!hasGustDir)
	{
		climSet.m_VarID = 33;
		climSet.m_Variable_Name = "Gust Direction";
		climSet.m_ShortName = "GustDir";
		climSet.m_BinSize = 1;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 75;
		climSet.m_cp2 = 90;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 33";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 33;
		binSet.Update();
	}

	//Gust Speed
	climSet.m_strFilter = "[VarID] = 34";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Gust Speed") != 0)
		{//replace with gust direction
			climSet.Edit();
		}
		else
			hasGustSpd = true;
	}
	else
		climSet.AddNew();
	if(!hasGustSpd)
	{
		climSet.m_VarID = 34;
		climSet.m_Variable_Name = "Gust Speed";
		climSet.m_ShortName = "GustSpd";
		climSet.m_BinSize = 5;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 75;
		climSet.m_cp2 = 90;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 34";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 34;
		binSet.Update();
	}
	//Solar Radiation
	climSet.m_strFilter = "[VarID] = 35";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Solar Radiation") != 0)
		{//replace with gust direction
			climSet.Edit();
		}
		else
			hasSolarRadiation = true;
	}
	else
		climSet.AddNew();
	if(!hasSolarRadiation)
	{
		climSet.m_VarID = 35;
		climSet.m_Variable_Name = "Solar Radiation";
		climSet.m_ShortName = "SolRad";
		climSet.m_BinSize = 20;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 75;
		climSet.m_cp2 = 90;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 35";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 35;
		binSet.Update();
	}
	//Wet Flag
	climSet.m_strFilter = "[VarID] = 36";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Wet Flag") != 0)
		{//replace with gust direction
			climSet.Edit();
		}
		else
			hasWetFlag = true;
	}
	else
		climSet.AddNew();
	if(!hasWetFlag)
	{
		climSet.m_VarID = 36;
		climSet.m_Variable_Name = "Wet Flag";
		climSet.m_ShortName = "WetFlag";
		climSet.m_BinSize = 1;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 50;
		climSet.m_cp2 = 50;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 36";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 36;
		binSet.Update();
	}
	//Dew Point
	climSet.m_strFilter = "[VarID] = 37";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Dew Point Temperature") != 0)
		{//replace with Dew Point Temperature
			climSet.Edit();
		}
		else
			hasDPT = true;
	}
	else
		climSet.AddNew();
	if(!hasDPT)
	{
		climSet.m_VarID = 37;
		climSet.m_Variable_Name = "Dew Point Temperature";
		climSet.m_ShortName = "DPT";
		climSet.m_BinSize = 1;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 90;
		climSet.m_cp2 = 95;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 37";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 37;
		binSet.Update();
	}

	//Vapor Pressure Deficit
	climSet.m_strFilter = "[VarID] = 38";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Vapor Pressure Deficit Max") != 0)
		{//replace with Vapor Pressure Deficit
			climSet.Edit();
		}
		else
			hasVPDmax = true;
	}
	else
		climSet.AddNew();
	if(!hasVPDmax)
	{
		climSet.m_VarID = 38;
		climSet.m_Variable_Name = "Vapor Pressure Deficit Max";
		climSet.m_ShortName = "VPDmax";
		climSet.m_BinSize = 100;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 90;
		climSet.m_cp2 = 95;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 38";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 38;
		binSet.Update();
	}

	//Vapor Pressure Deficit Avg
	climSet.m_strFilter = "[VarID] = 39";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Vapor Pressure Deficit Avg") != 0)
		{//replace with Vapor Pressure Deficit
			climSet.Edit();
		}
		else
			hasVPDavg = true;
	}
	else
		climSet.AddNew();
	if(!hasVPDavg)
	{
		climSet.m_VarID = 39;
		climSet.m_Variable_Name = "Vapor Pressure Deficit Avg";
		climSet.m_ShortName = "VPDavg";
		climSet.m_BinSize = 100;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 90;
		climSet.m_cp2 = 95;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 39";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 39;
		binSet.Update();
	}

	//Live Fuel Index
	climSet.m_strFilter = "[VarID] = 40";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Growing Season Index") != 0)
		{//replace with Live Fuel Index
			climSet.Edit();
		}
		else
			hasGSI = true;
	}
	else
		climSet.AddNew();
	if(!hasGSI)
	{
		climSet.m_VarID = 40;
		climSet.m_Variable_Name = "Growing Season Index";
		climSet.m_ShortName = "GSI";
		climSet.m_BinSize = 0.1;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 90;
		climSet.m_cp2 = 95;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 40";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 40;
		binSet.Update();
	}

	//Wind Azimuth
	climSet.m_strFilter = "[VarID] = 41";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Wind Azimuth") != 0)
		{//replace with wind azimuth
			climSet.Edit();
		}
		else
			hasWindAzimuth = true;
	}
	else
		climSet.AddNew();
	if(!hasWindAzimuth)
	{
		climSet.m_VarID = 41;
		climSet.m_Variable_Name = "Wind Azimuth";
		climSet.m_ShortName = "WindAzi";
		climSet.m_BinSize = 1;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 75;
		climSet.m_cp2 = 90;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 41";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 41;
		binSet.Update();
	}

	//Hourly Precip
	climSet.m_strFilter = "[VarID] = 42";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Hourly Precip") != 0)
		{//replace with wind azimuth
			climSet.Edit();
		}
		else
			hasWindAzimuth = true;
	}
	else
		climSet.AddNew();
	if(!hasWindAzimuth)
	{
		climSet.m_VarID = 42;
		climSet.m_Variable_Name = "Hourly Precip";
		climSet.m_ShortName = "HourPcp";
		climSet.m_BinSize = 1;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 75;
		climSet.m_cp2 = 90;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 42";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 42;
		binSet.Update();
	}

		//LFI Herb
	climSet.m_strFilter = "[VarID] = 43";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("GSI HerbFM") != 0)
		{//replace with Live Fuel Index
			climSet.Edit();
		}
		else
			hasLfiHerb = true;
	}
	else
		climSet.AddNew();
	if(!hasLfiHerb)
	{
		climSet.m_VarID = 43;
		climSet.m_Variable_Name = "GSI HerbFM";
		climSet.m_ShortName = "GSIH";
		climSet.m_BinSize = 1;
		climSet.m_OptionType = 1;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 90;
		climSet.m_cp2 = 95;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 43";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 43;
		binSet.Update();
	}
		//LFI Woody
	climSet.m_strFilter = "[VarID] = 44";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("GSI WoodyFM") != 0)
		{//replace with Live Fuel Index
			climSet.Edit();
		}
		else
			hasLfiWoody = true;
	}
	else
		climSet.AddNew();
	if(!hasLfiWoody)
	{
		climSet.m_VarID = 44;
		climSet.m_Variable_Name = "GSI WoodyFM";
		climSet.m_ShortName = "GSIW";
		climSet.m_BinSize = 1;
		climSet.m_OptionType = 1;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 90;
		climSet.m_cp2 = 95;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 44";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 44;
		binSet.Update();
	}

		//FFWI (FOSBERG Index)
	climSet.m_strFilter = "[VarID] = 45";
	climSet.Requery();
	if(!climSet.IsEOF())
	{//might already be upgraded... check field
		if(climSet.m_Variable_Name.Compare("Fosberg FWI") != 0)
		{//replace with Live Fuel Index
			climSet.Edit();
		}
		else
			hasFFWI = true;
	}
	else
		climSet.AddNew();
	if(!hasFFWI)
	{
		climSet.m_VarID = 45;
		climSet.m_Variable_Name = "Fosberg FWI";
		climSet.m_ShortName = "FFWI";
		climSet.m_BinSize = 1;
		climSet.m_OptionType = 0;
		climSet.m_Stats_Graph = climSet.m_Stats_Table = climSet.m_Daily_Freqs = climSet.m_Data_Count
			= climSet.m_Period_Mins = climSet.m_Period_Maxs = FALSE;
		climSet.m_Daily_List = 0L;
		climSet.m_CriticalPercentile = 90;
		climSet.m_cp2 = 95;
		climSet.Update();
	}
	binSet.m_strFilter = "[VarID] = 45";
	binSet.Requery();
	if(binSet.IsBOF() || binSet.IsEOF())
	{
		binSet.AddNew();
		binSet.m_VarID = 45;
		binSet.Update();
	}

	//X1000 set OptionType to -1 (deprecate it!)
	//2018/04/04 - replace x1000 with SnowFlag
	climSet.m_strFilter = "[VarID] = 23";
	climSet.Requery();
	if (!climSet.IsEOF())
	{//might already be upgraded... check field
		if (climSet.m_Variable_Name.Compare("Snow Flag") != 0)
		{//replace with snoe flag
			climSet.Edit();
		}
		else
			hasSnowFlag = true;
	}
	else
		climSet.AddNew();
	if (!hasSnowFlag)
	{
		climSet.Edit();
		climSet.m_OptionType = 0;
		climSet.m_Variable_Name = "Snow Flag";
		climSet.m_ShortName = "SnowFlag";
		climSet.m_BinSize = 1;
		climSet.m_Daily_List = 0;
		climSet.m_Stats_Table = FALSE;
		climSet.m_Stats_Graph = FALSE;
		climSet.m_Daily_Freqs = climSet.m_Period_Mins = climSet.m_Period_Maxs = climSet.m_Data_Count = FALSE;
		climSet.m_CriticalPercentile = 50;
		climSet.m_cp2 = 50;
		climSet.Update();
	}


	climSet.Close();
	binSet.Close();
}

void CFireplusDoc::CheckBatchOptionsTable()
{
	// adds new fields to ffpGraphOptions table if necessary
	bool hasPromptDesc = false;
	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "ffpBatchOptions";
		columns.Open();
		while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("PromptDesc") == 0)
				hasPromptDesc = true;

			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if(!hasPromptDesc)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpBatchOptions] ADD [PromptDesc] INTEGER";
			m_pDB->ExecuteSQL(strSql);
			CBatchOptionsSet boSet(m_pDB);
			if (! boSet.IsOpen())
				boSet.Open();
			boSet.MoveFirst();

			boSet.Edit();
			boSet.m_PromptDesc = 0;
			boSet.Update();
			boSet.Close();
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
}
void CFireplusDoc::CheckFireOptionsTable()
{
	bool hasConditional = false;
	bool hasCauseFlag = false;
	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "ffpFireOptions";
		columns.Open();
		while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("Conditional") == 0)
				hasConditional = true;
			if(columns.m_strColumnName.CompareNoCase("CauseFlag") == 0)
				hasCauseFlag = true;
			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if(!hasConditional)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpFireOptions] ADD [Conditional] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		
		ffp4upgrade = true;
	}

	// new 2014: CauseFlag
	if(!hasCauseFlag)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpFireOptions] ADD [CauseFlag] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}

		//
		// add ffpRunFireOptions.CauseFlag
		// this should occur for v4.X -> v5 upgrades only
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRunFireOptions] ADD [CauseFlag] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		
		try
		{
			CString strSql;
			strSql = "UPDATE [ffpRunFireOptions] SET [CauseFlag] = 1022"; // all human causes
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			
			e->Delete();
			
		}

		try
		{
			CString strSql;
			strSql = "UPDATE [ffpFireOptions] SET [CauseFlag] = 1022"; // all human causes
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			
			e->Delete();
			
		}
		ffp5upgrade = true;
	}
}

// 05/2006 changes to fire table format to allow for WFMI import etc.
void CFireplusDoc::CheckFireTable()
{
  //adds new fields to PocketCard table if necessary
	bool hasSlope = false,
		 hasElevation = false,
		 hasAspect = false,
		 hasFuelModel = false,
		 hasFireType = false,
		 newFireNumber = true;

	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "Fire";
		columns.Open();
		while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("Slope") == 0)
				hasSlope = true;
			if(columns.m_strColumnName.CompareNoCase("Elevation") == 0)
				hasElevation = true;
			if(columns.m_strColumnName.CompareNoCase("Aspect") == 0)
				hasAspect = true;
			if(columns.m_strColumnName.CompareNoCase("FuelModel") == 0)
				hasFuelModel = true;
			if(columns.m_strColumnName.CompareNoCase("FireType") == 0)
				hasFireType = true;

            // modify existing fields if necessary
			if (columns.m_strColumnName.CompareNoCase("FireNumber") == 0)
			   if (columns.m_lLength < 12)
				   newFireNumber = false;

			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if(!hasSlope)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [Fire] ADD [Slope] CHAR(1)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasElevation)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [Fire] ADD [Elevation] CHAR(1)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasAspect)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [Fire] ADD [Aspect] CHAR(1)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
    if(!hasFuelModel)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [Fire] ADD [FuelModel] CHAR(1)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasFireType)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [Fire] ADD [FireType] CHAR(1)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!newFireNumber)
	{
		// first try to remove any old index
		try
		{
		
			CString strSql = "DROP INDEX FireNumber on Fire";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			
			e->Delete();
		}

		try
		{
		
			CString strSql = "DROP INDEX iFireNumber on Fire";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			
			e->Delete();
		}

		try
		{
			// 2014 update:  on certain databases (with 100,000+ records in table [Fire]
			// the ALTER COLUMN fails, due to issues with ODBC/Jet
			//
			// workaround: copy the field values into an array before dropping the 
			//   column and re-creating it.
			//
			// CString strSql;
			// strSql = "ALTER TABLE [Fire] ALTER COLUMN [FireNumber] CHAR(12)";
			// m_pDB->ExecuteSQL(strSql);
			//
			CFireSumSet fireSet(m_pDB);
			fireSet.m_strSort.Format("[FireID]");
			fireSet.m_strFilter.Format("");

			
			// copy into temp array
			if (fireSet.IsOpen())
				fireSet.Requery();
			else
				fireSet.Open();
			// int recs = fireSet.GetRecordCount();   // doesn't work on huge DB?
			int recs = 0;
			// get  record count
			fireSet.MoveFirst();
			while (! fireSet.IsEOF())
			{		
				fireSet.MoveNext();
				recs++;
			}

			// get data from existing FireNumber field.
			CString *temp = new CString[recs];
			fireSet.MoveFirst();
			int i=0;
			while (! fireSet.IsEOF())
			{
				temp[i] = fireSet.m_FireNumber.Trim();
				fireSet.MoveNext();
				i++;
			}

			fireSet.Close();
			// now drop the column
			CString strSql = "ALTER TABLE [Fire] DROP FireNumber";
			m_pDB->ExecuteSQL(strSql);

			// now create new column
			strSql = "ALTER TABLE [Fire] ADD [FireNumber] CHAR(12)";
			m_pDB->ExecuteSQL(strSql);

			// now copy data from temp array
			fireSet.m_strSort.Format("[FireID]");
			fireSet.Open();
			fireSet.MoveFirst();
			i=0;
			while (! fireSet.IsEOF())
			{
				fireSet.Edit();
				fireSet.m_FireNumber = temp[i].Trim();
				fireSet.Update();
				i++;
				fireSet.MoveNext();

			}
			fireSet.Close();



		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
}

void CFireplusDoc::ChecNFDRS2016Table()
{

}

// 09/2012
// loop through WxObs.
// if ObsType = 'O', set DailyObs=1
// if ObsType = 'R', set DailyObs=1
//void CFireplusDoc::setDailyObs(CCancelDialog *cd)
void CFireplusDoc::setDailyObs()
{
    //CString strSQL;
	//strSQL.Format("SELECT * FROM WxStation WHERE StationID IN (SELECT DISTINCT WxObs.StationID FROM WXObs ORDER BY WxObs.StationID)");

		CSIGStationSet staset(m_pDB);
		staset.m_strSort.Format("[StationID]");

        staset.Open(CRecordset::dynaset);//, strSQL);

		int iWx = 0;

		 // find number of records for progress bar
		 int iNumRecs = 0;
		 CWxSet wxset1(m_pDB);
		 wxset1.Open();
		 while (! wxset1.IsEOF())
		 {
            iNumRecs++;
		    wxset1.MoveNext();
		 }
		 wxset1.Close();

		int iStep = max(1,iNumRecs / 50);
		/*if (cd)
			
			{
			
				cd->Reset();
			 CString tempstr;

				tempstr.Format("FFP v5: Updating DailyObs field for Wx records.");

			 cd->SetStatusString(tempstr);
			 }*/

		while (! staset.IsEOF())// && (!cd || !cd->Abort()))
		{
		   CWxSet wxset(m_pDB);
           /* select only records which meet the criteria */
		   // removed this filter, because we want this function to reset all records

		   // wxset.m_strFilter.Format("([ObsType] = 'O' OR ([ObsType] = 'R' AND [Hour] = %d AND [SOW] IS NOT NULL))", staSet.m_RegSchdObs);
		   wxset.m_strFilter.Format("[StationID]='%s'",staset.m_StationID);
		   wxset.Open();
		   wxset.setDailyObs(staset.m_RegSchdObs, iStep);

		   wxset.Close();
           staset.MoveNext();
		}
		staset.Close();
}

// 05/2006 changes to WxObs table to allow for deadfuelmodel calcs
void CFireplusDoc::CheckWxObsTable()
{
  //adds new fields to PocketCard table if necessary
	bool hasSolarRadiation = false,
		 hasWetFlag = false,
		 hasGustDir = false,
		 hasGustSpd = false,
		 hasHourlyPrecip = false,
		 hasCalculatedSOW = false,
		 hasSR_WetFlag = false,
		 hasSnowFlag = false,
		 hasHour = false,      // 09/2012 - lack of 'Hour' field now triggers updateWxObs()
		// new fields NFDRS2016
		 hasDailyObs = false,
		hasFM1 = false,
		hasFM10 = false,
		hasFM100 = false,
		hasFM1000 = false,
		hasFuelTemperature = false,
		needRepairFM1000 = false;   

	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "WxObs";
		columns.Open();
		while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("SolarRadiation") == 0)
				hasSolarRadiation = true;
			if(columns.m_strColumnName.CompareNoCase("WetFlag") == 0)
				hasWetFlag = true;
			if(columns.m_strColumnName.CompareNoCase("GustDir") == 0)
				hasGustDir = true;
			if(columns.m_strColumnName.CompareNoCase("GustSpd") == 0)
				hasGustSpd = true;
			if(columns.m_strColumnName.CompareNoCase("HourlyPrecip") == 0)
				hasHourlyPrecip = true;
			if(columns.m_strColumnName.CompareNoCase("SR_SOW") == 0)
				hasCalculatedSOW = true;
			if(columns.m_strColumnName.CompareNoCase("SR_WetFlag") == 0)
				hasSR_WetFlag = true;
			if(columns.m_strColumnName.CompareNoCase("SnowFlag") == 0)
				hasSnowFlag = true;
			if(columns.m_strColumnName.CompareNoCase("Hour") == 0)
				hasHour = true;
			if (columns.m_strColumnName.CompareNoCase("DailyObs") == 0)
				hasDailyObs = true;
			if (columns.m_strColumnName.CompareNoCase("FM_1") == 0)
				hasFM1 = true;
			if (columns.m_strColumnName.CompareNoCase("FM_10") == 0)
				hasFM10 = true;
			if (columns.m_strColumnName.CompareNoCase("FM_100") == 0)
				hasFM100 = true;
			if (columns.m_strColumnName.CompareNoCase("FM_1000") == 0)
			{
				if (columns.m_strTypeName.CompareNoCase("REAL") != 0)
				{
					//need to repair the damn thing
					needRepairFM1000 = true;
				}
				hasFM1000 = true;
			}
			if (columns.m_strColumnName.CompareNoCase("FuelTemperature") == 0)
				hasFuelTemperature = true;

			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if(!hasSolarRadiation)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [SolarRadiation] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasWetFlag)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [WetFlag] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasGustDir)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [GustDir] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasGustSpd)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [GustSpd] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasHourlyPrecip)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [HourlyPrecip] REAL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasCalculatedSOW)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [SR_SOW] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//ffp4upgrade = true;
	}
	if(!hasSR_WetFlag)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [SR_WetFlag] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//ffp4upgrade = true;
	}
	if(!hasSnowFlag)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [SnowFlag] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//ffp4upgrade = true;
	}
	if(!hasHour)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [Hour] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		// 09/2012
		// trigger update of Wx Records - after database structure has been changed!
		updateWxObs = true;
		//ffp4upgrade = true;
	}

	// this function added 09/2012
	if(!hasDailyObs)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [DailyObs] SHORT";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}

		try
		{
			CString strSql;

			strSql = "CREATE INDEX [DailyObs] ON WxObs([DailyObs] ASC)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
/*
		CWxSet wxSet(this->m_pDB);
		wxSet.Open();
		while(!wxSet.IsEOF())
		{
			wxSet.Edit();
			wxSet.m_DailyObs = 0;
			wxSet.Update();
			wxSet.MoveNext();
		}
		wxSet.Close();
*/
		// new for 2012 // set DailyObs field to appropriate values...
		// 09/2012
		// trigger update of Wx Records - after database structure has been changed!
		updateWxObs = true;

		
	}

	if (!hasFM1)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [FM_1] REAL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}

	}
	if (!hasFM10)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [FM_10] REAL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}

	}
	if (!hasFM100)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [FM_100] REAL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}

	}
	if (!hasFM1000 || needRepairFM1000)
	{
		if (needRepairFM1000)
		{
			try
			{
				CString strSql;
				strSql = "ALTER TABLE [WxObs] DROP COLUMN [FM_1000]";
				m_pDB->ExecuteSQL(strSql);
			}
			catch (CDBException* e)
			{
				e->Delete();
			}

		}
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [FM_1000] REAL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}

	}
	if (!hasFuelTemperature)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] ADD [FuelTemperature] REAL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}

	}
	/*if (needRepairFM1000)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxObs] DROP COLUMN [FM_1000]";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}

	}*/
	try
	{
		CString strSql;

		strSql = "ALTER TABLE WxObs ADD CONSTRAINT PK_Obs PRIMARY KEY (StationID, ObsDate)";
		m_pDB->ExecuteSQL(strSql);
	}
	catch (CDBException* e)
	{
		e->Delete();
	}
	try
	{
		CString strSql;
		strSql = "CREATE INDEX iStationID ON WXObs (StationID)";
		m_pDB->ExecuteSQL(strSql);
	}
	catch (CDBException* e)
	{
		e->Delete();
	}
	try
	{
		CString strSql;
		strSql = "CREATE INDEX iObsDate ON WXObs (ObsDate)";
		m_pDB->ExecuteSQL(strSql);
	}
	catch (CDBException* e)
	{
		e->Delete();
	}
	try
	{
		CString strSql;
		strSql = "CREATE INDEX iStationIDObsDate ON WXObs (StationID, ObsDate)";
		m_pDB->ExecuteSQL(strSql);
	}
	catch (CDBException* e)
	{
		e->Delete();
	}
	try
	{
		CString strSql;
		strSql = "CREATE INDEX iStationIDDailyObsObsDate ON WXObs (StationID, DailyObs, ObsDate)";
		m_pDB->ExecuteSQL(strSql);
	}
	catch (CDBException* e)
	{
		e->Delete();
	}



	// 2015: new check to see if there are DailyObs values set
	// if not, we may need to update this database.
	//
	CWxSet wxSet(this->m_pDB);
	wxSet.m_strFilter.Format("[DailyObs]=1");
	wxSet.Open();
	if(wxSet.IsEOF())
		updateWxObs = true;

	wxSet.Close();

}

void CFireplusDoc::CheckffpRunClimateOptionsTable()
{//add FilterValue field if not present
	bool hasFilterValue = false;
	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "ffpRunClimateOptions";
		columns.Open();
		while (!columns.IsEOF())
		{
			if (columns.m_strColumnName.CompareNoCase("FilterValue") == 0)
				hasFilterValue = true;
			columns.MoveNext();
		}
		columns.Close();
	}
	catch (CDBException* e)
	{
		e->Delete();
	}
	if (!hasFilterValue)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRunClimateOptions] ADD [FilterValue] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}

	}
}

//void CFireplusDoc::CheckffpRunWxStationTable(CCancelDialog *cd)
void CFireplusDoc::CheckffpRunWxStationTable()
{
	bool hasWRCC = false, hasUseDormant = false,
		hasDormantDay = false, hasUseStick = false, hasRegSchdObs = false,
		hasNESDIS_ID = false, hasMaxSC = false, hasMXD = false;
	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "ffpRunWxStation";
		columns.Open();
		while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("WRCC_ID") == 0)
				hasWRCC = true;
			if(columns.m_strColumnName.CompareNoCase("UseDormant") == 0)
				hasUseDormant = true;
			if(columns.m_strColumnName.CompareNoCase("DormantJulian") == 0)
				hasDormantDay = true;
			if(columns.m_strColumnName.CompareNoCase("UseStick") == 0)
				hasUseStick = true;
			if(columns.m_strColumnName.CompareNoCase("RegSchedObs") == 0)
				hasRegSchdObs = true;
			if(columns.m_strColumnName.CompareNoCase("NESDIS_ID") == 0)
				hasNESDIS_ID = true;
			if (columns.m_strColumnName.CompareNoCase("SCM_V") == 0)
				hasMaxSC = true;
			if (columns.m_strColumnName.CompareNoCase("MX_Humid_V") == 0)
				hasMXD = true;

			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if (!hasWRCC)
	{
		//CCancelDialog cd;
		//cd.Create(IDD_CANCELDIALOG, this);
	
        //CWaitCursor wait;
		//if (cd)		
		//	   cd->SetStatusString("Adding WRCC_ID and related fields...");
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [WRCC_ID] CHAR(4)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasUseStick)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [UseStick] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasUseDormant)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [UseDormant] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasDormantDay)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [DormantJulian] DATETIME";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasRegSchdObs)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [RegSchedObs] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if(!hasNESDIS_ID)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [NESDIS_ID] CHAR(8)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasMaxSC)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [SCM_V] INTEGER";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [SCM_W] INTEGER";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [SCM_X] INTEGER";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [SCM_Y] INTEGER";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [SCM_Z] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasMXD)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [MX_Humid_V] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [MX_Humid_W] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [MX_Humid_X] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [MX_Humid_Y] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [ffpRunWxStation] ADD [MX_Humid_Z] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
}

// 05/2006 changes to fire table format to allow for WFMI import etc.
//void CFireplusDoc::CheckWxStationTable(CCancelDialog *cd)
void CFireplusDoc::CheckWxStationTable()
{
  //adds new fields to WxStation table if necessary

	bool  newLat = true, newLon = true, hasWRCC = false, hasUseDormant = false,
		hasDormantDay = false, hasUseStick = false, hasRegSchdObs = false,
		hasNESDIS_ID = false, hasTimeZone = false, hasKBDIThreshold = false;// , hasMaxGSI = false, hasGSIGreenup = false;
	//added for NFDR2016 / FFP5
	bool hasMaxSC = false, hasMXD = false;
	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "WxStation";
		columns.Open();
		while (!columns.IsEOF())
		{
            // modify existing fields if necessary
			if (columns.m_strColumnName.CompareNoCase("LatDegrees") == 0)
  	 		   if (columns.m_nDataType != SQL_DOUBLE)
				   newLat = false;

			if (columns.m_strColumnName.CompareNoCase("LonDegrees") == 0)
  	 		   if (columns.m_nDataType != SQL_DOUBLE)
				   newLon = false;

			if(columns.m_strColumnName.CompareNoCase("WRCC_ID") == 0)
				hasWRCC = true;

			if(columns.m_strColumnName.CompareNoCase("UseDormant") == 0)
				hasUseDormant = true;
			if(columns.m_strColumnName.CompareNoCase("DormantJulian") == 0)
				hasDormantDay = true;
			if(columns.m_strColumnName.CompareNoCase("UseStick") == 0)
				hasUseStick = true;
			if(columns.m_strColumnName.CompareNoCase("RegSchedObs") == 0)
				hasRegSchdObs = true;
			if (columns.m_strColumnName.CompareNoCase("NESDIS_ID") == 0)
				hasNESDIS_ID = true;
			/*if (columns.m_strColumnName.CompareNoCase("GSI_Max") == 0)
				hasMaxGSI = true;
			if (columns.m_strColumnName.CompareNoCase("GSI_Greenup") == 0)
				hasGSIGreenup = true;*/
			if (columns.m_strColumnName.CompareNoCase("SCM_V") == 0)
				hasMaxSC = true;
			if (columns.m_strColumnName.CompareNoCase("MX_Humid_V") == 0)
				hasMXD = true;
			if (columns.m_strColumnName.CompareNoCase("TimeZoneOffset") == 0)
				hasTimeZone = true;
			if (columns.m_strColumnName.CompareNoCase("KBDIThreshold") == 0)
				hasKBDIThreshold = true;
			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}

	if(!newLat)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ALTER COLUMN [LatDegrees] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}

	if(!newLon)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ALTER COLUMN [LonDegrees] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasUseStick)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [UseStick] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasUseDormant)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [UseDormant] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasDormantDay)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [DormantJulian] DATETIME";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasRegSchdObs)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [RegSchedObs] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if(!hasNESDIS_ID)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [NESDIS_ID] CHAR(8)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if (!hasWRCC)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [WRCC_ID] CHAR(4)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		 ffp4upgrade = true;
	}

	if (!hasMaxSC)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [SCM_V] INTEGER";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [WxStation] ADD [SCM_W] INTEGER";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [WxStation] ADD [SCM_X] INTEGER";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [WxStation] ADD [SCM_Y] INTEGER";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [WxStation] ADD [SCM_Z] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasMXD)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [MX_Humid_V] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [WxStation] ADD [MX_Humid_W] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [WxStation] ADD [MX_Humid_X] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [WxStation] ADD [MX_Humid_Y] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
			strSql = "ALTER TABLE [WxStation] ADD [MX_Humid_Z] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasTimeZone)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [TimeZoneOffset] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	if (!hasKBDIThreshold)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [KBDIThreshold] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
	}
	/*	if (!hasMaxGSI)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [GSI_Max] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}
	if (!hasGSIGreenup)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [WxStation] ADD [GSI_Greenup] DOUBLE";
			m_pDB->ExecuteSQL(strSql);
		}
		catch (CDBException* e)
		{
			e->Delete();
		}
		ffp4upgrade = true;
	}*/
	if(!hasWRCC || !hasRegSchdObs || !hasNESDIS_ID)
	{
		
	  
		//if (cd)
	    //		cd->SetStatusString("Initializing master database...");
        CWaitCursor wait;
       CString master_path;
        master_path.Format("%s\\ffplus5.ffp",theApp.workDir);

        if(access(master_path, 0) != 0)
	    {
		     CString msg;
		     msg.Format("Error: Master Database does not exist:\n%s", master_path);
		     AfxMessageBox(msg);
		     return;
	     }

	     CDatabase tmpDB;

	     CString strConnect;
	     strConnect.Format("Provider=MSDASQL;Driver={Microsoft Access Driver (*.mdb)};"
             "Dbq=%s;Uid=admin;Pwd=;Exclusive=1", master_path);
	     // Attempt to open the new database before replacing our ptr
	     try
	     {
		     //tmpDB->Open(lpszPathName);
		     tmpDB.OpenEx(strConnect, CDatabase::noOdbcDialog);
	     }
	     catch (CDBException* e)
	     {
			DisplayDBException(e);
			//delete tmpDB;
			e->Delete();
			return;
		 }
         CSIGStationSet masterSta(&tmpDB);
	     CSIGStationSet ourSta(m_pDB);
		 masterSta.m_strSort.Format("[StationID]");
	     masterSta.Open();

		 ourSta.m_strSort.Format("[StationID]");
	     ourSta.Open();

		 CString tempstr;

         tempstr.Format("Copying station list from master DB to working DB...");
		 //if (cd)
		 //   cd->SetStatusString(tempstr);

		 // find number of records for progress bar
		 int iNumRecs = 0;
		 while (! masterSta.IsEOF())
		 {
            iNumRecs++;
		    masterSta.MoveNext();
		 }
		 masterSta.MoveFirst();

		int iStep = max(1,iNumRecs / 25);
        int iSta = 0;

		 while (! masterSta.IsEOF()){
            ourSta.m_strFilter.Format("[StationID]='%s'", masterSta.m_StationID);
            ourSta.Requery();
			iSta ++;

			/*if (cd)
			{
				if (!(iSta % 10))
				{
					cd->Add(10);
				}
				if (!(iSta % iStep))
				{
					cd->Step();
				}
			}*/
           //tempstr.Format("Copying station '%s' from master DB to working DB...", masterSta.m_StationID);
		   //cd.SetStatusString(tempstr);

			if (ourSta.IsEOF() || ourSta.IsBOF()){ // we don't already have this station - import all data
               ourSta.AddNew();
               ourSta.m_StationID = masterSta.m_StationID;
			   if (masterSta.IsFieldNull(&masterSta.m_Name))
			      ourSta.SetFieldNull(&ourSta.m_Name);
			   else
                  ourSta.m_Name = masterSta.m_Name;

			   if (masterSta.IsFieldNull(&masterSta.m_NFDRSFM))
			      ourSta.SetFieldNull(&ourSta.m_NFDRSFM);
			   else
			      ourSta.m_NFDRSFM = masterSta.m_NFDRSFM;

			   if (masterSta.IsFieldNull(&masterSta.m_Use88))
			      ourSta.SetFieldNull(&ourSta.m_Use88);
			   else
			      ourSta.m_Use88 = masterSta.m_Use88;

			   if (masterSta.IsFieldNull(&masterSta.m_LatDegrees))
			      ourSta.SetFieldNull(&ourSta.m_LatDegrees);
			   else
			      ourSta.m_LatDegrees = masterSta.m_LatDegrees;

			   if (masterSta.IsFieldNull(&masterSta.m_LonDegrees))
			      ourSta.SetFieldNull(&ourSta.m_LonDegrees);
			   else
			      ourSta.m_LonDegrees = masterSta.m_LonDegrees;

			   if (masterSta.IsFieldNull(&masterSta.m_Elevation))
			      ourSta.SetFieldNull(&ourSta.m_Elevation);
			   else
			      ourSta.m_Elevation = masterSta.m_Elevation;

			   if (masterSta.IsFieldNull(&masterSta.m_ClimateCls))
			      ourSta.SetFieldNull(&ourSta.m_ClimateCls);
			   else
			      ourSta.m_ClimateCls = masterSta.m_ClimateCls;

			   if (masterSta.IsFieldNull(&masterSta.m_SlopeCls))
			      ourSta.SetFieldNull(&ourSta.m_SlopeCls);
			   else
                  ourSta.m_SlopeCls = masterSta.m_SlopeCls;

			   if (masterSta.IsFieldNull(&masterSta.m_HerbAnnual))
			      ourSta.SetFieldNull(&ourSta.m_HerbAnnual);
			   else
				   ourSta.m_HerbAnnual = masterSta.m_HerbAnnual;

			   if (masterSta.IsFieldNull(&masterSta.m_GreenJulian))
			      ourSta.SetFieldNull(&ourSta.m_GreenJulian);
			   else
			      ourSta.m_GreenJulian = masterSta.m_GreenJulian;

			   if (masterSta.IsFieldNull(&masterSta.m_FreezeJulian))
			      ourSta.SetFieldNull(&ourSta.m_FreezeJulian);
			   else
			      ourSta.m_FreezeJulian = masterSta.m_FreezeJulian;

			   if (masterSta.IsFieldNull(&masterSta.m_StartGreenHerb))
			      ourSta.SetFieldNull(&ourSta.m_StartGreenHerb);
			   else
			      ourSta.m_StartGreenHerb = masterSta.m_StartGreenHerb;

			   if (masterSta.IsFieldNull(&masterSta.m_StartGreenShrub))
			      ourSta.SetFieldNull(&ourSta.m_StartGreenShrub);
			   else
			      ourSta.m_StartGreenShrub = masterSta.m_StartGreenShrub;

			   if (masterSta.IsFieldNull(&masterSta.m_StartKBDI))
			      ourSta.SetFieldNull(&ourSta.m_StartKBDI);
			   else
			      ourSta.m_StartKBDI = masterSta.m_StartKBDI;

			   if (masterSta.IsFieldNull(&masterSta.m_Deciduous))
			      ourSta.SetFieldNull(&ourSta.m_Deciduous);
			   else
			      ourSta.m_Deciduous = masterSta.m_Deciduous;

			   if (masterSta.IsFieldNull(&masterSta.m_AvgPrecip))
			      ourSta.SetFieldNull(&ourSta.m_AvgPrecip);
			   else
			      ourSta.m_AvgPrecip = masterSta.m_AvgPrecip;

			   if (masterSta.IsFieldNull(&masterSta.m_Start1000))
			      ourSta.SetFieldNull(&ourSta.m_Start1000);
			   else
			      ourSta.m_Start1000 = masterSta.m_Start1000;

               if (masterSta.IsFieldNull(&masterSta.m_FM1Eq10))
			      ourSta.SetFieldNull(&ourSta.m_FM1Eq10);
			   else
			      ourSta.m_FM1Eq10 = masterSta.m_FM1Eq10;

			   if (masterSta.IsFieldNull(&masterSta.m_Aspect))
			      ourSta.SetFieldNull(&ourSta.m_Aspect);
			   else
				   ourSta.m_Aspect = masterSta.m_Aspect;

			   if (masterSta.IsFieldNull(&masterSta.m_PsnOnSlope))
			      ourSta.SetFieldNull(&ourSta.m_PsnOnSlope);
			   else
				   ourSta.m_PsnOnSlope = masterSta.m_PsnOnSlope;

			   if (masterSta.IsFieldNull(&masterSta.m_StnType))
			      ourSta.SetFieldNull(&ourSta.m_StnType);
			   else
				   ourSta.m_StnType = masterSta.m_StnType;

			   if (masterSta.IsFieldNull(&masterSta.m_Site))
			      ourSta.SetFieldNull(&ourSta.m_Site);
			   else
			      ourSta.m_Site = masterSta.m_Site;

			   if (masterSta.IsFieldNull(&masterSta.m_State))
			      ourSta.SetFieldNull(&ourSta.m_State);
			   else
				   ourSta.m_State = masterSta.m_State;

			   if (masterSta.IsFieldNull(&masterSta.m_County))
			      ourSta.SetFieldNull(&ourSta.m_County);
			   else
	              ourSta.m_County  = masterSta.m_County;

			   if (masterSta.IsFieldNull(&masterSta.m_ObsAgy))
			      ourSta.SetFieldNull(&ourSta.m_ObsAgy);
			   else
				   ourSta.m_ObsAgy = masterSta.m_ObsAgy;

			   if (masterSta.IsFieldNull(&masterSta.m_ObsUnit))
			      ourSta.SetFieldNull(&ourSta.m_ObsUnit);
			   else
			      ourSta.m_ObsUnit = masterSta.m_ObsUnit;

			   if (masterSta.IsFieldNull(&masterSta.m_FSRegion))
			      ourSta.SetFieldNull(&ourSta.m_FSRegion);
			   else
			      ourSta.m_FSRegion = masterSta.m_FSRegion;

			   if (masterSta.IsFieldNull(&masterSta.m_WRCC_ID))
			      ourSta.SetFieldNull(&ourSta.m_WRCC_ID);
			   else
			      ourSta.m_WRCC_ID = masterSta.m_WRCC_ID;

			   if (masterSta.IsFieldNull(&masterSta.m_RegSchdObs))
			      ourSta.SetFieldNull(&ourSta.m_RegSchdObs);
			   else
			      ourSta.m_RegSchdObs = masterSta.m_RegSchdObs;

			   if (masterSta.IsFieldNull(&masterSta.m_NESDIS_ID))
			      ourSta.SetFieldNull(&ourSta.m_NESDIS_ID);
			   else
			      ourSta.m_NESDIS_ID = masterSta.m_NESDIS_ID;
			   ourSta.Update();
			}
			else
			{ // already have this station - just import WRCC field
               ourSta.Edit();
			   if(!hasWRCC)
			   {
				   if (masterSta.IsFieldNull(&masterSta.m_WRCC_ID))
					  ourSta.SetFieldNull(&ourSta.m_WRCC_ID);
				   else
					   ourSta.m_WRCC_ID.Format("%s",masterSta.m_WRCC_ID);
			   }
			   if(!hasRegSchdObs)
			   {
				   if (masterSta.IsFieldNull(&masterSta.m_RegSchdObs))
					  ourSta.SetFieldNull(&ourSta.m_RegSchdObs);
				   else
					   ourSta.m_RegSchdObs = masterSta.m_RegSchdObs;
			   }
			   if(!hasNESDIS_ID)
			   {
				   if (masterSta.IsFieldNull(&masterSta.m_NESDIS_ID))
					  ourSta.SetFieldNull(&ourSta.m_NESDIS_ID);
				   else
					   ourSta.m_NESDIS_ID.Format("%s",masterSta.m_NESDIS_ID);
			   }
			   //goddamn acces integrity check on date fields
			   if(!ourSta.IsFieldNull(&ourSta.m_GreenJulian) && ourSta.m_GreenJulian.GetStatus() == COleDateTime::invalid)
				   ourSta.SetFieldNull(&ourSta.m_GreenJulian);
			   if(!ourSta.IsFieldNull(&ourSta.m_FreezeJulian) && ourSta.m_FreezeJulian.GetStatus() == COleDateTime::invalid)
				   ourSta.SetFieldNull(&ourSta.m_FreezeJulian);
		       ourSta.Update();
 			}
			/*if (cd)
            if (cd->Abort()){
			      AfxMessageBox("To ensure the integrity of this database, the FFP4 upgrade routine needs to run completely.");

				  cd->m_bCancelled = FALSE;
		    }*/
	        masterSta.MoveNext();
	     }
         masterSta.Close();
	     ourSta.Close();

	     tmpDB.Close();
		//delete tmpDB;

		 //ffp4upgrade = true;
	}
	//cludge for missing greenup dates and missing lookfreeze dates!
	//ADDED SCM_? AND MX_Humid_? inits to this loop, 3/31/2017
	CSIGStationSet staSet(m_pDB);
	staSet.Open();
	while(!staSet.IsEOF())
	{
		if(staSet.m_GreenJulian.GetStatus() != COleDateTime::valid)
		{
			staSet.Edit();
			staSet.m_GreenJulian.SetDateTime(2008, 1, 1, 13, 0, 0);
			staSet.Update();
		}
		if(staSet.m_FreezeJulian.GetStatus() != COleDateTime::valid)
		{
			staSet.Edit();
			staSet.m_FreezeJulian.SetDateTime(2008, 12, 31, 13, 0, 0);
			staSet.Update();
		}
		// 6/13/2012 - Add RegSchdObs at default value of 13 if it's NULL!!!!!
		if(staSet.IsFieldNull(&staSet.m_RegSchdObs))
		{
			staSet.Edit();
			staSet.m_RegSchdObs = 13;
			staSet.Update();
		}
		if (!hasMaxSC)
		{
			staSet.Edit();
			theApp.m_NFDRS2016.iSetFuelModel('V');
			staSet.m_SCM_V = theApp.m_NFDRS2016.GetSCMax();
			theApp.m_NFDRS2016.iSetFuelModel('W');
			staSet.m_SCM_W = theApp.m_NFDRS2016.GetSCMax();
			theApp.m_NFDRS2016.iSetFuelModel('X');
			staSet.m_SCM_X = theApp.m_NFDRS2016.GetSCMax();
			theApp.m_NFDRS2016.iSetFuelModel('Y');
			staSet.m_SCM_Y = theApp.m_NFDRS2016.GetSCMax();
			theApp.m_NFDRS2016.iSetFuelModel('Z');
			staSet.m_SCM_Z = theApp.m_NFDRS2016.GetSCMax();
			staSet.Update();
		}
		else
		{
			if (staSet.IsFieldNull(&staSet.m_SCM_V) || staSet.m_SCM_V > 300 || staSet.m_SCM_V < 1)
			{
				staSet.Edit();
				theApp.m_NFDRS2016.iSetFuelModel('V');
				staSet.m_SCM_V = theApp.m_NFDRS2016.GetSCMax();
				staSet.Update();
			}
			if (staSet.IsFieldNull(&staSet.m_SCM_W) || staSet.m_SCM_W > 300 || staSet.m_SCM_W < 1)
			{
				staSet.Edit();
				theApp.m_NFDRS2016.iSetFuelModel('W');
				staSet.m_SCM_W = theApp.m_NFDRS2016.GetSCMax();
				staSet.Update();
			}
			if (staSet.IsFieldNull(&staSet.m_SCM_X) || staSet.m_SCM_X > 300 || staSet.m_SCM_X < 1)
			{
				staSet.Edit();
				theApp.m_NFDRS2016.iSetFuelModel('X');
				staSet.m_SCM_X = theApp.m_NFDRS2016.GetSCMax();
				staSet.Update();
			}
			if (staSet.IsFieldNull(&staSet.m_SCM_Y) || staSet.m_SCM_Y > 300 || staSet.m_SCM_Y < 1)
			{
				staSet.Edit();
				theApp.m_NFDRS2016.iSetFuelModel('Y');
				staSet.m_SCM_Y = theApp.m_NFDRS2016.GetSCMax();
				staSet.Update();
			}
			if (staSet.IsFieldNull(&staSet.m_SCM_Z) || staSet.m_SCM_Z > 300 || staSet.m_SCM_Z < 1)
			{
				staSet.Edit();
				theApp.m_NFDRS2016.iSetFuelModel('Z');
				staSet.m_SCM_Z = theApp.m_NFDRS2016.GetSCMax();
				staSet.Update();
			}
		}
		if (!hasMXD)
		{
			staSet.Edit();
			theApp.m_NFDRS2016.iSetFuelModel('V');
			staSet.m_MX_Humid_V = theApp.m_NFDRS2016.GetMxdHumid();
			theApp.m_NFDRS2016.iSetFuelModel('W');
			staSet.m_MX_Humid_W = theApp.m_NFDRS2016.GetMxdHumid();
			theApp.m_NFDRS2016.iSetFuelModel('X');
			staSet.m_MX_Humid_X = theApp.m_NFDRS2016.GetMxdHumid();
			theApp.m_NFDRS2016.iSetFuelModel('Y');
			staSet.m_MX_Humid_Y = theApp.m_NFDRS2016.GetMxdHumid();
			theApp.m_NFDRS2016.iSetFuelModel('Z');
			staSet.m_MX_Humid_Z = theApp.m_NFDRS2016.GetMxdHumid();
			staSet.Update();
		}
		else
		{
			if (staSet.IsFieldNull(&staSet.m_MX_Humid_V))
			{
				staSet.Edit();
				theApp.m_NFDRS2016.iSetFuelModel('V');
				staSet.m_MX_Humid_V = theApp.m_NFDRS2016.GetMxdHumid();
				staSet.Update();
			}
			if (staSet.IsFieldNull(&staSet.m_MX_Humid_W))
			{
				staSet.Edit();
				theApp.m_NFDRS2016.iSetFuelModel('W');
				staSet.m_MX_Humid_W = theApp.m_NFDRS2016.GetMxdHumid();
				staSet.Update();
			}
			if (staSet.IsFieldNull(&staSet.m_MX_Humid_X))
			{
				staSet.Edit();
				theApp.m_NFDRS2016.iSetFuelModel('X');
				staSet.m_MX_Humid_X = theApp.m_NFDRS2016.GetMxdHumid();
				staSet.Update();
			}
			if (staSet.IsFieldNull(&staSet.m_MX_Humid_Y))
			{
				staSet.Edit();
				theApp.m_NFDRS2016.iSetFuelModel('Y');
				staSet.m_MX_Humid_Y = theApp.m_NFDRS2016.GetMxdHumid();
				staSet.Update();
			}
			if (staSet.IsFieldNull(&staSet.m_MX_Humid_Z))
			{
				staSet.Edit();
				theApp.m_NFDRS2016.iSetFuelModel('Z');
				staSet.m_MX_Humid_Z = theApp.m_NFDRS2016.GetMxdHumid();
				staSet.Update();
			}
		}
		if (!hasTimeZone)
		{
			staSet.Edit();
			staSet.m_timeZoneOffset = theApp.m_timeZones.GetTimeZone(staSet.m_State.GetBuffer()).m_offset;
			staSet.Update();
		}
		if(!hasKBDIThreshold)
		{
			staSet.Edit();
			staSet.m_KBDIThreshold = 100;
			staSet.Update();
		}
		staSet.MoveNext();
	}
	staSet.Close();
}

int CFireplusDoc::CreateffpRunPocketCardTable()
{
	try
	{
		CString sqlStr = "CREATE TABLE [ffpRunPocketCard] ("
									"[runID] INTEGER,"
									"[varID] INTEGER,"
									"[FireDangerArea] CHAR(30),"
									"[AreaBitmap] CHAR(255),"
									"[Index] CHAR(50),"
									"[FuelModelName] CHAR(30),"
									"[Year1] INTEGER,"
									"[Year2] INTEGER,"
									"[Fire1] CHAR(25),"
									"[Fire2] CHAR(25),"
									"[Fire3] CHAR(25),"
									"[Fire1Value] INTEGER,"
									"[Fire2Value] INTEGER,"
									"[Fire3Value] INTEGER,"
									"[Fire1Date] DATETIME,"
									"[Fire2Date] DATETIME,"
									"[Fire3Date] DATETIME,"
									"[Line1] CHAR(25),"
									"[Line2] CHAR(25),"
									"[Line3] CHAR(25),"
									"[WindSpeed] INTEGER,"
									"[RH] INTEGER,"
									"[Temp] INTEGER,"
									"[LiveFM] INTEGER,"
									"[PastExperience] MEMO,"
									"[SIG/Station] CHAR(20),"
									"[pcRyVal] INTEGER,"
									"[pcGyVal] INTEGER,"
									"[pcYVal] INTEGER,"
									"[ExtraVar] CHAR(30),"
									"[CP] INTEGER,"
									"[RespAgency] CHAR(35),"
									"[NWCGStd] YESNO NOT NULL)";
		m_pDB->ExecuteSQL(sqlStr);
	}
	catch( CDBException* e )
	{
		DisplayDBException(e);
		e->Delete();
		return 0;
	}
	return 1;
}

void CFireplusDoc::CheckPocketCardTable()
{
//adds new fields to PocketCard table if necessary
	bool hasSIGSta = false, hasRY = false, hasGY = false, hasY = false,
		hasExtra = false, hasExtraVal = false, hasCP = false, hasNWCGStd = false, hasRespAgency = false;
	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "ffpPocketCard";
		columns.Open();
		while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("SIG/Station") == 0)
				hasSIGSta = true;
			if(columns.m_strColumnName.CompareNoCase("pcRyVal") == 0)
				hasRY = true;
			if(columns.m_strColumnName.CompareNoCase("pcGyVal") == 0)
				hasGY = true;
			if(columns.m_strColumnName.CompareNoCase("pcYVal") == 0)
				hasY = true;
			if(columns.m_strColumnName.CompareNoCase("ExtraVar") == 0)
				hasExtra = true;
			if(columns.m_strColumnName.CompareNoCase("CP") == 0)
				hasCP = true;
			if(columns.m_strColumnName.CompareNoCase("NWCGStd") == 0)
				hasNWCGStd = true;
			if(columns.m_strColumnName.CompareNoCase("RespAgency") == 0)
				hasRespAgency = true;
			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if(!hasSIGSta)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpPocketCard] ADD [SIG/Station] CHAR(20)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//td.CreateField("SIG/Station", dbText, 20, dbFixedField | dbUpdatableField);
	}
	if(!hasRY)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpPocketCard] ADD [pcRyVal] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//td.CreateField("SIG/Station", dbText, 20, dbFixedField | dbUpdatableField);
	}
	if(!hasGY)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpPocketCard] ADD [pcGyVal] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//td.CreateField("SIG/Station", dbText, 20, dbFixedField | dbUpdatableField);
	}
	if(!hasY)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpPocketCard] ADD [pcYVal] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//td.CreateField("SIG/Station", dbText, 20, dbFixedField | dbUpdatableField);
	}
	if(!hasExtra)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpPocketCard] ADD [ExtraVar] CHAR(30)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//td.CreateField("SIG/Station", dbText, 20, dbFixedField | dbUpdatableField);
	}
	if(!hasCP)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpPocketCard] ADD [CP] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//td.CreateField("SIG/Station", dbText, 20, dbFixedField | dbUpdatableField);
	}
	if(!hasNWCGStd)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpPocketCard] ADD [NWCGStd] YESNO NOT NULL";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//td.CreateField("SIG/Station", dbText, 20, dbFixedField | dbUpdatableField);
	}
	if(!hasRespAgency)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpPocketCard] ADD [RespAgency] CHAR(35)";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//td.CreateField("SIG/Station", dbText, 20, dbFixedField | dbUpdatableField);
	}
	/*try
	{
		//CString strSql = "ALTER TABLE [ffpPocketCard] DROP PRIMARY KEY StationID";
		CString strSql = "DROP INDEX StationID on ffpPocketCard";
		m_pDB->ExecuteSQL(strSql);
	}
	catch(CDBException* e)
	{
		DisplayDBException(e);
		e->Delete();
	}*/
	//get rid of StationID field, must remove primary key first
	try
	{
		CString strSql = "ALTER TABLE [ffpPocketCard] DROP CONSTRAINT PrimaryKey";
		m_pDB->ExecuteSQL(strSql);
	}
	catch(CDBException* e)
	{
		//DisplayDBException(e);
		e->Delete();
	}
	try
	{
		CString strSql = "ALTER TABLE [ffpPocketCard] DROP StationID";
		m_pDB->ExecuteSQL(strSql);
	}
	catch(CDBException* e)
	{
		//DisplayDBException(e);
		e->Delete();
	}
	/*try
	{
		CString strSql = "ALTER TABLE [ffpPocketCard] ADD FOREIGN KEY [SIG/Station]";
		m_pDB->ExecuteSQL(strSql);
	}
	catch(CDBException* e)
	{
		e->Delete();
	}*/
/*	CDaoTableDef td(m_pDB);
	td.Open("ffpPocketCard");
	bool hasSIGSta = false, hasRY = false, hasGY = false, hasY = false,
		hasExtra = false, hasExtraVal = false, hasCP = false, hasNWCGStd = false, hasRespAgency = false;
	for(int i = 0; i < td.GetIndexCount(); i++)
	{
		CDaoIndexInfo iInfo;
		td.GetIndexInfo(i, iInfo);
		if(iInfo.m_strName.CompareNoCase("StationID") == 0)
			td.DeleteIndex(i);
	}
	for(int f = 0; f < td.GetFieldCount(); f++)
	{
		CDaoFieldInfo fInfo;
		td.GetFieldInfo(f, fInfo);
		if(fInfo.m_strName.CompareNoCase("SIG/Station") == 0)
			hasSIGSta = true;
		if(fInfo.m_strName.CompareNoCase("pcRyVal") == 0)
			hasRY = true;
		if(fInfo.m_strName.CompareNoCase("pcGyVal") == 0)
			hasGY = true;
		if(fInfo.m_strName.CompareNoCase("pcYVal") == 0)
			hasY = true;
		if(fInfo.m_strName.CompareNoCase("ExtraVar") == 0)
			hasExtra = true;
		if(fInfo.m_strName.CompareNoCase("CP") == 0)
			hasCP = true;
		if(fInfo.m_strName.CompareNoCase("NWCGStd") == 0)
			hasNWCGStd = true;
		if(fInfo.m_strName.CompareNoCase("RespAgency") == 0)
			hasRespAgency = true;
	}
	if(!hasSIGSta)
		td.CreateField("SIG/Station", dbText, 20, dbFixedField | dbUpdatableField);
	if(!hasRY)
		td.CreateField("pcRyVal", dbInteger, 2 , dbFixedField | dbUpdatableField);
	if(!hasGY)
		td.CreateField("pcGyVal", dbInteger, 2 , dbFixedField | dbUpdatableField);
	if(!hasY)
		td.CreateField("pcYVal", dbInteger, 2 , dbFixedField | dbUpdatableField);
	if(!hasExtra)
		td.CreateField("ExtraVar", dbText, 30 , dbFixedField | dbUpdatableField);
	if(!hasCP)
		td.CreateField("CP", dbInteger, 2 , dbFixedField | dbUpdatableField);
	if(!hasNWCGStd)
		td.CreateField("NWCGStd", dbBoolean, 1 , dbFixedField | dbUpdatableField);
	if(!hasRespAgency)
		td.CreateField("RespAgency", dbText, 35 , dbFixedField | dbUpdatableField);
	bool hasSIGStaNDX = false;
	for(i = 0; i < td.GetIndexCount(); i++)
	{
		CDaoIndexInfo iInfo;
		td.GetIndexInfo(i, iInfo);
		if(iInfo.m_strName.CompareNoCase("SIG/Station") == 0)
			hasSIGStaNDX = true;
	}
	if(!hasSIGStaNDX)
	{
		for(i = td.GetIndexCount() - 1; i >= 0; i--)
		{
			td.DeleteIndex(i);
		}
		CDaoIndexInfo iInfo;
		CDaoIndexFieldInfo ifInfo;
		ifInfo.m_strName = "SIG/Station";
		ifInfo.m_bDescending = FALSE;
		iInfo.m_strName = "SIG/Station";
		iInfo.m_nFields = 1;
		iInfo.m_pFieldInfos = &ifInfo;
		iInfo.m_bPrimary = FALSE;
		iInfo.m_bUnique = TRUE;
		iInfo.m_bClustered = FALSE;
		iInfo.m_bForeign = FALSE;
		iInfo.m_bIgnoreNulls = FALSE;
		iInfo.m_bRequired = FALSE;
		td.CreateIndex(iInfo);
	}
	td.Close();*/
}

// added for batch2
void CFireplusDoc::CheckRunsTable()
{
	// adds new field(s) to ffpRuns table if necessary
	bool hasBatchInc = false;
	try
	{
		CColumns columns(m_pDB);
		columns.m_strTableNameParam = "ffpRuns";
		columns.Open();
		while (!columns.IsEOF())
		{
			if(columns.m_strColumnName.CompareNoCase("BatchInc") == 0)
				hasBatchInc = true;
			columns.MoveNext();
		}
		columns.Close();
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	if(!hasBatchInc)
	//if(!hasSIGSta)
	{
		try
		{
			CString strSql;
			strSql = "ALTER TABLE [ffpRuns] ADD [BatchInc] INTEGER";
			m_pDB->ExecuteSQL(strSql);
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		//td.CreateField("BatchInc", dbInteger, 1, dbFixedField | dbUpdatableField);
	}

/*	CDaoTableDef td(m_pDB);
	td.Open("ffpRuns");
	bool hasBatchInc = false;

	for(int f = 0; f < td.GetFieldCount(); f++)
	{
		CDaoFieldInfo fInfo;
		td.GetFieldInfo(f, fInfo);
		if(fInfo.m_strName.CompareNoCase("BatchInc") == 0)
			hasBatchInc = true;
	}
	if(!hasBatchInc)
		td.CreateField("BatchInc", dbInteger, 1, dbFixedField | dbUpdatableField);

	td.Close();

	*/
}

/////////////////////////////////////////////////////////////////////////////
// CUpgradeDialog dialog

CUpgradeDialog::CUpgradeDialog(CFireplusDoc* pParent /*=NULL*/)
	: CDialog(CUpgradeDialog::IDD)
{
	//{{AFX_DATA_INIT(CBatchEditDialog)
	//}}AFX_DATA_INIT
}

void CUpgradeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUpgradeDialog, CDialog)

END_MESSAGE_MAP()

BOOL CUpgradeDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUpgradeDialog::OnOK()
{
	CDialog::OnOK();
}

// end added for batch2

int CFireplusDoc::RunDeleteRun(int _runID)
{
	CRunsSet runsSet(m_pDB);
	CString runFilter;
	runFilter.Format("[runID] = %d", _runID);
	runsSet.m_strFilter = runFilter;
	runsSet.Open();
	while(!runsSet.IsEOF())
	{
		runsSet.Delete();
		runsSet.MoveNext();
	}
	runsSet.Close();
	CRunClimateOptionsSet rcoSet(m_pDB);
	rcoSet.m_strFilter = runFilter;
	rcoSet.Open();
	while(!rcoSet.IsEOF())
	{
		rcoSet.Delete();
		rcoSet.MoveNext();
	}
	rcoSet.Close();
	CRunDescriptionSet rdSet(m_pDB);
	rdSet.m_strFilter = runFilter;
	rdSet.Open();
	while(!rdSet.IsEOF())
	{
		rdSet.Delete();
		rdSet.MoveNext();
	}
	rdSet.Close();
	CRunFireAssocSet rfaSet(m_pDB);
	rfaSet.m_strFilter = runFilter;
	rfaSet.Open();
	while(!rfaSet.IsEOF())
	{
		rfaSet.Delete();
		rfaSet.MoveNext();
	}
	rfaSet.Close();
	CRunFireOptionsSet rfoSet(m_pDB);
	rfoSet.m_strFilter = runFilter;
	rfoSet.Open();
	while(!rfoSet.IsEOF())
	{
		rfoSet.Delete();
		rfoSet.MoveNext();
	}
	rfoSet.Close();
	CRunOptionsSet roSet(m_pDB);
	roSet.m_strFilter = runFilter;
	roSet.Open();
	while(!roSet.IsEOF())
	{
		roSet.Delete();
		roSet.MoveNext();
	}
	roSet.Close();
	CRunPocketCardSet rpcSet(m_pDB);
	rpcSet.m_strFilter = runFilter;
	rpcSet.Open();
	while(!rpcSet.IsEOF())
	{
		rpcSet.Delete();
		rpcSet.MoveNext();
	}
	rpcSet.Close();
	CRunWxStationSet rwsSet(m_pDB);
	rwsSet.m_strFilter = runFilter;
	rwsSet.Open();
	while(!rwsSet.IsEOF())
	{
		rwsSet.Delete();
		rwsSet.MoveNext();
	}
	rwsSet.Close();
	return 0;
}

int CFireplusDoc::RunDeleteBatch(int _batchID)
{
	CBatchRunsSet brSet(m_pDB);
	brSet.m_strFilter.Format("[batchID] = %d", _batchID);
	CBatchSet bSet(m_pDB);
	bSet.m_strFilter.Format("[batchID] = %d", _batchID);
	brSet.Open();
	bSet.Open();
	while(!brSet.IsEOF())
	{
		brSet.Delete();
		brSet.MoveNext();
	}
	while(!bSet.IsEOF())
	{
		bSet.Delete();
		bSet.MoveNext();
	}
	brSet.Close();
	bSet.Close();
	return 0;
}

// Check for and perform any necessary run maintenance
int CFireplusDoc::DoRunMaintenance(void)
{
	CBatchOptionsSet bsSet(m_pDB);
	bsSet.Open();

	if(bsSet.IsEOF())
	{
		/* add first record */
		bsSet.AddNew();
		bsSet.m_maintScheme = tNone;
		bsSet.m_maintValue = 1000;
		bsSet.Update();
		bsSet.MoveFirst();
	}
	/*if(bsSet.m_maintScheme == tDisabled)
	{
		doRunSaving = FALSE;
		bsSet.Close();
		return 0;
	}*/
	if(bsSet.m_maintScheme == tNone)
	{//do nothing
		bsSet.Close();
		return 0;
	}

	CList<int, int> runKillList;
	CBatchSet batchSet(m_pDB);
	batchSet.m_strSort = "[LastRun]";
	CRunsSet runsSet(m_pDB);
	batchSet.Open();
	runsSet.Open();
	int maxRecs = bsSet.m_maintValue;
	for(int r = 0; r < maxRecs && !runsSet.IsEOF(); r++)
		runsSet.MoveNext();
	switch(bsSet.m_maintScheme)
	{
	case tLRU:
		{
			COleDateTime today = COleDateTime::GetCurrentTime();
			COleDateTimeSpan span(14, 0, 0, 1);
			COleDateTime rDate;
			if(!rDate.ParseDateTime(runsSet.m_LastRun))
				rDate = today;
			while(!runsSet.IsEOF())
			{
				if(today - rDate > span)
					runKillList.AddTail(runsSet.m_runID);
				runsSet.MoveNext();
			}
		}
		break;
	case tNumRecs:
		{
			while(!runsSet.IsEOF())
			{
				runKillList.AddTail(runsSet.m_runID);
				runsSet.MoveNext();
			}
		}
		break;
	default:
		break;
	}
	bsSet.Close();
	CBatchRunsSet brSet(m_pDB);
	brSet.Open();
	POSITION pos = runKillList.GetHeadPosition();
	while(pos)
	{
		int killID = runKillList.GetNext(pos);
		RunDeleteRun(killID);
		//now kill any batch->Run orphans
		brSet.m_strFilter.Format("[runID] = %d", killID);
		brSet.Requery();
		while(!brSet.IsEOF())
		{
			brSet.Delete();
			brSet.MoveNext();
		}
		/*runsSet.m_strFilter.Format("[runID] = %d", killID);
		runsSet.Requery();
		while(!runsSet.IsEOF())
		{
			runsSet.Delete();
			runsSet.MoveNext();
		}*/
	}
	brSet.Close();
	return 0;
}

void CFireplusDoc::OnCloseDocument()
{
	//save Document specific options...
	//overlays
	//CFFPOverlaysSet olSet(m_pDB);
	if ( m_pDB && m_pDB->IsOpen())
	{
	//olSet.m_strFilter.Format(
	CFFPOptsSet *optsSet1 = new CFFPOptsSet(m_pDB);
	optsSet1->Open();
	if(!optsSet1->IsEOF())
	{
		optsSet1->Edit();
		optsSet1->m_RunSaveOn = doRunSaving;
		optsSet1->m_EventDays = 1;//theApp.eventDays;
		optsSet1->m_EventHours = 1;//theApp.eventHours;
		optsSet1->m_PctWxVarID = theApp.rerapVarID;
		optsSet1->m_CP1 = theApp.m_OverallCP1;
		optsSet1->m_CP1Color = theApp.m_OverallCP1Color;
		optsSet1->m_CP2 = theApp.m_OverallCP2;
		optsSet1->m_CP2Color = theApp.m_OverallCP2Color;
		optsSet1->m_MaxMissWx = theApp.maxMissingWx;
		optsSet1->m_LastGfxFmt = theApp.lastGraphicFormat;
		optsSet1->m_EnableFuture = theApp.m_enableFuture;

		//overlays
		optsSet1->m_OverlayYear1 = theApp.overLayYears[0];
		optsSet1->m_OverlayYear2 = theApp.overLayYears[1];
		optsSet1->m_OverlayYear3 = theApp.overLayYears[2];
		optsSet1->m_OverlayWidth1 = theApp.lineWidths[0];
		optsSet1->m_OverlayWidth2 = theApp.lineWidths[1];
		optsSet1->m_OverlayWidth3 = theApp.lineWidths[2];
		optsSet1->m_OverlayStyle1 = theApp.lineStyles[0];
		optsSet1->m_OverlayStyle2 = theApp.lineStyles[1];
		optsSet1->m_OverlayStyle3 = theApp.lineStyles[2];
		optsSet1->m_OverlayColor1 = theApp.overLayColors[0];
		optsSet1->m_OverlayColor2 = theApp.overLayColors[1];
		optsSet1->m_OverlayColor3 = theApp.overLayColors[2];

		//Merges
		optsSet1->m_MergeColor1 = theApp.mergeColors[0];
		optsSet1->m_MergeColor2 = theApp.mergeColors[1];
		optsSet1->m_MergeColor3 = theApp.mergeColors[2];
		optsSet1->m_MergeColor4 = theApp.mergeColors[3];
		optsSet1->m_MergeWidth1 = theApp.mergeWidths[0];
		optsSet1->m_MergeWidth2 = theApp.mergeWidths[1];
		optsSet1->m_MergeWidth3 = theApp.mergeWidths[2];
		optsSet1->m_MergeWidth4 = theApp.mergeWidths[3];
		optsSet1->m_MergeStyle1 = theApp.mergeStyles[0];
		optsSet1->m_MergeStyle2 = theApp.mergeStyles[1];
		optsSet1->m_MergeStyle3 = theApp.mergeStyles[2];
		optsSet1->m_MergeStyle4 = theApp.mergeStyles[3];

		optsSet1->Update();
	}
	optsSet1->Close();
	delete optsSet1;
	}

	CString find, del;
	find.Format("%s\\*.nfd", docDir);//theApp.dbDir);
	CFileFind ff;
	BOOL bWorking = ff.FindFile(find);
	while(bWorking)
	{
		bWorking = ff.FindNextFile();
		del = ff.GetFilePath();
		remove(del);
	}

	CDocument::OnCloseDocument();
}

void CFireplusDoc::OnUpdateBatchEnablerunsaving(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(doRunSaving);
}

void CFireplusDoc::OnBatchEnablerunsaving()
{
	doRunSaving = !doRunSaving;
	CFFPOptsSet optsSet(m_pDB);
	optsSet.Open();
	optsSet.Edit();
	optsSet.m_RunSaveOn = doRunSaving;
	optsSet.Update();
	optsSet.Close();
}
// UserImportDialog.cpp : implementation file
//
 
#include "stdafx.h"
#include "fireplus.h"
#include "UserImportDialog.h"
#include "UserVarSet.h"
#include "SIGStationSet.h"
#include "ImportDoneDialog.h"
//#include "UserValsSet.h"
#include "ODBCRecordset.h"
//#include <afxtempl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserImportDialog dialog


CUserImportDialog::CUserImportDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CUserImportDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserImportDialog)
	m_FileName = _T("");
	m_comma = FALSE;
	m_space = FALSE;
	m_tab = FALSE;
	m_semicolon = FALSE;
	//}}AFX_DATA_INIT
	dateType = Full;
	singleStation = FALSE;
	pDB = NULL;
}


void CUserImportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserImportDialog)
	DDX_Control(pDX, IDC_READ_STATION, m_readStaCtrl);
	DDX_Control(pDX, IDC_DATE1, m_date1Ctrl);
	DDX_Control(pDX, IDC_SELECTEDLIST, m_selectedFields);
	DDX_Control(pDX, IDC_FIELDSLIST, m_availFields);
	DDX_Control(pDX, IDC_COMBO_STATION, m_stationCombo);
	DDX_Text(pDX, IDC_FILENAME, m_FileName);
	DDX_Check(pDX, IDC_CHECKCOMMA, m_comma);
	DDX_Check(pDX, IDC_CHECKSPACE, m_space);
	DDX_Check(pDX, IDC_CHECKTAB, m_tab);
	DDX_Check(pDX, IDC_CHECKSEMICOLON, m_semicolon);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserImportDialog, CDialog)
	//{{AFX_MSG_MAP(CUserImportDialog)
	ON_BN_CLICKED(IDC_MOVELEFT, OnMoveleft)
	ON_BN_CLICKED(IDC_MOVERIGHT, OnMoveright)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_READ_STATION, OnReadStation)
	ON_BN_CLICKED(IDC_SINGLE_STATION, OnSingleStation)
	ON_BN_CLICKED(IDC_DATE1, OnDate1)
	ON_BN_CLICKED(IDC_DATE2, OnDate2)
	ON_BN_CLICKED(IDC_DATE3, OnDate3)
	ON_LBN_DBLCLK(IDC_FIELDSLIST, OnDblclkFieldslist)
	ON_LBN_DBLCLK(IDC_SELECTEDLIST, OnDblclkSelectedlist)
	ON_BN_CLICKED(IDC_FILEBUTTON, OnFilebutton)
	ON_BN_CLICKED(IDC_IMPORTBUTTON, OnImportbutton)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	ON_BN_CLICKED(IDC_BOTTOM, OnBottom)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserImportDialog message handlers

BOOL CUserImportDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CUserVarSet userVars(pDB);
	userVars.Open();
	while(!userVars.IsEOF())
	{
		int loc = m_availFields.AddString(userVars.m_VarName);
		m_availFields.SetItemData(loc, (DWORD)userVars.m_UserVarID);
		userVars.MoveNext();
	}
	userVars.Close();
	m_selectedFields.AddString("StationID");
	m_selectedFields.AddString("Date");
	upButton.AutoLoad(IDC_UP, this);
	downButton.AutoLoad(IDC_DOWN, this);
	topButton.AutoLoad(IDC_TOP, this);
	bottomButton.AutoLoad(IDC_BOTTOM, this);
//	m_spaceCtrl.SetCheck(TRUE);
	m_date1Ctrl.SetCheck(TRUE);
	m_readStaCtrl.SetCheck(TRUE);
	CSIGStationSet staSet(pDB);
	staSet.Open();
	while(!staSet.IsEOF())
	{
		m_stationCombo.AddString(staSet.m_StationID);
		staSet.MoveNext();
	}
	staSet.Close();
	m_stationCombo.EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserImportDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CUserImportDialog::OnMoveleft() 
{
	if(m_selectedFields.GetCurSel() != LB_ERR)
		OnDblclkSelectedlist();
}

void CUserImportDialog::OnMoveright() 
{
	if(m_availFields.GetCurSel() != LB_ERR)
		OnDblclkFieldslist();
}

/*void CUserImportDialog::OnRemoveall() 
{
	for(int i = 0; i < m_selectedFields.GetCount(); i++)
	{
		m_selectedFields.SetCurSel(0);
		OnDblclkSelectedlist();
	}	
}*/

void CUserImportDialog::OnSelectall() 
{
	while(m_availFields.GetCount() > 0)
	{
		m_availFields.SetCurSel(0);
		OnDblclkFieldslist();
	}
}

void CUserImportDialog::OnReadStation() 
{
	m_stationCombo.EnableWindow(FALSE);
	singleStation = FALSE;
	int index = m_selectedFields.FindString(-1, "StationID");
	if(index == LB_ERR)
	{
		m_selectedFields.AddString("StationID");
	}
}

void CUserImportDialog::OnSingleStation() 
{
	m_stationCombo.EnableWindow(TRUE);
	singleStation = TRUE;
	int index = m_selectedFields.FindString(-1, "StationID");
	if(index != LB_ERR)
		m_selectedFields.DeleteString(index);
}

void CUserImportDialog::OnDate1() 
{
	dateType = Full;
	
}

void CUserImportDialog::OnDate2() 
{
	dateType = Packed;
	
}

void CUserImportDialog::OnDate3() 
{
	dateType = Reverse;
	
}

void CUserImportDialog::OnDblclkFieldslist() 
{
	CString str;
	int cur = m_availFields.GetCurSel();
	m_availFields.GetText(cur, str);
	int val = m_availFields.GetItemData(cur);
	int loc = m_selectedFields.AddString(str);
	m_selectedFields.SetItemData(loc, (DWORD)val);
	m_availFields.DeleteString(cur);
}

void CUserImportDialog::OnDblclkSelectedlist() 
{
	CString str;
	int cur = m_selectedFields.GetCurSel();
	m_selectedFields.GetText(cur, str);
	if(str.Compare("StationID") != 0 && str.Compare("Date") != 0)
	{
		int val = m_selectedFields.GetItemData(cur);
		int loc = m_availFields.AddString(str);
		m_availFields.SetItemData(loc, (DWORD)val);
		m_selectedFields.DeleteString(cur);
	}
}

void CUserImportDialog::OnUp() 
{
	CWaitCursor wait;
	if(m_selectedFields.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = m_selectedFields.GetCurSel();
		if(cur > 0)
		{
			m_selectedFields.GetText(cur, str);
			int val = m_selectedFields.GetItemData(cur);
			m_selectedFields.DeleteString(cur);
			cur--;
			int loc = m_selectedFields.InsertString(cur, str);
			m_selectedFields.SetItemData(loc, (DWORD)val);
			m_selectedFields.SetCurSel(loc);
		}
	}
}

void CUserImportDialog::OnTop() 
{
	CWaitCursor wait;
	if(m_selectedFields.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = m_selectedFields.GetCurSel();
		if(cur > 0)
		{
			m_selectedFields.GetText(cur, str);
			int val = m_selectedFields.GetItemData(cur);
			m_selectedFields.DeleteString(cur);
			int loc = m_selectedFields.InsertString(0, str);
			m_selectedFields.SetItemData(loc, (DWORD)val);
			m_selectedFields.SetCurSel(loc);
		}
	}
}

void CUserImportDialog::OnDown() 
{
	CWaitCursor wait;
	if(m_selectedFields.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = m_selectedFields.GetCurSel();
		if(cur < m_selectedFields.GetCount() - 1)
		{
			m_selectedFields.GetText(cur, str);
			int val = m_selectedFields.GetItemData(cur);
			m_selectedFields.DeleteString(cur);
			cur++;
			int loc = m_selectedFields.InsertString(cur, str);
			m_selectedFields.SetItemData(loc, (DWORD)val);
			m_selectedFields.SetCurSel(loc);
		}
	}
}

void CUserImportDialog::OnBottom() 
{
	CWaitCursor wait;
	if(m_selectedFields.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = m_selectedFields.GetCurSel();
		if(cur < m_selectedFields.GetCount() - 1)
		{
			m_selectedFields.GetText(cur, str);
			int val = m_selectedFields.GetItemData(cur);
			m_selectedFields.DeleteString(cur);
			int loc = m_selectedFields.InsertString(-1, str);
			m_selectedFields.SetItemData(loc, (DWORD)val);
			m_selectedFields.SetCurSel(loc);
		}
	}
}

void CUserImportDialog::OnFilebutton() 
{
	CFileDialog	dlg (TRUE, _T("txt"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST |
                          OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT,
		_T("Ascii Text Files (*.txt;*.asc)|*.txt;*.asc|All Files (*.*)|*.*||"), this);
	if(dlg.DoModal() == IDOK)
	{
		UpdateData();
		m_FileName = dlg.GetPathName();
		UpdateData(FALSE);
	}
}


void CUserImportDialog::OnImportbutton() 
{
	UpdateData();
	//AfxMessageBox("Not yet implemented due to database upgrade.");
	//check for fileName
	if(m_FileName.GetLength() <= 0)
	{
		AfxMessageBox("File Name can not be blank.");
		return;
	}
	if(singleStation && m_stationCombo.GetWindowTextLength() <= 0)
	{
		AfxMessageBox("Select a StationID.");
		return;
	}
	if(!m_space && !m_comma && !m_tab && !m_semicolon)
	{
		AfxMessageBox("Select field delimeter.");
		return;
	}
	int nFields = m_selectedFields.GetCount();
	int need = 2;
	if(!singleStation)
		need = 3;
	int nVars = nFields - need + 1;
	if(nFields < need)
	{
		AfxMessageBox("No import variables selected.");
		return;
	}
	FILE *stream = fopen(m_FileName, "rt");
	if(!stream)
	{
		CString msg;
		msg.Format("Error opening file:\n%s", m_FileName);
		AfxMessageBox(msg);
		return;
	}
	EnableWindow(FALSE);
	//create new temp table
	/*CDaoTableDefInfo tInfo;
	for(int i = 0; i < pDB->GetTableDefCount(); i++)
	{
		pDB->GetTableDefInfo(i, tInfo);
		if(tInfo.m_strName.CompareNoCase("ffpTempUser") == 0)
		{
			pDB->DeleteTableDef("ffpTempUser");
			break;
		}
	}*/
	try
	{
		pDB->ExecuteSQL("DROP TABLE ffpTempUser");
	}
	catch(CDBException* e)
	{
		e->Delete();
	}
	/*CDaoFieldInfo fInfo;
	CDaoTableDef userDef(pDB);
	userDef.Open("ffpUserVals");
	CDaoTableDef vt(pDB);
	vt.Create("ffpTempUser");
	for(i = 0; i < userDef.GetFieldCount(); i++)
	{
		userDef.GetFieldInfo(i, fInfo, AFX_DAO_ALL_INFO);
		fInfo.m_nOrdinalPosition = 0;
		vt.CreateField(fInfo);
	}
	userDef.Close();
	vt.Append();
	vt.Close();*/
	try
	{
		pDB->ExecuteSQL("SELECT * INTO ffpTempUser FROM ffpUserVals WHERE 1 = 0");
			//"CREATE TABLE [ffpTempUser] SELECT * FROM [ffpUserVals]");
	}
	catch(CDBException* e)
	{
		DisplayDBException(e);
		return;
	}

	//CUserValsSet records(pDB);
	CODBCRecordset records(pDB);
	records.m_strSort = "[ObsDate]";
	//records.AddFields("ffpUserVals");
	records.Open("SELECT * FROM [ffpUserVals]", CRecordset::dynaset);
	//CUserValsSet tRecs(pDB);
	CODBCRecordset tRecs(pDB);
	tRecs.m_strSort = "[ObsDate]";
	//tRecs.AddFields("ffpTempUser");
	tRecs.Open("SELECT * FROM [ffpTempUser]", CRecordset::dynaset);

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
	     cd->SetStatusString("Reading: " + m_FileName);
	 	cd->SetTitle("Importing User Variable Data");
	  }
	}*/

	
	char *logFileName = GetTempFileName();
	FILE * logFile = fopen(logFileName, "wt");
	COleDateTime dateTime = COleDateTime::GetCurrentTime();
	fprintf(logFile, "FireFamily Plus User Variable Import Log\nStarted: %s", dateTime.Format("%m/%d/%Y at %H:%M:%S"));
	long errors = 0, count = 0;
	fprintf(logFile, "\n***************************************************\nImporting %s", m_FileName);
	if(singleStation)
	{
		CString sStr;
		m_stationCombo.GetWindowText(sStr);
		fprintf(logFile, "\nSingle Station: %s", sStr);
	}
	fprintf(logFile, "\nImport Variables: (in expected column order)\n");
	CString staStr;
	CString fStrs[MAXVARIDS];
	//for(int s = 0; s < nFields; s++)
	//	fStrs[s] = new CString();

	COleDateTime obsDate;
	//int loc = 0;
	for(int i = 0; i < nFields; i++)
	{
		CString vStr;
		m_selectedFields.GetText(i, vStr);
		fprintf(logFile, "%s ", vStr);
		if(vStr.Compare("StationID") != 0 && vStr.Compare("Date") != 0)
		{
			fStrs[i].Format("%ld", m_selectedFields.GetItemData(i));
			//loc++;
		}
		else if(vStr.Compare("Date") == 0)
			fStrs[i] = "ObsDate";
		else
			fStrs[i] = vStr;
	}
	fprintf(logFile, "\n");
	double *vals = new double[nVars];
	char buf[1024];
	CList<CString, CString> staList;
	if(singleStation)
	{
		m_stationCombo.GetWindowText(staStr);
		staList.AddHead(staStr);
	}
	//open database and prepare to append records.
	char delim[8] = "";
	if(m_comma)
		strcpy(delim, ",");
	if(m_space)
	{
		if(strlen(delim) <= 0)
			strcpy(delim, " ");
		else
			strcat(delim, " ");
	}
	if(m_tab)
	{
		if(strlen(delim) <= 0)
			strcpy(delim, "\t");
		else
			strcat(delim, "\t");
	}
	if(m_semicolon)
	{
		if(strlen(delim) <= 0)
			strcpy(delim, ";");
		else
			strcat(delim, ";");
	}
	int v = 0;
	CSIGStationSet stnSet(pDB);
	stnSet.Open();
	long rejects = 0;
	pDB->BeginTrans();
	while(!feof(stream))// && (!cd || !cd->Abort()))
	{
		fgets(buf, 1023, stream);
		if(strlen(buf) <= 0)
			continue;
		//ok, got a record....parse into values
		count++;
		bool valid = true;
		char *ptr = strtok(buf, delim);
		v = 0;
		int i;
		for(i = 0; i < nFields && ptr; i++)
		{

			if(fStrs[i].Compare("StationID") == 0)//station
			{
				staStr.Format("%6.6s", ptr);
			}
			else if(fStrs[i].Compare("ObsDate") == 0)//ObsDate
			{
				obsDate.ParseDateTime(ptr, VAR_DATEVALUEONLY);
				//obsDate.SetTime(13, 0, 0);
				
			}
			else//must be a variable
			{
				vals[v] = atof(ptr);
				v++;
			}
			ptr = strtok(NULL, delim);
		}
		if(v < nVars)
		{
			fprintf(logFile, "\nError line %ld: Too few number of variables.", count);
			valid = false;
			errors++;
		}
		if(obsDate.GetStatus())
		{
			fprintf(logFile, "\nError: Invalid Date line %ld", count);
			valid = false;
			errors++;
		}
		if(!valid)
		{
			rejects++;
			continue;
		}
		//records.m_strFilter.Format("[StationID] = '%s' and [ObsDate] < #%d/%d/%d#",
			//staStr, obsDate.GetMonth(), obsDate.GetDay(), obsDate.GetYear());
		//records.Requery();
		//if(!records.IsBOF() && !records.IsEOF())
			//records.Edit();
		//else
		tRecs.AddNew();
		//for(i = 2; i < tRecs.GetFieldCount(); i++)
		//	tRecs.SetFieldNull(
		v = 0;
		if(singleStation)
		{
			//COleVariant var(staStr, VT_BSTRT );
			//tRecs.SetFieldValue("StationID", var);
			//tRecs.m_StationID = staStr;
			tRecs.Field("StationID") = staStr;
		}
		for(i = 0; i < nFields; i++)
		{
			if(fStrs[i].Compare("StationID") == 0)
			{
				//COleVariant var(staStr, VT_BSTRT );
				//tRecs.SetFieldValue(fStrs[i], var);
				//tRecs.m_StationID = staStr;
				tRecs.Field("StationID") = staStr;
			}
			else if(fStrs[i].Compare("ObsDate") == 0)
			{
				//COleVariant var(obsDate);
				//tRecs.SetFieldValue(fStrs[i], var);
				//CTime ttime(obsDate.GetYear(), obsDate.GetMonth(), obsDate.GetDay(),
				//	obsDate.GetHour(), obsDate.GetMinute(), obsDate.GetSecond());
				//tRecs.m_ObsDate = ttime;
				tRecs.Field("ObsDate") = obsDate;
			}
			else
			{
				//COleVariant fVar(vals[v]);
				//tRecs.SetFieldValue(fStrs[i], fVar);
				//tRecs.SetFieldValue(fStrs[i], vals[v]);
				tRecs.Field(fStrs[i]) = vals[v];
				v++;
			}
		}
		
		tRecs.Update();
		if(!singleStation)
		{
			if(staList.IsEmpty())
				staList.AddTail(staStr);
			else
			{
				bool there = false;
				POSITION pos = staList.GetHeadPosition();
				while(pos)
				{
					if(staList.GetNext(pos) == staStr)
					{
						there = true;
						break;
					}
				}
				if(!there)
					staList.AddHead(staStr);
			}
		}
		//if (cd)
		//   cd->Increment();
	}
	//to maintain referential integrity,
	//must add any new stations to wxSation table
	int staAdded = 0;
	//if(!cd || !cd->Abort())
	//{
		POSITION pos = staList.GetHeadPosition();
		while(pos)
		{//check each station
			CString strSQL, sta;
			sta = staList.GetNext(pos);
			//strSQL.Format("[StationID] = '%6.6s'", sta); 
			//if(!stnSet.FindFirst(strSQL))
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
				fprintf(logFile, "Added Station: %s\n", sta);
				staAdded++;
			}
		}
	//}
	long edits = 0, appends = 0;
	//if(!cd || !cd->Abort())
	//{
		pDB->CommitTrans();
		/*if (cd)
		{
		   cd->Reset();
	    	cd->SetStatusString("Updating Database");
		}*/
		pos = staList.GetHeadPosition();
		while(pos)// && (!cd || !cd->Abort()))
		{//process for each station
			CString strSQL, sta;
			sta = staList.GetNext(pos);
			strSQL.Format("[StationID] = '%6.6s'", sta);
			records.m_strFilter = strSQL;
			tRecs.m_strFilter = strSQL;
			records.Requery();
			tRecs.Requery();
			pDB->BeginTrans();
			//long recPos = 1;
			while(!tRecs.IsEOF())// && (!cd || !cd->Abort()))
			{
				//if (cd)
				//   cd->Increment();
				//CTime tDate, rDate;
				//CDBVariant varRecordToReturnTo;
				bool didAppend = false;
				COleDateTime tDate, rDate;
				//CDBVariant tVar, rVar;
				//tRecs.GetFieldValue("ObsDate", tVar);
				//tDate = tVar.m_pdate;
				tDate = tRecs.GetDate("ObsDate");//.m_ObsDate;
				if(!records.IsEOF())
				{
					//records.GetBookmark( varRecordToReturnTo );
					//records.GetFieldValue("ObsDate", rVar);
					//rDate = rVar.date;
					rDate = records.GetDate("ObsDate");//.m_ObsDate;
					while(!records.IsEOF() && (rDate < tDate))
					{
						records.MoveNext();
						//records.GetBookmark( varRecordToReturnTo );
						//recPos++;
						if(!records.IsEOF())
						{
							//records.GetFieldValue("ObsDate", rVar);
							//rDate = rVar.date;
							rDate = records.GetDate("ObsDate");//.m_ObsDate;
						}
					}
				}
				//else
				//	rDate = tDate;
				if(!records.IsEOF() && (rDate == tDate))
				{
					//editRow++;// = records.GetP
					records.Edit();
					edits++;
				}
				else
				{
					records.AddNew();
					//varRecordToReturnTo.Clear();
					didAppend = true;
					appends++;
				}
				//records.m_StationID = tRecs.m_StationID;
				//records.m_ObsDate = tRecs.m_ObsDate;
				//POSITION fPos = tRecs.m_listName.GetHeadPosition();
				//POSITION vPos = tRecs.m_listValues.GetHeadPosition();
				//while(fPos)
				//{
					//CString fName = tRecs.m_listName.GetNext(fPos);
					//CUserVal uVal = tRecs.m_listValues.GetNext(vPos);
				//	int loc = atoi(fName);
					//records.SetFieldValue(fName, (double) uVal.floatVal);
				//	records.SetFieldValue(fName, (double) records.values[loc]);
				//}
				for(int i = 0; i < records.GetODBCFieldCount(); i++)
				{
					//CODBCFieldInfo fieldInfo;
					//tRecs.GetODBCFieldInfo(i, fieldInfo);
					//if(!tRecs.IsFieldNullable(&fieldInfo) || (tRecs.IsFieldNullable(&fieldInfo) && !tRecs.IsFieldNull(&fieldInfo)))
					if(!tRecs.IsFieldStatusNull(i))
					{
						CDBVariant var;
						tRecs.GetFieldValue(i, var);
						if(var.m_dwType != DBVT_NULL)
						{
							switch(var.m_dwType)
							{
							case	DBVT_NULL:
								records.SetNullFieldStatus(i);
								break;
							case	DBVT_BOOL:
								records.Field(i) = tRecs.Field(i).AsBool();
								break;
							case	DBVT_UCHAR:
								records.Field(i) = tRecs.Field(i).AsChar();
								break;
							case	DBVT_SHORT:
								records.Field(i) = tRecs.Field(i).AsShort();
								break;
							case	DBVT_LONG:
								records.Field(i) = tRecs.Field(i).AsLong();
								break;
							case	DBVT_SINGLE:
								records.Field(i) = tRecs.Field(i).AsFloat();
								break;
							case	DBVT_DOUBLE:
								records.Field(i) = tRecs.Field(i).AsDouble();
								break;
							case	DBVT_DATE:
								records.Field(i) = tRecs.Field(i).AsDate();
								break;
							case	DBVT_STRING:
							case	DBVT_ASTRING:
							case	DBVT_WSTRING:
								records.Field(i) = tRecs.Field(i).AsString();
								break;
							case	DBVT_BINARY:
								records.Field(i) = tRecs.Field(i).AsBinary();
								break;
							}
						}
					}
					else
						records.SetNullFieldStatus(i);
				}
				records.Update();
				//if(varRecordToReturnTo.m_dwType
				//records.SetBookmark(varRecordToReturnTo);
				//records.SetAbsolutePosition(editRow);
				tRecs.MoveNext();
				if(didAppend)
					records.MoveLast();
			}
			//if(!cd || !cd->Abort())
				pDB->CommitTrans(); //so we can requery
			//else
			//	pDB->Rollback();
		}
	//}
	//else
	//	pDB->Rollback();
	EnableWindow(TRUE);
	CString strMsg;
	//if(!cd || !cd->Abort())
	//{
		fprintf(logFile, "\n\nProcessed %ld user records for %d stations:\n"
			"\tUpdated %ld records.\n"
			"\tAppended %ld records.\n"
			"\tRejected %ld records.\n"
			"\tAdded %d new stations.\n\nBe sure to fill in metadata for any new stations\n", 
			count, staList.GetCount(), edits, appends, rejects, staAdded);
		strMsg.Format("Import Complete.\n%ld records imported.\n%ld errors were encountered.\n", count, errors);
	/*}
	else//cancelled
	{
		pDB->Rollback();
		fprintf(logFile, "\nImport cancelled.\n");
		strMsg.Format("Import cancelled.");
		//fclose(errLog);
		//errors = -1;
	}
	if (cd)
		cd->DestroyWindow();*/

	stnSet.Close();
	records.Close();
	tRecs.Close();
	fclose(stream);
	fclose(logFile);
	CImportDoneDialog idd(this);
	idd.statusStr = strMsg;
	idd.logFileName = logFileName;
	staList.RemoveAll();
	delete[] vals;
	free(logFileName);
	idd.DoModal();
		//CDialog::OnOK();
}

// UserVarsDialog.cpp : implementation file
//
#include "stdafx.h"
#include "fireplus.h"
#include "UserVarsDialog.h"
#include "UserVarSet.h"
#include "ClimateSet.h"
#include "SeasonBinSet.h"
#include "columnst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserVarsDialog dialog


CUserVarsDialog::CUserVarsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CUserVarsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserVarsDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	pDB = NULL;
}


void CUserVarsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserVarsDialog)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserVarsDialog, CDialog)
	//{{AFX_MSG_MAP(CUserVarsDialog)
	ON_BN_CLICKED(IDC_DELETEVAR, OnDeletevar)
	ON_BN_CLICKED(IDC_NEWVAR, OnNewvar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserVarsDialog message handlers

BOOL CUserVarsDialog::OnInitDialog() 
{
	CString str;
	CDialog::OnInitDialog();
	m_grid.AttachGrid(this, IDC_GRID);
	int row = 0, rows = 0;
	if(pDB)
	{
		CUserVarSet varSet(pDB);
		varSet.Open();
		CClimateSet climSet(pDB);
		//climSet.m_strFilter = "[OptionType] = 0 Or [OptionType] = 1 Or [OptionType] = 3 Or [OptionType] = 4";
		climSet.Open();
		while(!varSet.IsEOF() && !varSet.IsBOF())
		{
			rows++;
			varSet.MoveNext();
		}
		if(rows > 0)
		{
			CUGCell cell;
			m_grid.SetNumberRows(rows);
			varSet.MoveFirst();
			while(!varSet.IsEOF() && !varSet.IsBOF())
			{
				climSet.m_strFilter.Format("[VarID] = %d", varSet.m_VarID);
				climSet.Requery();
				m_grid.QuickSetText(0, row, varSet.m_VarName); 
				m_grid.QuickSetText(1, row, varSet.m_Abbrev);
				str.Format("%d", (int)varSet.m_Decimals);
				m_grid.QuickSetText(2, row, str); 
				m_grid.GetCell(2, row, &cell);
				cell.SetReadOnly(FALSE);
				cell.SetParam(USE_COXNUMBER);
				m_grid.SetCell( 2, row, &cell );
				m_grid.GetCell(3, row, &cell);
				str.Format("%.2f", climSet.m_BinSize);
				cell.SetReadOnly(FALSE);
				cell.SetText(str);
				cell.SetParam(USE_COXNUMBER);
				m_grid.SetCell( 3, row, &cell );
				str.Format("%d", (int)varSet.m_VarID);
				m_grid.QuickSetText(4, row, str); 
				str.Format("%d", (int)varSet.m_UserVarID);
				m_grid.QuickSetText(5, row, str); 
				varSet.MoveNext();
				row++;
			}
		}
		climSet.Close();
		varSet.Close();
	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserVarsDialog::OnOK() 
{
	//AfxMessageBox("Not yet implemented due to database upgrade.");
	CUGCell cell;
	short vID, uID;
	CString msg;
	int i;
	for(i = 0; i < m_grid.GetNumberRows(); i++)
	{
		CString vStr;
		int vDecs;
		double vBinSize;
		vStr = m_grid.QuickGetText(0, i);
		vStr.TrimRight();
		if(vStr.GetLength() <= 0)
		{
			msg.Format("Error: Record %d Encountered a blank Full Name.", i);
			AfxMessageBox(msg);
			return;
		}
		int c;
		for(c = 0; c < vStr.GetLength(); c++)
		{
			if(!isalnum(vStr[c]))
			{
				msg.Format("Error: Record %d Variable Full Name '%s' contains an invalid character.\nOnly letters and numbers allowed",
					i, vStr);
				AfxMessageBox(msg);
				return;
			}
		}
		vStr = m_grid.QuickGetText(1, i);
		vStr.TrimRight();
		if(vStr.GetLength() <= 0)
		{
			msg.Format("Error: Record %d Encountered a blank Abbreviation.", i);
			AfxMessageBox(msg);
			return;
		}
		for(c = 0; c < vStr.GetLength(); c++)
		{
			if(!isalnum(vStr[c]))
			{
				msg.Format("Error: Record %d Variable Abbreviation '%s' contains an invalid character.\nOnly letters and numbers allowed",
					i, vStr);
				AfxMessageBox(msg);
				return;
			}
		}
		vStr = m_grid.QuickGetText(2, i);
		if(atoi(vStr) < 0)
		{
			msg.Format("Error. Record %d Decimals field must be greater than or equal to zero.", i + 1);
			AfxMessageBox(msg);
			return;
		}
		float binsz;
		m_grid.GetCell(3, i, &cell);
		binsz = cell.GetNumber();//pNumCell->GetNumber();
		if(binsz <= 0)
		{
			msg.Format("Error. Record %d Analysis Bin Size field must be greater than zero.", i + 1);
			AfxMessageBox(msg);
			return;
		}

	}

	//save the table to the database	
	//make modifications to ClimateOptions( add new user variables or update names)

	CUserVarSet varSet(pDB);
	CClimateSet climSet(pDB);
	CSeasonBinSet binSet(pDB);
	//CDaoTableDef tableDef(pDB);
	//tableDef.Open("ffpUserVals");
	varSet.Open();
	climSet.Open();
	binSet.Open();
	while(!varSet.IsEOF())
	{
		varSet.Delete();
		varSet.MoveNext();
	}
	for(i = 0; i < m_grid.GetNumberRows(); i++)
	{
		varSet.AddNew();
		varSet.m_VarName = m_grid.QuickGetText(0, i);
		varSet.m_VarName.TrimRight();
		varSet.m_Abbrev = m_grid.QuickGetText(1, i);
		varSet.m_Abbrev.TrimRight();
		CString decs;
		decs = m_grid.QuickGetText(2, i);
		varSet.m_Decimals = atoi(decs);
		m_grid.GetCell(4, i, &cell);
		vID = varSet.m_VarID = cell.GetNumber();
		varSet.m_VarID = vID;
		m_grid.GetCell(5, i, &cell);
		uID = varSet.m_UserVarID = cell.GetNumber();
		varSet.m_UserVarID = uID;

		climSet.m_strFilter.Format("[VarID] = %d", varSet.m_VarID);
		climSet.Requery();
		if(!climSet.IsBOF() && !climSet.IsEOF())
			climSet.Edit();
		else
		{
			climSet.AddNew();
			climSet.m_VarID = varSet.m_VarID;
		}
		float binsz;
		m_grid.GetCell(3, i, &cell);
		binsz = cell.GetNumber();
		climSet.m_Variable_Name = varSet.m_VarName;
		climSet.m_ShortName = varSet.m_Abbrev;
		climSet.m_OptionType = 4;
		climSet.m_BinSize = binsz;
		//modify structure of value table if necessary
		bool colFound = false;
		/*for(int c = 2; c < tableDef.GetFieldCount(); c++)
		{
			CDaoFieldInfo fieldinfo;
			tableDef.GetFieldInfo(c, fieldinfo);
			if(atoi(fieldinfo.m_strName) == uID)
				colFound = true;
		}*/
		try
		{
			CColumns columns(pDB);
			columns.m_strTableNameParam = "ffpUserVals";
			columns.Open();
			while (!columns.IsEOF())
			{
				if(atoi(columns.m_strColumnName) == uID)
					colFound = true;
				columns.MoveNext();
			}

			columns.Close();
		}
		catch(CDBException* e)
		{
			e->Delete();
		}
		if(!colFound)
		{
			//CString fName;
			//fName.Format("%d", uID);
			//tableDef.CreateField(fName, dbDouble, 8, dbUpdatableField | dbFixedField);
			try
			{
				CString strSql;
				strSql.Format("ALTER TABLE [ffpUserVals] ADD [%d] REAL", uID);
				pDB->ExecuteSQL(strSql);
			}
			catch(CDBException* e)
			{
				e->Delete();
			}
		}
		binSet.m_strFilter.Format("[VarID] = %d", vID);
		binSet.Requery();
		if(!binSet.IsBOF() && !binSet.IsEOF())
			binSet.Edit();
		else
		{
			binSet.AddNew();
			binSet.m_VarID = varSet.m_VarID;
		}
		varSet.Update();
		climSet.Update();
		binSet.Update();
	}
	//adjust varIDs if necessary
	short nextID = STATICVARIDS + 1;
	varSet.m_strFilter = "";
	varSet.m_strSort.Format("[VarID]");
	varSet.Requery();
	while(!varSet.IsEOF())
	{
		if(varSet.m_VarID > nextID) //bump it
		{
			//CString seek;
			//seek.Format("[VarID] = %d", varSet.m_VarID);
			climSet.m_strFilter.Format("[VarID] = %d", varSet.m_VarID);
			climSet.Requery();
			binSet.m_strFilter.Format("[VarID] = %d", varSet.m_VarID);
			binSet.Requery();
			if(!climSet.IsEOF() && !binSet.IsEOF())
			{//change the varIDs
				climSet.Edit();
				climSet.m_VarID = nextID;
				climSet.Update();
				binSet.Edit();
				binSet.m_VarID = nextID;
				binSet.Update();
				varSet.Edit();
				varSet.m_VarID = nextID;
				varSet.Update();
			}
		}
		nextID++;
		varSet.MoveNext();
	}
	//tableDef.Close();
	varSet.Close();
	climSet.Close();
	binSet.Close();
	CDialog::OnOK();
}

void CUserVarsDialog::OnNewvar() 
{
	CUGCell cell;
	short userID = m_grid.GetNumberRows() + 1, lastID = 0;
	if(userID > MAXVARIDS - STATICVARIDS)
	{
		AfxMessageBox(
			"You have exceeded the maximum number of user defined variables\n"
			"for a Firefamily Plus database. The database has succumbed to\n"
			"the torture and the numbers will tell you anything you want them to say.");
		return;
	}
	short tID;
	bool found;
	for(lastID = 1; lastID < userID; lastID++)
	{
		found = false;
		for(int i = 0; i < m_grid.GetNumberRows(); i++)
		{
			tID = atoi(m_grid.QuickGetText(5, i));
			if(tID == lastID)
			{
				found = true;
				break;
			}
		}
		if(!found)
		{
			userID = lastID;
			break;
		}
	}
	m_grid.AppendRow();
	int row = m_grid.GetNumberRows() - 1;
	m_grid.QuickSetText(0, row, "Name");
	m_grid.QuickSetText(1, row, "Var");
	m_grid.GetCell(2, row, &cell);
	cell.SetNumber(0);
	cell.SetNumberDecimals(0);
	cell.SetParam(USE_COXNUMBER);
	m_grid.SetCell(2, row, &cell);
	m_grid.GetCell(3, row, &cell);
	cell.SetNumber(1.0);
	cell.SetNumberDecimals(1);
	cell.SetParam(USE_COXNUMBER);
	m_grid.SetCell(3, row, &cell);
	m_grid.GetCell(4, row, &cell);
	cell.SetNumber(STATICVARIDS + userID);
	cell.SetReadOnly(TRUE);
	m_grid.SetCell(4, row, &cell);
	m_grid.GetCell(5, row, &cell);
	cell.SetNumber(userID);
	cell.SetReadOnly(TRUE);
	m_grid.SetCell(5, row, &cell);
	m_grid.GotoCell(0, row);
	m_grid.RedrawRow(row);
}

void CUserVarsDialog::OnDeletevar() 
{
	long Row = m_grid.GetCurrentRow();//m_table.GetCurCell().GetRow();
	CString strMsg, vName;
	vName = m_grid.QuickGetText(0, Row);
	vName.TrimRight();
	strMsg.Format(
		"Permanently delete variable \"%s\"?\n"
		"WARNING: This will also remove any data values\n"
		"for \"%s\" from the database.", vName, vName);
	if(AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		DeleteUserVar(Row);
		m_grid.DeleteRow(Row);
	}
}


void CUserVarsDialog::DeleteUserVar(long Row)
{
	//AfxMessageBox("Not yet implemented due to database upgrade.");
	//remove all instances of the user variable from the database
	//delete from climateOptions & useVals & userVars
	short delVar, delUserVar;
	CUGCell cell;
	m_grid.GetCell(4, Row, &cell);
	delVar = cell.GetNumber();
	m_grid.GetCell(5, Row, &cell);
	delUserVar = cell.GetNumber();
	CClimateSet climSet(pDB);
	climSet.m_strFilter.Format("[VarID] = %d", delVar);
	climSet.Open();
	if(!climSet.IsBOF() && !climSet.IsEOF())
		climSet.Delete();
	climSet.Close();
	//CDaoTableDef tableDef(pDB);
	//tableDef.Open("ffpUserVals");
	//for(int c = 2; c < tableDef.GetFieldCount(); c++)
	//{
	//	CDaoFieldInfo fieldinfo;
	//	tableDef.GetFieldInfo(c, fieldinfo);
	//	if(atoi(fieldinfo.m_strName) == delUserVar)
	//	{
	////		tableDef.DeleteField(c);
	//		break;
	//	}
	//}
	//tableDef.Close();
	try
	{
		CString strSql;
		strSql.Format("ALTER TABLE [ffpUserVals] DROP COLUMN [%d]", delUserVar);
		pDB->ExecuteSQL(strSql);
	}
	catch(CDBException *e)
	{
		DisplayDBException(e);
		e->Delete();
	}
	CSeasonBinSet binSet(pDB);
	binSet.m_strFilter.Format("[VarID] = %d", delVar);
	binSet.Open();
	if(!binSet.IsBOF() && !binSet.IsEOF())
		binSet.Delete();
	binSet.Close();

	CUserVarSet varSet(pDB);
	varSet.m_strFilter.Format("[UserVarID] = %d", delUserVar);
	varSet.Open();
	if(!varSet.IsBOF() && !varSet.IsEOF())
		varSet.Delete();
	varSet.Close();
}


// SIGEditDialog.cpp : implementation file
//
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
//#include "wxset.h"
#include "sigset.h"
#include "SIGEditDialog.h"
#include "SIGStationSet.h"
#include "StationInSIGSet.h"
#include ".\sigeditdialog.h"
#include "WxStationSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSIGEditDialog dialog

CSIGEditDialog::CSIGEditDialog(CSIGSet *_sigSet, CWnd* pParent /*=NULL*/, bool newSIG /*= false*/)
	: CDialog(CSIGEditDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSIGEditDialog)
	nameString = _T("");
	//}}AFX_DATA_INIT
	sigSet = _sigSet;
	isNew = newSIG;
}

void CSIGEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSIGEditDialog)
	DDX_Control(pDX, IDC_MOVERIGHT, moveRight);
	DDX_Control(pDX, IDC_MOVELEFT, moveLeft);
	DDX_Control(pDX, IDC_SIGNAME, SIGName);
	DDX_Control(pDX, IDC_SIGDESCRIPTION, description);
	DDX_Control(pDX, IDC_LIST2, stationList);
	DDX_Control(pDX, IDC_LIST1, selectedList);
	DDX_Text(pDX, IDC_SIGNAME, nameString);
	DDV_MaxChars(pDX, nameString, 14);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSIGEditDialog, CDialog)
	//{{AFX_MSG_MAP(CSIGEditDialog)
	ON_LBN_DBLCLK(IDC_LIST2, OnDblclkList2)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	ON_BN_CLICKED(IDC_MOVELEFT, OnMoveleft)
	ON_BN_CLICKED(IDC_MOVERIGHT, OnMoveright)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SETWEIGHTS, &CSIGEditDialog::OnBnClickedButtonSetweights)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSIGEditDialog message handlers
BOOL CSIGEditDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	SIGName.SetLimitText(14);
	description.SetLimitText(50);
	//populate the available stations list
	CSIGStationSet staSet(sigSet->m_pDatabase);
	//staSet.m_strSort
	staSet.Open();
	CWxStationSet usedStations(sigSet->m_pDatabase);
	usedStations.Open(CRecordset::dynaset, "SELECT DISTINCT WXObs.StationID FROM WXObs   ORDER BY WXObs.StationID;");

	if (usedStations.IsOpen())
	{
		while (!usedStations.IsEOF())
		{
			CString str = usedStations.m_StationID;
			str.TrimRight();
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", str);
			staSet.Requery();
			str += " - " + staSet.m_Name;
			stationList.AddString(str);
			//m_ctlSIG_Station.SetItemData(loc, 0);
			//if(usedStations.m_StationID == m_pSet->m_SIG_Station)
				//m_ctlSIG_Station.SetCurSel(loc);
			usedStations.MoveNext();
			//loc++;
		}
	}
	usedStations.Close();
	/*if (staSet.IsOpen())
	{
		while (!staSet.IsEOF())
		{
			stationList.AddString(CString(staSet.m_StationID + " - " + staSet.m_Name));
			staSet.MoveNext();
		}
	}*/
	staSet.Close();
	if(!isNew)
	{
		//grey out key field if already exists
		SIGName.SetWindowText(sigSet->m_SIG);
		SIGName.EnableWindow(false);
		description.SetWindowText(sigSet->m_DESCRIPTION);
		//get selected stations
		CStationInSIGSet staSigSet(sigSet->m_pDatabase);
		staSigSet.m_strFilter = "[SIG] = '" + sigSet->m_SIG + "'";
		staSigSet.Open();
		if (staSigSet.IsOpen())
		{
			while (!staSigSet.IsEOF())
			{
				//if(stationList.SelectString(-1, staSigSet.m_SIG) != LB_ERR)
				for(int j = 0; j < stationList.GetCount(); j++)
				{
					CString staStr;
					stationList.GetText(j, staStr);
					if(strncmp(staStr, staSigSet.m_StationID, 6) == 0)
					{
						stationList.SetCurSel(j);
						OnDblclkList2();
						break;
					}
				}
				staSigSet.MoveNext();
			}
		}
		staSigSet.Close();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSIGEditDialog::OnDblclkList2()
{
	// TODO: Add your control notification handler code here
	//user double clicked a string, so move it to selected list
	CString str;
	int cur = stationList.GetCurSel();
	stationList.GetText(cur, str);
	selectedList.AddString(str);
	stationList.DeleteString(cur);
}

void CSIGEditDialog::OnDblclkList1()
{
	// TODO: Add your control notification handler code here
	//user double clicked a string, so move it to station list
	CString str;
	int cur = selectedList.GetCurSel();
	selectedList.GetText(cur, str);
	stationList.AddString(str);
	selectedList.DeleteString(cur);
}

void CSIGEditDialog::OnMoveleft()
{
	// TODO: Add your control notification handler code here
	if(stationList.GetCurSel() != LB_ERR)
		OnDblclkList2();
}

void CSIGEditDialog::OnMoveright()
{
	// TODO: Add your control notification handler code here
	if(selectedList.GetCurSel() != LB_ERR)
		OnDblclkList1();
}

void CSIGEditDialog::OnOK()
{
	// TODO: Add extra validation here
	//make sure fields correctly filled in
	if(isNew)
	{
		CString cstr;
		SIGName.GetWindowText(cstr);
		cstr.TrimRight();
		cstr.TrimLeft();
		if(cstr.GetLength() <= 0)
		{
			CString strMsg;
			strMsg.Format(
				"ERROR: SIG Name can not be blank.\n");
			AfxMessageBox(strMsg);
			return;
		}
		else if(cstr.GetLength() > 14)
		{
			CString strMsg;
			strMsg.Format(
				"ERROR: SIG Name can not exceed 14 characters.\n");
			AfxMessageBox(strMsg);
			return;
		}
		//ensure SIG is filled in AND unique
		char str1[64];//, str2[64];
		//SIGName.GetWindowText(str2, 63);
		sprintf(str1, "[SIG] = '%-20s'", cstr);
		sigSet->m_strFilter = str1;
		sigSet->Requery();
		if(!sigSet->IsEOF())
		//if(sigSet->Find(AFX_DAO_FIRST, str1) != 0)
		{
			CString strMsg;
			strMsg.Format(
				"ERROR: SIG %s already exists.\nSIG Name must be unique.\n", cstr);
			AfxMessageBox(strMsg);
			return;
		}
	}
	if(selectedList.GetCount() <= 0)
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: At least one station must be selected.\n");
		AfxMessageBox(strMsg);
		return;
	}
	//data OK, write the record to the recordset
	if(isNew)
	{
		CString cstr;
		SIGName.GetWindowText(cstr);
		cstr.TrimRight();
		cstr.TrimLeft();
		sigSet->AddNew();
		sigSet->m_SIG = cstr;
	}
	else
		sigSet->Edit();
	//write description field
	CString descStr, dStr;
	description.GetWindowText(dStr);
	//descStr.Format("%s", dStr);
	sigSet->m_DESCRIPTION = dStr;
	//complete the writing of the record to the SIGSET
	//sigSet->Update();

	//now update the stations
	CStationInSIGSet staSigSet(sigSet->m_pDatabase);
	staSigSet.m_strFilter = "[SIG] = '" + sigSet->m_SIG + "'";
	staSigSet.Open();
	if (staSigSet.IsOpen())
	{
		//remove any existing records not in the selected list

		if(!staSigSet.IsBOF() && !staSigSet.IsEOF())
		{
			staSigSet.MoveFirst();
			while (!staSigSet.IsEOF())
			{
				if(selectedList.FindString(-1, staSigSet.m_StationID) == LB_ERR)
				{
					staSigSet.Delete();
				}
				staSigSet.MoveNext();
			}
		}
		//add the new records or edit existing
		bool exists;
		for(int i = 0; i < selectedList.GetCount(); i++)
		{
			CString cstr;
			exists = false;
			selectedList.GetText(i, cstr);
			if(!staSigSet.IsBOF() || !staSigSet.IsEOF())
			{
				staSigSet.MoveFirst();
				while(!staSigSet.IsEOF())
				{
					//if(cstr.Compare(staSigSet.m_StationID) == 0)
					if(strncmp(cstr, staSigSet.m_StationID, 6) == 0)
					{
						exists = true;
						break;
					}
					staSigSet.MoveNext();
				}
			}
			if(!exists)
			{
				staSigSet.AddNew();
				staSigSet.m_SIG = sigSet->m_SIG;
				staSigSet.m_StationID = cstr.Left(6);
				staSigSet.m_WeightFactor = 1.0;
				staSigSet.Update();
			}
		}
	}
	staSigSet.Close();
	sigSet->Update();

	CDialog::OnOK();
}

void CSIGEditDialog::OnBnClickedButtonSetweights()
{
	//Allow user to set weights for indiviual stations...
	if(selectedList.GetCount() <= 0)
	{
		CString strMsg;
		strMsg.Format(
			"ERROR: At least one station must be selected.\n");
		AfxMessageBox(strMsg);
		return;
	}
	//first, check if list has changed...
	bool changed = false;
	CStationInSIGSet staSigSet(sigSet->m_pDatabase);
	staSigSet.m_strFilter = "[SIG] = '" + sigSet->m_SIG + "'";
	staSigSet.Open();
	int stnCount = 0;
	if (staSigSet.IsOpen())
	{
		while(!staSigSet.IsEOF())
		{
			stnCount++;
			if(selectedList.FindString(-1, staSigSet.m_StationID) == LB_ERR)
				changed = true;
			staSigSet.MoveNext();
		}
		if(changed || stnCount != selectedList.GetCount())//something's changed...
		{
			if(IDYES == AfxMessageBox("One or more stations in the SIG have changed, save new stations for the SIG?", MB_YESNO))
			{
			}
		}
	}
}
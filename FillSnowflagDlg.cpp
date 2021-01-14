// FillSnowflagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "FillSnowflagDlg.h"
#include "afxdialogex.h"
#include "FireplusDoc.h"
#include "WxStationSet.h"
#include "SIGStationSet.h"
#include "WxSet.h"


// CFillSnowflagDlg dialog

IMPLEMENT_DYNAMIC(CFillSnowflagDlg, CDialogEx)

CFillSnowflagDlg::CFillSnowflagDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_FILL_SNOWFLAG, pParent)
{
	m_pDoc = NULL;
}

CFillSnowflagDlg::~CFillSnowflagDlg()
{
}

void CFillSnowflagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_STATION, m_comboStation);
}


BEGIN_MESSAGE_MAP(CFillSnowflagDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CFillSnowflagDlg::OnBnClickedApply)
END_MESSAGE_MAP()


// CFillSnowflagDlg message handlers


void CFillSnowflagDlg::OnBnClickedApply()
{
	CString staStr, msg;
	int dailySnowFlag;
	m_comboStation.GetWindowTextA(staStr);
	int dailyNulls = 0;
	int nUpdated = 0;
	int nChanged = 0;
	if (staStr.GetLength() > 0)
	{
		msg.Format("Fill RAWS record SnowFlag values to match daily observation SnowFlag values for station %s? Warning, this process may take a long time.", staStr);
		if (IDYES != AfxMessageBox(msg, MB_YESNO))
			return;
		CWaitCursor wait;
		//BOOL bOK = m_pDoc->m_pDB->BeginTrans();
		//if (bOK)
		//{
			CWxSet dailySet(m_pDoc->m_pDB), hourlySet(m_pDoc->m_pDB);
			dailySet.m_strFilter.Format("[StationID] = '%6.6s' AND [DailyObs] = 1", staStr);
			dailySet.m_strSort = _T("[ObsDate]");
			dailySet.Open();
			hourlySet.m_strSort = _T("[ObsDate]");
			hourlySet.m_strFilter.Format("[StationID] = '%6.6s'", staStr);
			hourlySet.Open();
			while (!dailySet.IsBOF() && !dailySet.IsEOF())
			{
				if (dailySet.IsFieldNull(&dailySet.m_SnowFlag))
				{
					dailyNulls++;
					continue;
				}
				dailySnowFlag = dailySet.m_SnowFlag;
				while (!hourlySet.IsBOF() && !hourlySet.IsEOF() 
					&& hourlySet.m_ObsDate.GetYear() <= dailySet.m_ObsDate.GetYear()
					&& hourlySet.m_ObsDate.GetMonth() <= dailySet.m_ObsDate.GetMonth()
					&& hourlySet.m_ObsDate.GetDay() <= dailySet.m_ObsDate.GetDay())
				{
					if (hourlySet.m_ObsDate.GetYear() == dailySet.m_ObsDate.GetYear()
						&& hourlySet.m_ObsDate.GetMonth() == dailySet.m_ObsDate.GetMonth()
						&& hourlySet.m_ObsDate.GetDay() == dailySet.m_ObsDate.GetDay()
						&& hourlySet.m_DailyObs != 1)
					{
						if (hourlySet.IsFieldNull(&hourlySet.m_SnowFlag))
							nUpdated++;
						else if (hourlySet.m_SnowFlag != dailySnowFlag)
						{
							nChanged++;
							nUpdated++;
						}
						hourlySet.Edit();
						hourlySet.m_SnowFlag = dailySnowFlag;
						hourlySet.Update();
					}
					hourlySet.MoveNext();
				}
				dailySet.MoveNext();
			}
			//bOK = m_pDoc->m_pDB->CommitTrans();
			//if (!bOK)
			//{
			//	AfxMessageBox("Failure committing transaction.");
			//}
			dailySet.Close();
			hourlySet.Close();
			if (nChanged > 0 || nUpdated > 0)
			{
				CWxSet allSet(m_pDoc->m_pDB);
				allSet.m_strSort = _T("[ObsDate]");
				allSet.m_strFilter.Format("[StationID] = '%6.6s'", staStr);
				allSet.Open();
				while (!allSet.IsEOF())
				{
					allSet.Edit();
					allSet.SetFieldNull(&allSet.m_FM1);
					allSet.SetFieldNull(&allSet.m_FM10);
					allSet.SetFieldNull(&allSet.m_FM100);
					allSet.SetFieldNull(&allSet.m_FM1000);
					allSet.SetFieldNull(&allSet.m_FuelTemperature);
					allSet.Update();
					allSet.MoveNext();
				}
				allSet.Close();
			}
		//}
		//else
		//{
		//	AfxMessageBox("Failure in call to BeginTrans().");
		//}
		if(nChanged || nUpdated > 0)
			msg.Format("Updated %d SnowFlag values for RAWS records for station %s.  All stored calculated Dead Fuel Moistures and Fuel Temperatures for station %s have been cleared and set to NULL.", nUpdated, staStr, staStr);
		else
			msg.Format("No records updated or changed for station %s.", staStr);
		AfxMessageBox(msg);
	}
}


BOOL CFillSnowflagDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CSIGStationSet staSet(m_pDoc->m_pDB);
	staSet.Open();
	CWxStationSet usedStations(m_pDoc->m_pDB);
	usedStations.Open(CRecordset::dynaset, "SELECT DISTINCT WXObs.StationID FROM WXObs   ORDER BY WXObs.StationID;");

	if (usedStations.IsOpen())
	{
		while (!usedStations.IsEOF())
		{
			CString str = usedStations.m_StationID;
			str.TrimRight();
			CString sta;

			staSet.m_strFilter.Format("[StationID] = '%6.6s'", str);
			staSet.Requery();
			str += " - " + staSet.m_Name;
			int loc = m_comboStation.AddString(str);
			usedStations.MoveNext();
		}
	}
	usedStations.Close();
	staSet.Close();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

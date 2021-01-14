// DailyObsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "DailyObsDialog.h"
#include "afxdialogex.h"
#include "WxStationSet.h"
#include "SIGStationSet.h"
#include "WxSet.h"
#include "FireplusDoc.h"
#include "FireplusSet.h"


// CDailyObsDialog dialog

IMPLEMENT_DYNAMIC(CDailyObsDialog, CDialogEx)

CDailyObsDialog::CDailyObsDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_SET_DAILY_OBS, pParent)
{
	m_pDoc = NULL;
}

CDailyObsDialog::~CDailyObsDialog()
{
}

void CDailyObsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_STATION, m_comboStation);
	DDX_Control(pDX, IDC_COMBO_OBS_HOUR, m_comboObsHour);
}


BEGIN_MESSAGE_MAP(CDailyObsDialog, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_STATION, &CDailyObsDialog::OnCbnSelchangeComboStation)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CDailyObsDialog::OnBnClickedButtonApply)
END_MESSAGE_MAP()


// CDailyObsDialog message handlers


void CDailyObsDialog::OnCbnSelchangeComboStation()
{
	CString staStr;
	m_comboStation.GetWindowTextA(staStr);
	if (staStr.GetLength() > 0)
	{
		CSIGStationSet staSet(m_pDoc->m_pDB);
		staSet.m_strFilter.Format("[StationID] = '%6.6s'", staStr);
		staSet.Open();
		CString obsStr;
		obsStr.Format("%d", staSet.m_RegSchdObs);
		m_comboObsHour.SelectString(-1, obsStr);
		staSet.Close();
	}
}

void CDailyObsDialog::OnBnClickedButtonApply()
{
	CString staStr;
	CWaitCursor wait;
	m_comboStation.GetWindowTextA(staStr);
	CSIGStationSet staSet(m_pDoc->m_pDB);
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", staStr);
	staSet.Open();
	CString hrStr;
	m_comboObsHour.GetWindowTextA(hrStr);
	int hr = atoi(hrStr);
	if (staSet.m_RegSchdObs != hr)
	{
		staSet.Edit();
		staSet.m_RegSchdObs = hr;
		staSet.Update();
	}
	CWxSet wxSet(m_pDoc->m_pDB);
	wxSet.m_strFilter.Format("[StationID] = '%6.6s'", staStr);
	wxSet.Open();
	wxSet.setDailyObs(staSet.m_RegSchdObs);
	wxSet.Close();
	staSet.Close();
	CString msg;
	msg.Format("Set DailyObs complete for station %s", staStr);
	AfxMessageBox(msg);
}

BOOL CDailyObsDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	if (m_pDoc->m_pDB)
	{
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
				//m_comboStation.SetItemData(loc, 0);
				usedStations.MoveNext();
				//loc++;
			}
		}
		usedStations.Close();
		CString staStr;
		m_comboStation.GetWindowTextA(staStr);
		if (staStr.GetLength() > 0)
		{
			staSet.m_strFilter.Format("[StationID] = '%6.6s'", staStr);
			staSet.Requery();
			CString obsStr;
			obsStr.Format("%d", staSet.m_RegSchdObs);
			m_comboObsHour.SelectString(-1, obsStr);
		}
		staSet.Close();
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
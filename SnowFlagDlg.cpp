// SnowFlagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "SnowFlagDlg.h"
#include "afxdialogex.h"
#include "WxStationSet.h"
#include "SIGStationSet.h"
#include "WxSet.h"
#include "FireplusDoc.h"
#include "FireplusSet.h"

// CSnowFlagDlg dialog
extern bool IsLeap(int year);

IMPLEMENT_DYNAMIC(CSnowFlagDlg, CDialogEx)

CSnowFlagDlg::CSnowFlagDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SNOWFLAG_DIALOG, pParent)
{
	//m_pDB = NULL;
	m_pDoc = NULL;
}

CSnowFlagDlg::~CSnowFlagDlg()
{
}

void CSnowFlagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_STATION, m_comboStation);
	DDX_Control(pDX, IDC_COMBO_SNOWFLAG, m_comboValue);
	DDX_Control(pDX, IDC_SYEAR, m_editStartYear);
	DDX_Control(pDX, IDC_SPIN_SYEAR, m_spinStartYear);
	DDX_Control(pDX, IDC_EYEAR, m_editEndYear);
	DDX_Control(pDX, IDC_SPIN_EYEAR, m_spinEndYear);
	DDX_Control(pDX, IDC_SMONTH, m_comboStartMonth);
	DDX_Control(pDX, IDC_EMONTH, m_comboEndMonth);
	DDX_Control(pDX, IDC_SDAY, m_editStartDay);
	DDX_Control(pDX, IDC_SPIN_SDAY, m_spinStartDay);
	DDX_Control(pDX, IDC_EDAY, m_editEndDay);
	DDX_Control(pDX, IDC_SPIN_EDAY, m_spinEndDay);
}


BEGIN_MESSAGE_MAP(CSnowFlagDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_STATION, &CSnowFlagDlg::OnCbnSelchangeComboStation)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CSnowFlagDlg::OnBnClickedButtonApply)
	ON_CBN_SELCHANGE(IDC_SMONTH, &CSnowFlagDlg::OnCbnSelchangeSmonth)
	ON_CBN_SELCHANGE(IDC_EMONTH, &CSnowFlagDlg::OnCbnSelchangeEmonth)
END_MESSAGE_MAP()


// CSnowFlagDlg message handlers


void CSnowFlagDlg::OnCbnSelchangeComboStation()
{
	CString staStr;
	m_comboStation.GetWindowTextA(staStr);
	SetYears(staStr);
}


void CSnowFlagDlg::OnBnClickedButtonApply()
{
	//build confirmation string
	CString msg, sfStr, staStr, m1Str, m2Str;
	int d1, d2, yr1, yr2, snowFlagVal;
	m_comboValue.GetWindowTextA(sfStr);
	m_comboStation.GetWindowTextA(staStr);
	yr1 = m_spinStartYear.GetPos();
	yr2 = m_spinEndYear.GetPos();
	m_comboStartMonth.GetWindowTextA(m1Str);
	m_comboEndMonth.GetWindowTextA(m2Str);
	d1 = m_spinStartDay.GetPos();
	d2 = m_spinEndDay.GetPos();
	snowFlagVal = atoi(sfStr);
	if (staStr.GetLength() <= 0)
		return;
	msg.Format("Set SnowFlag = %d for Station %s for Years %d through %d, from %s %d through %s %d?",
		snowFlagVal, staStr, yr1, yr2, m1Str, d1, m2Str, d2);
	if (IDYES == AfxMessageBox(msg, MB_YESNO))
	{
		CWaitCursor wait;
		COleDateTime n2Start, n2End;
		n2Start.SetDateTime(yr1, MonthChtoInt(m1Str), d1, 0, 0, 0);
		n2End.SetDateTime(yr2, MonthChtoInt(m2Str), d2, 0, 0, 0);

		CString baseQuery, query;
		BuildQuery(baseQuery);

		CWxSet wxSet(m_pDoc->m_pDB);
		//wxSet.m_strFilter.Format("[StationID] = '%6.6s' AND [ObsDate] >= #%s# AND [ObsDate] <= #%s#", staStr,
		//	n2Start.Format(), n2End.Format());
		wxSet.m_strFilter = baseQuery;
		wxSet.m_strSort = "[ObsDate]";
		wxSet.Open();
		if (wxSet.IsBOF() || wxSet.IsEOF())
		{
			CString strMsg;
			strMsg.Format(
				_T("ERROR: No matching weather observation records.\n Redefine Working Set Query. \n\n"));
			AfxMessageBox(strMsg);
			return;
		}
		wxSet.m_pDatabase->BeginTrans();
		int count = 0;
		while (!wxSet.IsEOF())
		{
			if (wxSet.IsFieldNull(&wxSet.m_SnowFlag) || wxSet.m_SnowFlag != snowFlagVal)
			{
				wxSet.Edit();
				wxSet.m_SnowFlag = snowFlagVal;
				wxSet.Update();
				count++;
			}
			wxSet.MoveNext();
		}
		wxSet.m_pDatabase->CommitTrans();
		wxSet.m_strFilter.Format("[StationID] = '%6.6s'", staStr);
		wxSet.Requery();
		while (!wxSet.IsEOF())
		{
			wxSet.Edit();
			wxSet.SetFieldNull(&wxSet.m_FM1);
			wxSet.SetFieldNull(&wxSet.m_FM10);
			wxSet.SetFieldNull(&wxSet.m_FM100);
			wxSet.SetFieldNull(&wxSet.m_FM1000);
			wxSet.SetFieldNull(&wxSet.m_FuelTemperature);
			wxSet.Update();
			wxSet.MoveNext();
		}
		wxSet.Close();
		msg.Format("Updated SnowFlag to %d for %d records. All stored calculated Dead Fuel Moistures and Fuel Temperatures for station %s have been cleared and set to NULL.", snowFlagVal, count, staStr);
		AfxMessageBox(msg);
	}
}

int DaysInMonth(int month)
{
	int days = 31;
	switch (month)
	{
	case 9:
	case 4:
	case 6:
	case 11:
		days = 30;
		break;
	case 2:
		days = 28;
	}
	return days;
}

void CSnowFlagDlg::BuildQuery(CString &query)
{
	CString sfStr, staStr, m1Str, m2Str, temp;
	int day1, day2, yr1, yr2;
	m_comboValue.GetWindowTextA(sfStr);
	m_comboStation.GetWindowTextA(staStr);
	yr1 = m_spinStartYear.GetPos();
	yr2 = m_spinEndYear.GetPos();
	m_comboStartMonth.GetWindowTextA(m1Str);
	m_comboEndMonth.GetWindowTextA(m2Str);
	day1 = m_spinStartDay.GetPos();
	day2 = m_spinEndDay.GetPos();
	COleDateTime d1, d2;
	//if (MonthChtoInt(m1Str) == 2 && day1 >= 29)
	//	day1 = 28;
	//if (MonthChtoInt(m2Str) == 2 && day2 >= 29)
	//	day2 = 28;
	if (day1 > DaysInMonth(MonthChtoInt(m1Str)))
		day1 = DaysInMonth(MonthChtoInt(m1Str));
	if (day2 > DaysInMonth(MonthChtoInt(m2Str)))
		day2 = DaysInMonth(MonthChtoInt(m2Str));
	if (day1 <= 0)
		day1 = 1;
	if (day2 <= 0)
		day2 = 1;
	d1.SetDate(BASISYEAR, MonthChtoInt(m1Str), day1);
	d2.SetDate(BASISYEAR, MonthChtoInt(m2Str), day2);
	int dOrder = DOY_NORMAL;
	if (d1 == d2)// || d2 + day == d1)
		dOrder = DOY_EQUAL;
	else if (d1 > d2)
		dOrder = DOY_FLIPPED;
	query.Format("(Year([ObsDate]) >= %d and Year([ObsDate]) <= %d)",
		yr1, yr2);

	switch (dOrder)
	{
	case DOY_FLIPPED:
	{
		//END DOY EARLIER THAN START DOY
		temp.Format(" and ((Month([ObsDate]) > %d or Month([ObsDate]) < %d)",
			d1.GetMonth(), d2.GetMonth());
		query += temp;
		if (d1.GetMonth() == d2.GetMonth())
		{
			temp.Format(" or (Month([ObsDate]) = %d and Day([ObsDate]) >= %d"
				" or Day([ObsDate]) <= %d))",
				d1.GetMonth(), day1, day2);
		}
		else
		{
			temp.Format(
				" or (Month([ObsDate]) = %d and Day([ObsDate]) >= %d)"
				" or (Month([ObsDate]) = %d and Day([ObsDate]) <= %d))",
				d1.GetMonth(), day1,
				d2.GetMonth(), day2);
		}
		query += temp;
	}
	break;
	case DOY_EQUAL:
	default: //DOY NORMAL
		if (d1.GetMonth() == d2.GetMonth())
		{
			temp.Format(" and Month([ObsDate]) = %d and Day([ObsDate]) >= %d"
				" and Day([ObsDate]) <= %d",
				d1.GetMonth(), day1, day2);
		}
		else
		{
			temp.Format(" and ((Month([ObsDate]) > %d and Month([ObsDate]) < %d)",
				d1.GetMonth(), d2.GetMonth());
			query += temp;
			temp.Format(
				" or (Month([ObsDate]) = %d and Day([ObsDate]) >= %d)"
				" or (Month([ObsDate]) = %d and Day([ObsDate]) <= %d))",
				d1.GetMonth(), day1,
				d2.GetMonth(), day2);
		}
		query += temp;
	}
	temp.Format(" and [StationID] = '%6.6s'", staStr);
	query += temp;
}

void CSnowFlagDlg::OnCbnSelchangeSmonth()
{
	SetStartDayRange();
}


void CSnowFlagDlg::OnCbnSelchangeEmonth()
{
	SetEndDayRange();
}


BOOL CSnowFlagDlg::OnInitDialog()
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
		staSet.Close();
		CString staStr;
		m_comboStation.GetWindowTextA(staStr);
		if (staStr.GetLength() > 0)
		{
			SetYears(staStr);
		}
		m_comboStartMonth.SetCurSel(0);
		m_comboEndMonth.SetCurSel(11);
		SetStartDayRange();
		m_spinStartDay.SetPos(1);
		SetEndDayRange();
		m_spinEndDay.SetPos(31);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CSnowFlagDlg::SetStartDayRange()
{
	CString monthStr;
	m_comboStartMonth.GetWindowTextA(monthStr);
	int month = MonthChtoInt(monthStr);
	int days = DaysInMonth(month);
	if (m_spinStartDay.GetPos() > days)
		m_spinStartDay.SetPos(days);
	m_spinStartDay.SetRange(1, days);
}

void CSnowFlagDlg::SetEndDayRange()
{
	CString monthStr;
	m_comboEndMonth.GetWindowTextA(monthStr);
	int month = MonthChtoInt(monthStr);
	int days = DaysInMonth(month);
	if (m_spinEndDay.GetPos() > days)
		m_spinEndDay.SetPos(days);
	m_spinEndDay.SetRange(1, days);
}

void CSnowFlagDlg::SetYears(CString stationStr)
{
	int yr1, yr2;
	CWxSet wxSet(m_pDoc->m_pDB);
	CString temp;
	temp.Format( "[StationID] = '%6.6s'", stationStr);
	wxSet.m_strFilter = _T(temp);
	wxSet.m_strSort = "[ObsDate]";
	wxSet.Open();
	wxSet.MoveFirst();
	yr1 = wxSet.m_ObsDate.GetYear();
	wxSet.MoveLast();
	yr2 = wxSet.m_ObsDate.GetYear();
	m_spinStartYear.SetRange(yr1, yr2);
	m_spinEndYear.SetRange(yr1, yr2);
	m_spinStartYear.SetPos(yr1);
	m_spinEndYear.SetPos(yr2);
	wxSet.Close();
}


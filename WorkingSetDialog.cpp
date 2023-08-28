// WorkingSetDialog.cpp : implementation file
//
#include "stdafx.h"
#include "fireplus.h"
#include "WorkingSetDialog.h"
#include "fireplusSet.h"
#include ".\workingsetdialog.h"
#include "BatchOutputDoc.h"
#include "SIGStationSet.h"
#include "stationInSIGSet.h"
#include "WxSet.h"

extern CFireplusApp theApp;
// CWorkingSetDialog dialog
const CString wsd_strFModels = "A - Western Annual Grasses\nB - California Chaparral\nC - Pine-Grass Savanna\n"
			"D - Southern Rough\nE - Hardwood Litter (Winter)\nF - Intermediate Brush\n"
			"G - Short-Needle (Heavy Dead)\nH - Short-Needle (Normal Dead)\nI - Heavy Slash\n"
			"J - Intermediate Slash\nK - Light Slash\nL - Western Perennial Grasses\nN - Sawgrass\n"
			"O - High Pocosin\nP - Southern Pine Plantation\nQ - Alaskan Black Spruce\n"
			"R - Hardwood Litter (Summer)\nS - Tundra\nT - Sagebrush-Grass\nU - Western Pines\n"
			"V - Grass\nW - Grass-Shrub\nX - Brush\nY - Timber\nZ - Slash/Blowdown\n"
	;

IMPLEMENT_DYNAMIC(CWorkingSetDialog, CDialog)
CWorkingSetDialog::CWorkingSetDialog(CWnd* pParent /*=NULL*/, CRawClim *_m_pClim /*= NULL*/)
	: CDialog(CWorkingSetDialog::IDD, pParent)
{
	m_pClim = _m_pClim;
	tableBottomMin = 0;
	tableRightMin = 0;
}

CWorkingSetDialog::~CWorkingSetDialog()
{
}

void CWorkingSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATAYEARS, dataYearsBtn);
	DDX_Control(pDX, IDC_SYEAR, m_StartYear);
	DDX_Control(pDX, IDC_SMONTH, m_StartMonth);
	DDX_Control(pDX, IDC_SDAY, m_StartDay);
	DDX_Control(pDX, IDC_EYEAR, m_EndYear);
	DDX_Control(pDX, IDC_EDAY, m_EndDay);
	DDX_Control(pDX, IDC_EMONTH, m_EndMonth);
	DDX_Control(pDX, IDC_COMBO1, m_PeriodLenControl);
	DDX_Control(pDX, IDC_SPIN4, m_spnEday);
	DDX_Control(pDX, IDC_SPIN3, m_spnSday);
	DDX_Control(pDX, IDC_SPIN2, m_spnEyear);
	DDX_Control(pDX, IDC_SPIN1, m_spnSyear);
	DDX_Text(pDX, IDC_SYEAR, m_sYear);
	DDX_Text(pDX, IDC_EYEAR, m_eYear);
	DDX_Text(pDX, IDC_SDAY, m_sDay);
	DDX_Text(pDX, IDC_EDAY, m_eDay);
	DDX_CBString(pDX, IDC_SMONTH, m_sMonth);
	DDX_CBString(pDX, IDC_EMONTH, m_eMonth);
	DDX_CBString(pDX, IDC_COMBO1, m_period);
	DDX_Check(pDX, IDC_CHECK_FORCE_RECOMPUTE, theApp.m_ForceNFDRS2016Recompute);
	DDX_Control(pDX, IDC_STATIC_TABLELABEL, m_staticTableLabel);
	DDX_Control(pDX, IDC_CHECK_DAILY_EXTREMES, m_chkDailyExtremes);
	DDX_Control(pDX, IDC_CHECK_FORCE_RECOMPUTE, m_chkForceRecompute);
}


BEGIN_MESSAGE_MAP(CWorkingSetDialog, CDialog)
	ON_CBN_SELCHANGE(IDC_SMONTH, &CWorkingSetDialog::OnCbnSelchangeSmonth)
	ON_CBN_SELCHANGE(IDC_EMONTH, &CWorkingSetDialog::OnCbnSelchangeEmonth)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CHECK_DAILY_EXTREMES, &CWorkingSetDialog::OnBnClickedCheckDailyExtremes)
END_MESSAGE_MAP()


// CWorkingSetDialog message handlers

BOOL CWorkingSetDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_sYear = m_pClim->m_fpSet->m_StartYear;
	m_eYear = m_pClim->m_fpSet->m_EndYear;
	m_sDay = m_pClim->m_fpSet->m_StartDay;
	m_eDay = m_pClim->m_fpSet->m_EndDay;
	m_sMonth = m_pClim->m_fpSet->m_StartMonth;
	m_eMonth = m_pClim->m_fpSet->m_EndMonth;
	m_period = m_pClim->m_fpSet->m_PeriodLength;

	m_spnSyear.SetRange(1900, 1998);
	m_spnEyear.SetRange(1900, 1998);
	SetDayNumControlRange(&m_spnSday, MonthChtoInt(m_sMonth));
	SetDayNumControlRange(&m_spnEday, MonthChtoInt(m_eMonth));
	int yr1 = m_pClim->m_baseYear, yr2 = m_pClim->m_baseYear + m_pClim->m_nYears - 1;
	//set years for spinners
	m_spnSyear.SetRange(yr1, yr2);
	m_spnEyear.SetRange(yr1, yr2);
	CString gStr;
	gStr.Format("Data Years (%d - %d)", yr1, yr2);
	dataYearsBtn.SetWindowText(gStr);
	m_grid.AttachGrid(this, IDC_GRID);
	m_grid.m_pDB = m_pClim->m_fpSet->m_pDatabase;
	CRect tRect;
	m_grid.GetClientRect(&tRect);
	tableBottomMin = tRect.bottom;
	tableRightMin = tRect.right;
	fxParamChange = false;
	UpdateData(FALSE);
	PopulateMetaTable();
	SetYears();
	ConfigCheckBoxes();
	m_grid.SetVisibleColumns();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CWorkingSetDialog::OnOK()
{
	if(!UpdateData())
		return;
	//some error checking on spinner/text fields (user can enter hooey)
	if(m_sYear < m_pClim->m_baseYear)
	{
		CString msg;
		msg.Format("Error: Start Year %d less than raw data start year of %d", m_sYear, m_pClim->m_baseYear);
		AfxMessageBox(msg);
		//UpdateData(FALSE);
		return;
	}
	if(m_eYear < m_pClim->m_baseYear)
	{
		CString msg;
		msg.Format("Error: End Year %d less than raw data start year of %d", m_eYear, m_pClim->m_baseYear);
		AfxMessageBox(msg);
		//UpdateData(FALSE);
		return;
	}
	if(m_sYear > m_pClim->m_baseYear + m_pClim->m_nYears - 1)
	{
		CString msg;
		msg.Format("Error: Start Year %d greater than raw data end year of %d", m_sYear, m_pClim->m_baseYear + m_pClim->m_nYears - 1);
		AfxMessageBox(msg);
		//UpdateData(FALSE);
		return;
	}
	if(m_eYear > m_pClim->m_baseYear + m_pClim->m_nYears - 1)
	{
		CString msg;
		msg.Format("Error: End Year %d greater than raw data end year of %d", m_eYear, m_pClim->m_baseYear + m_pClim->m_nYears - 1);
		AfxMessageBox(msg);
		//UpdateData(FALSE);
		return;
	}
	if(m_sDay < 1)
	{
		CString msg;
		msg.Format("Error: Start Day must be greater than zero");
		AfxMessageBox(msg);
		//UpdateData(FALSE);
		return;
	}
	if(m_sDay > 31)
	{
		CString msg;
		msg.Format("Error: Start Day must be less than or equal to 31");
		AfxMessageBox(msg);
		//UpdateData(FALSE);
		return;
	}
	if(m_eDay < 1)
	{
		CString msg;
		msg.Format("Error: End Day must be greater than zero");
		AfxMessageBox(msg);
		//UpdateData(FALSE);
		return;
	}
	if(m_eDay > 31)
	{
		CString msg;
		msg.Format("Error: End Day must be less than or equal to 31");
		AfxMessageBox(msg);
		//UpdateData(FALSE);
		return;
	}
	SaveMetaTable();
	//basic check.. if everything is the same then cancel
	if(m_sYear == m_pClim->m_fpSet->m_StartYear
		&& m_eYear == m_pClim->m_fpSet->m_EndYear
		&& 	m_sDay == m_pClim->m_fpSet->m_StartDay
		&& 	m_eDay == m_pClim->m_fpSet->m_EndDay
		&& 	m_sMonth == m_pClim->m_fpSet->m_StartMonth
		&& 	m_eMonth == m_pClim->m_fpSet->m_EndMonth
		&& 	m_period == m_pClim->m_fpSet->m_PeriodLength
		&& !fxParamChange && !theApp.m_ForceNFDRS2016Recompute)
	{
		CDialog::OnCancel();
		return;
	}
	m_pClim->m_fpSet->Edit();
	m_pClim->m_fpSet->m_StartYear = m_sYear;
	m_pClim->m_fpSet->m_EndYear = m_eYear;
	m_pClim->m_fpSet->m_StartDay = m_sDay;
	m_pClim->m_fpSet->m_EndDay = m_eDay;
	m_pClim->m_fpSet->m_StartMonth = m_sMonth;
	m_pClim->m_fpSet->m_EndMonth = m_eMonth;
	m_pClim->m_fpSet->m_PeriodLength = m_period;
	if(m_pClim->m_fpSet->m_StartDay > 28)
	{
		switch(MonthChtoInt(m_pClim->m_fpSet->m_StartMonth))
		{
		case 4:
		case 6:
		case 9:
		case 11:
			if(m_pClim->m_fpSet->m_StartDay == 31)
			{
				m_pClim->m_fpSet->m_StartDay = 30;
			}
			break;
		case 2:
			{
				m_pClim->m_fpSet->m_StartDay = 28;
			}
			break;
		default://up to 31 is OK
			break;
		}
	}
	if(m_pClim->m_fpSet->m_EndDay > 28)
	{
		switch(MonthChtoInt(m_pClim->m_fpSet->m_EndMonth))
		{
		case 4:
		case 6:
		case 9:
		case 11:
			if(m_pClim->m_fpSet->m_EndDay == 31)
			{
				m_pClim->m_fpSet->m_EndDay = 30;
			}
			break;
		case 2:
			{
				m_pClim->m_fpSet->m_EndDay = 28;
			}
			break;
		default://up to 31 is OK
			break;
		}
	}
	m_pClim->m_fpSet->Update();
	m_pClim->m_fpSet->MoveLast();
	UpdateData(FALSE);
	//m_pClim->m_fpSet->Update();
	if(fxParamChange)
	{//everything needs to be recalculated...
	}
	CDialog::OnOK();
}

void CWorkingSetDialog::OnCbnSelchangeSmonth()
{
	UpdateData();
	SetDayNumControlRange(&m_spnSday, MonthChtoInt(m_sMonth));
	int low, hi;
	m_spnSday.GetRange(low, hi);
	if(m_sDay > hi)
		m_sDay = hi;
	UpdateData(FALSE);
}

void CWorkingSetDialog::OnCbnSelchangeEmonth()
{
	UpdateData();
	SetDayNumControlRange(&m_spnEday, MonthChtoInt(m_eMonth));
	int low, hi;
	m_spnEday.GetRange(low, hi);
	if(m_eDay > hi)
		m_eDay = hi;
	UpdateData(FALSE);
}

void CWorkingSetDialog::SetDayNumControlRange(CSpinButtonCtrl *pSpinCtrl, int month)
{
	int maxDays = 31;
	switch(month)
	{
	case 2://Feb
		maxDays = 28;
		break;
	case 9:
	case 4:
	case 6:
	case 11:
		maxDays = 30;
		break;
	default:
		maxDays = 31;
		break;
	}
	if(pSpinCtrl->GetPos() > maxDays)
		pSpinCtrl->SetPos(maxDays);
	pSpinCtrl->SetRange(1, maxDays);
}

void CWorkingSetDialog::PopulateMetaTable()
{
	CUGCell cell;
	CSIGStationSet ssSet(m_pClim->m_pDB);
	ssSet.Open();
	m_grid.SetNumberRows(0);
		//set up string arrays for combobox columns...
	char temp[64], station[64];
	strcpy_s(station, m_pClim->m_sigStaID);
	if(strncmp(station, "SIG", 3) == 0)//is a SIG
	{
			CStationInSIGSet sSet(m_pClim->m_pDB);
			sprintf(temp, "[SIG] = '%-20.20s'", &station[6]);
			sSet.m_strFilter = _T(temp);
			sSet.Open();
			int row = 0;
			while(!sSet.IsEOF())
			{
				sprintf(temp, "[StationID] = '%6.6s'", sSet.m_StationID);
				ssSet.m_strFilter = _T(temp);
				ssSet.Requery();
				if(!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
				{
					if(ssSet.m_GreenJulian.m_status != COleDateTime::valid)
					{
						ssSet.Edit();
						ssSet.m_GreenJulian.SetDateTime(2008, 1, 1, 13, 0, 0);
						ssSet.Update();
					}
					if(ssSet.m_DormantJulian.m_status != COleDateTime::valid)
					{
						ssSet.Edit();
						ssSet.m_DormantJulian.SetDateTime(2008, 9, 15, 13, 0, 0);
						ssSet.Update();
					}
					if(ssSet.m_FreezeJulian.m_status != COleDateTime::valid)
					{
						ssSet.Edit();
						ssSet.m_FreezeJulian.SetDateTime(2008, 12, 31, 13, 0, 0);
						ssSet.Update();
					}
					m_grid.AppendRow();
					m_grid.QuickSetText(0, row, ssSet.m_StationID);
					m_grid.QuickSetText(1, row, ssSet.m_Name);
					m_grid.GetCell(2, row, &cell);
					if(ssSet.IsFieldNull(&ssSet.m_NFDRSFM))
					{
						strcpy_s(temp, " ");
					}
					else
						sprintf(temp, "%c - ", ssSet.m_NFDRSFM[0]);
					if(temp[0] != ' ')
					{
						CString list = wsd_strFModels;
						char *pStr = (char *)strstr(list, temp);
						int l = 0;
						while(pStr[l] != '\t' && pStr[l] != '\n' && pStr[l] != 0)
							temp[l] = pStr[l++];
						temp[l] = 0;
					}
					cell.SetText(temp);
					m_grid.SetCell(2, row, &cell);
					m_grid.GetCell(3, row, &cell);
					cell.SetNumber(ssSet.m_Use88);
					m_grid.SetCell(3, row, &cell);
					CString tmp;
					tmp.Format("%d", ssSet.m_SlopeCls);
					m_grid.QuickSetText(4, row, tmp);
					tmp.Format("%d", ssSet.m_ClimateCls);
					m_grid.QuickSetText(5, row, tmp);
					m_grid.GetCell(6, row, &cell);
					tmp = ssSet.m_GreenJulian.Format("%m/%d");
					cell.SetText(tmp);
					m_grid.SetCell(6, row, &cell);
					m_grid.GetCell(7, row, &cell);
					tmp = ssSet.m_FreezeJulian.Format("%m/%d");
					cell.SetText(tmp);
					m_grid.SetCell(7, row, &cell);
					m_grid.QuickSetNumber(8, row, ssSet.m_StartKBDI);
					m_grid.QuickSetNumber(9, row, ssSet.m_Start1000);
					m_grid.QuickSetNumber(10, row, ssSet.m_AvgPrecip);
					m_grid.QuickSetNumber(11, row, ssSet.m_FM1Eq10);
					m_grid.QuickSetNumber(12, row, ssSet.m_HerbAnnual);
					m_grid.QuickSetNumber(13, row, ssSet.m_Deciduous);
					tmp = ssSet.m_Aspect;
					m_grid.QuickSetText(14, row, tmp);
					tmp = ssSet.m_PsnOnSlope;
					m_grid.QuickSetText(15, row, tmp);
					m_grid.QuickSetNumber(16, row, ssSet.m_Elevation);
					m_grid.QuickSetNumber(17, row, ssSet.m_LatDegrees);
					m_grid.GetCell(18, row, &cell);
					tmp = ssSet.m_DormantJulian.Format("%m/%d");
					cell.SetText(tmp);
					m_grid.SetCell(18, row, &cell);

					m_grid.GetCell(19, row, &cell);
					cell.SetNumber(ssSet.m_UseDormant);
					m_grid.SetCell(19, row, &cell);

					m_grid.GetCell(20, row, &cell);
					cell.SetNumber(ssSet.m_UseStick);
					m_grid.SetCell(20, row, &cell);
						//if it's an NFDR2016 Fuel set SCM and MX_Humid
					if (isNFDRS2016(ssSet.m_NFDRSFM[0]))
					{
						int scmVal;// , mxVal;
						switch (ssSet.m_NFDRSFM[0])
						{
						case 'v':
						case 'V':
							scmVal = ssSet.m_SCM_V;
							//mxVal = ssSet.m_MX_Humid_V;
							break;
						case 'w':
						case 'W':
							scmVal = ssSet.m_SCM_W;
							//mxVal = ssSet.m_MX_Humid_W;
							break;
						case 'x':
						case 'X':
							scmVal = ssSet.m_SCM_X;
							//mxVal = ssSet.m_MX_Humid_X;
							break;
						case 'y':
						case 'Y':
							scmVal = ssSet.m_SCM_Y;
							//mxVal = ssSet.m_MX_Humid_Y;
							break;
						case 'z':
						case 'Z':
							scmVal = ssSet.m_SCM_Z;
							//mxVal = ssSet.m_MX_Humid_Z;
							break;
						}
						m_grid.QuickSetNumber(21, row, scmVal);
						//m_grid.QuickSetNumber(22, row, mxVal);
					}
					if (!ssSet.IsFieldNull(&ssSet.m_MXD_Override))
						m_grid.QuickSetNumber(22, row, ssSet.m_MXD_Override);
					else
						m_grid.QuickSetText(22, row, "");
				}
				//check for hourly data
				sSet.MoveNext();
				row++;
			}
			sSet.Close();
	}
	else//single station
	{
			sprintf(temp, "[StationID] = '%6.6s'", station);
			ssSet.m_strFilter = _T(temp);
			ssSet.Requery();
			if(!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
			{
				if(ssSet.m_GreenJulian.m_status != COleDateTime::valid)
				{
					ssSet.Edit();
					ssSet.m_GreenJulian.SetDateTime(2008, 1, 1, 13, 0, 0);
					ssSet.Update();
				}
				if(ssSet.m_FreezeJulian.m_status != COleDateTime::valid)
				{
					ssSet.Edit();
					ssSet.m_FreezeJulian.SetDateTime(2008, 12, 31, 13, 0, 0);
					ssSet.Update();
				}
				if(ssSet.m_DormantJulian.m_status != COleDateTime::valid)
				{
					ssSet.Edit();
					ssSet.m_DormantJulian.SetDateTime(2008, 12, 31, 13, 0, 0);
					ssSet.Update();
				}
				int row = 0;
				m_grid.AppendRow();
				m_grid.QuickSetText(0, row, ssSet.m_StationID);
				m_grid.QuickSetText(1, row, ssSet.m_Name);
				m_grid.GetCell(2, row, &cell);
				if(ssSet.IsFieldNull(&ssSet.m_NFDRSFM))
					strcpy_s(temp, " ");
				else
					sprintf(temp, "%c - ", ssSet.m_NFDRSFM[0]);
				if(temp[0] != ' ')
				{
					CString list = wsd_strFModels;//cell.GetLabelText();//m_metaTable.GetColumnSet().GetItem(2).GetList();
					char *pStr = (char *)strstr(list, temp);
					int l = 0;
					while(pStr[l] != '\t' && pStr[l] != '\n' && pStr[l] != 0)
						temp[l] = pStr[l++];
					temp[l] = 0;
				}
				cell.SetText(temp);
				m_grid.SetCell(2, row, &cell);
				m_grid.GetCell(3, row, &cell);
				cell.SetNumber(ssSet.m_Use88);
				m_grid.SetCell(3, row, &cell);
				CString tmp;
				tmp.Format("%d", ssSet.m_SlopeCls);
				m_grid.QuickSetText(4, row, tmp);
				tmp.Format("%d", ssSet.m_ClimateCls);
				m_grid.QuickSetText(5, row, tmp);
				m_grid.GetCell(6, row, &cell);
				tmp = ssSet.m_GreenJulian.Format("%m/%d");
				cell.SetText(tmp);
				m_grid.SetCell(6, row, &cell);
				m_grid.GetCell(7, row, &cell);
				tmp = ssSet.m_FreezeJulian.Format("%m/%d");
				cell.SetText(tmp);
				m_grid.SetCell(7, row, &cell);
				m_grid.QuickSetNumber(8, row, ssSet.m_StartKBDI);
				m_grid.QuickSetNumber(9, row, ssSet.m_Start1000);
				m_grid.QuickSetNumber(10, row, ssSet.m_AvgPrecip);
				m_grid.QuickSetNumber(11, row, ssSet.m_FM1Eq10);
				m_grid.QuickSetNumber(12, row, ssSet.m_HerbAnnual);
				m_grid.QuickSetNumber(13, row, ssSet.m_Deciduous);
				tmp = ssSet.m_Aspect;
				m_grid.QuickSetText(14, row, tmp);
				tmp = ssSet.m_PsnOnSlope;
				m_grid.QuickSetText(15, row, tmp);
				m_grid.QuickSetNumber(16, row, ssSet.m_Elevation);
				m_grid.QuickSetNumber(17, row, ssSet.m_LatDegrees);
				m_grid.GetCell(18, row, &cell);
				tmp = ssSet.m_DormantJulian.Format("%m/%d");
				cell.SetText(tmp);
				m_grid.SetCell(18, row, &cell);

				m_grid.GetCell(19, row, &cell);
				cell.SetNumber(ssSet.m_UseDormant);
				m_grid.SetCell(19, row, &cell);

				m_grid.GetCell(20, row, &cell);
				cell.SetNumber(ssSet.m_UseStick);
				m_grid.SetCell(20, row, &cell);
				//if it's an NFDR2016 Fuel set SCM and MX_Humid
				if (isNFDRS2016(ssSet.m_NFDRSFM[0]))
				{
					int scmVal;// , mxVal;
					switch (ssSet.m_NFDRSFM[0])
					{
					case 'v':
					case 'V':
						scmVal = ssSet.m_SCM_V;
						//mxVal = ssSet.m_MX_Humid_V;
						break;
					case 'w':
					case 'W':
						scmVal = ssSet.m_SCM_W;
						//mxVal = ssSet.m_MX_Humid_W;
						break;
					case 'x':
					case 'X':
						scmVal = ssSet.m_SCM_X;
						//mxVal = ssSet.m_MX_Humid_X;
						break;
					case 'y':
					case 'Y':
						scmVal = ssSet.m_SCM_Y;
						//mxVal = ssSet.m_MX_Humid_Y;
						break;
					case 'z':
					case 'Z':
						scmVal = ssSet.m_SCM_Z;
						//mxVal = ssSet.m_MX_Humid_Z;
						break;
					}
					m_grid.QuickSetNumber(21, row, scmVal);
					//m_grid.QuickSetNumber(22, row, mxVal);
				}
				if (!ssSet.IsFieldNull(&ssSet.m_MXD_Override))
					m_grid.QuickSetNumber(22, row, ssSet.m_MXD_Override);
				else
					m_grid.QuickSetText(22, row, "");
			}
		}
		//wxSet.Close();
		ssSet.Close();
	//}
	m_grid.RedrawAll();
}

void CWorkingSetDialog::SaveMetaTable()
{
	//write metadata fields to wxStation
	CWaitCursor wait;
	char temp[64], station[8];
	CSIGStationSet ssSet(m_pClim->m_pDB);
	ssSet.Open();
	short iVal;
	float fVal;
	double dVal;

	CString str;
	CUGCell cell;
	if(ssSet.IsOpen())
	{
		try
		{    // requery recordset
			for(int rc = 0; rc < m_grid.GetNumberRows(); rc++)
			{
				CString tmp;
				tmp = m_grid.QuickGetText(0, rc);
				strcpy_s(station, tmp);
				sprintf(temp, "[StationID] = '%6.6s'", station);
				ssSet.m_strFilter = _T(temp);
				ssSet.Requery();
				if(!ssSet.IsBOF() && !ssSet.IsEOF())//has a record
				{
					ssSet.Edit();
					ssSet.m_Name = m_grid.QuickGetText(1, rc);//m_Grid.GetCell(rc, 1)->GetText();
					tmp = m_grid.QuickGetText(2, rc);//m_Grid.GetCell(rc, 2)->GetText();
					strcpy_s(temp, tmp);
					temp[1] = 0;
					if(strncmp(ssSet.m_NFDRSFM, temp, 1) != 0)
						fxParamChange = true;
					ssSet.m_NFDRSFM = temp;
					m_grid.GetCell(3, rc, &cell);
					iVal = cell.GetNumber();//pCellCheck->GetCheck();
					if(ssSet.m_Use88 != iVal)
						fxParamChange = true;
					ssSet.m_Use88 = iVal;
					iVal = atoi(m_grid.QuickGetText(4, rc));
					if(ssSet.m_SlopeCls != iVal)
						fxParamChange = true;
					ssSet.m_SlopeCls = iVal;
					iVal = atoi(m_grid.QuickGetText(5, rc));
					if(ssSet.m_ClimateCls != iVal)
						fxParamChange = true;
					ssSet.m_ClimateCls = iVal;
					int second, minute, hour, day, month, year;
					m_grid.GetCell(6, rc, &cell);
					cell.GetTime(&second, &minute, &hour, &day, &month, &year);
					COleDateTime tTime(year, month, day, hour, minute, second);
					if(ssSet.m_GreenJulian != tTime)
						fxParamChange = true;
					ssSet.m_GreenJulian = tTime;
					m_grid.GetCell(7, rc, &cell);
					cell.GetTime(&second, &minute, &hour, &day, &month, &year);
					COleDateTime tTime2(year, month, day, hour, minute, second);
					if(ssSet.m_FreezeJulian != tTime2)
						fxParamChange = true;
					ssSet.m_FreezeJulian = tTime2;
					m_grid.GetCell(8, rc, &cell);
					iVal = cell.GetNumber();
					if(ssSet.m_StartKBDI != iVal)
						fxParamChange = true;
					ssSet.m_StartKBDI = iVal;
					m_grid.GetCell(9, rc, &cell);
					fVal = cell.GetNumber();
					if(ssSet.m_Start1000 != fVal)
						fxParamChange = true;
					ssSet.m_Start1000 = fVal;
					m_grid.GetCell(10, rc, &cell);
					fVal = cell.GetNumber();
					if(ssSet.m_AvgPrecip != fVal)
						fxParamChange = true;
					ssSet.m_AvgPrecip = fVal;
					m_grid.GetCell(11, rc, &cell);
					iVal = cell.GetNumber();
					if(ssSet.m_FM1Eq10 != iVal)
						fxParamChange = true;
					ssSet.m_FM1Eq10 = iVal;
					m_grid.GetCell(12, rc, &cell);
					iVal = cell.GetNumber();
					if(ssSet.m_HerbAnnual != iVal)
						fxParamChange = true;
					ssSet.m_HerbAnnual = iVal;
					m_grid.GetCell(13, rc, &cell);
					iVal = cell.GetNumber();
					if(ssSet.m_Deciduous != iVal)
						fxParamChange = true;
					ssSet.m_Deciduous = iVal;
					str = m_grid.QuickGetText(14, rc);
					if(str.Compare(ssSet.m_Aspect) != 0)
						fxParamChange = true;
					ssSet.m_Aspect = str;
					str = m_grid.QuickGetText(15, rc);
					if(str.Compare(ssSet.m_PsnOnSlope) != 0)
						fxParamChange = true;
					m_grid.GetCell(16, rc, &cell);
					iVal = cell.GetNumber();
					if(ssSet.m_Elevation != iVal)
						fxParamChange = true;
					ssSet.m_Elevation = iVal;
					m_grid.GetCell(17, rc, &cell);
					dVal = cell.GetNumber();
					if(ssSet.m_LatDegrees != dVal)
						fxParamChange = true;
					ssSet.m_LatDegrees = dVal;

					m_grid.GetCell(18, rc, &cell);
					cell.GetTime(&second, &minute, &hour, &day, &month, &year);
					COleDateTime dTime(year, month, day, hour, minute, second);

					if(ssSet.m_DormantJulian != dTime)
						fxParamChange = true;
					ssSet.m_DormantJulian = dTime;

					m_grid.GetCell(19, rc, &cell);
					iVal = cell.GetNumber();
					if(ssSet.m_UseDormant != iVal)
						fxParamChange = true;
					ssSet.m_UseDormant = iVal;

					m_grid.GetCell(20, rc, &cell);
					iVal = cell.GetNumber();
					if(ssSet.m_UseStick != iVal)
						fxParamChange = true;
					ssSet.m_UseStick = iVal;

					//if NFDRS2016 fuel model, need to save SCM_? and MX_Humid_? for station's selected fuel model
					//BOOL bVal;
					if (isNFDRS2016(ssSet.m_NFDRSFM[0]))
					{
						m_grid.GetCell(21, rc, &cell);
						iVal = cell.GetNumber();//pCellCheck->GetCheck();
						//m_grid.GetCell(22, rc, &cell);
						//bVal = cell.GetNumber();

						switch (ssSet.m_NFDRSFM[0])
						{
						case 'v':
						case 'V':
							ssSet.m_SCM_V = iVal;
							//ssSet.m_MX_Humid_V = bVal;
							break;
						case 'w':
						case 'W':
							ssSet.m_SCM_W = iVal;
							//ssSet.m_MX_Humid_W = bVal;
							break;
						case 'x':
						case 'X':
							ssSet.m_SCM_X = iVal;
							//ssSet.m_MX_Humid_X = bVal;
							break;
						case 'y':
						case 'Y':
							//ssSet.m_MX_Humid_Y = bVal;
							ssSet.m_SCM_Y = iVal;
							break;
						case 'z':
						case 'Z':
							ssSet.m_SCM_Z = iVal;
							//ssSet.m_MX_Humid_Z = bVal;
							break;
						}
					}
					m_grid.GetCell(22, rc, &cell);
					int mxdVal = cell.GetNumber();
					if (mxdVal > 0)
						ssSet.m_MXD_Override = mxdVal;
					else
						ssSet.SetFieldNull(&ssSet.m_MXD_Override);

					ssSet.Update();
				}
			}
		}
		catch( CDBException* e )
		{    // Do nothing--used for security violations 
			// when opening tables    
			e->Delete( );
		}
	}
	ssSet.Close();
	if (theApp.m_UseDailyExtremes != m_chkDailyExtremes.GetCheck())
	{
		theApp.m_UseDailyExtremes = m_chkDailyExtremes.GetCheck();
		fxParamChange = true;
	}
}


void CWorkingSetDialog::OnSize(UINT nType, int cx, int cy)
{
	if (cx < 575)
		cx = 575;
	if (cy < 200)
		cy = 200;

	CDialog::OnSize(nType, cx, cy);

	CWaitCursor wait;
	if(m_grid.m_hWnd && m_staticTableLabel.m_hWnd)
	{
		int a,b;
	    a = 137;
	    
		b = cy - (a + 10);

		m_grid.MoveWindow(11, a, cx -15, b);
	}
}

void  CWorkingSetDialog::ConfigCheckBoxes()
{
	bool allNFDRS2016 = true;
	CString str;
	for (int r = 0; r < m_grid.GetNumberRows(); r++)
	{
		str = m_grid.QuickGetText(2, r);
		if (!isNFDRS2016(str[0]))
		{
			allNFDRS2016 = false;
			break;
		}
	}
	m_chkForceRecompute.EnableWindow(allNFDRS2016);
	m_chkDailyExtremes.EnableWindow(allNFDRS2016);
	m_chkDailyExtremes.SetCheck(theApp.m_UseDailyExtremes);
}

bool CWorkingSetDialog::SetYears(CString fmStr/* = ""*/)
{
	bool ret = false;
	int yr1 = 2030, yr2 = 0;
	CUGCell cell;
	CWxSet wxSet(m_pClim->m_pDB);
	wxSet.m_strSort = _T("[ObsDate]");
	char temp[64], station[64];
	bool isValid2016 = true;
	bool using2016 = false;
	//if (itemLoc != CB_ERR)
	//{
		CWaitCursor wait;
		strcpy_s(station, m_pClim->m_sigStaID);
		//m_ctlSIG_Station.GetLBText(itemLoc, station);
		if (strncmp(station, "SIG", 3) == 0)//is a SIG
		{
			CStationInSIGSet sSet(m_pClim->m_pDB);
			sprintf(temp, "[SIG] = '%-20.20s'", &station[6]);
			sSet.m_strFilter = _T(temp);
			sSet.Open();
			wxSet.Open();
			int row = 0;
			while (!sSet.IsEOF() && (!using2016 || (using2016 && isValid2016)))
			{
				sprintf(temp, "[StationID] = '%6.6s'", sSet.m_StationID);
				wxSet.m_strFilter = _T(temp);
				wxSet.Requery();
				if (!wxSet.IsBOF() && !wxSet.IsEOF())//has records
				{	//set years for spinners
					wxSet.MoveFirst();
					if (yr1 > 0)
						yr1 = min(yr1, wxSet.m_ObsDate.GetYear());
					else
						yr1 = wxSet.m_ObsDate.GetYear();
					wxSet.MoveLast();
					if (yr2 > 0)
						yr2 = max(yr2, wxSet.m_ObsDate.GetYear());
					else
						yr2 = wxSet.m_ObsDate.GetYear();
					if (HasUserTables(m_pClim->m_pDB))
					{
						CRecordset userVals(m_pClim->m_pDB);
						CString strSQL;
						strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", sSet.m_StationID);
						userVals.m_strSort = _T("[ObsDate]");
						userVals.Open(CRecordset::dynaset, strSQL);
						if (!userVals.IsBOF() && !userVals.IsEOF())//has records
						{	//set years for spinners
							CDBVariant var;
							userVals.MoveFirst();
							userVals.GetFieldValue("ObsDate", var);
							COleDateTime dt(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
								var.m_pdate->minute, var.m_pdate->second);
							//dt = var.m_boolVal;
							if (yr1 > 0)
								yr1 = min(yr1, dt.GetYear());
							else
								yr1 = dt.GetYear();
							userVals.MoveLast();
							userVals.GetFieldValue("ObsDate", var);
							COleDateTime dt2(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
								var.m_pdate->minute, var.m_pdate->second);
							if (yr2 > 0)
								yr2 = max(yr2, dt2.GetYear());
							else
								yr2 = dt2.GetYear();
						}
						userVals.Close();
					}
					CSIGStationSet ssSet(m_pClim->m_pDB);
					ssSet.m_strFilter.Format("[StationID] = '%6.6s'", sSet.m_StationID);
					ssSet.Open();
					if (isNFDRS2016(fmStr.GetLength() > 0 ? fmStr[0] : ssSet.m_NFDRSFM[0]))
					{
						using2016 = true;
						COleDateTime n2Start, n2End;
						bool valid2016 = m_pClim->m_fpSet->GetNFDRS2016Range(sSet.m_StationID, &n2Start, &n2End);
						if (valid2016 && isValid2016)
						{
							yr1 = min(yr1, n2Start.GetYear());
							yr2 = max(yr2, n2End.GetYear());
						}
						else
						{
							isValid2016 = false;
							yr1 = yr2 = 0;
						}
					}
					ssSet.Close();
				}
				sSet.MoveNext();
			}
		}
		else
		{
			//single station
			wxSet.Open();
			sprintf(temp, "[StationID] = '%6.6s'", station);
			wxSet.m_strFilter = _T(temp);
			wxSet.Requery();
			if (!wxSet.IsBOF() && !wxSet.IsEOF())//has records
			{	//set years for spinners
				wxSet.MoveFirst();
				if (yr1 > 0)
					yr1 = min(yr1, wxSet.m_ObsDate.GetYear());
				else
					yr1 = wxSet.m_ObsDate.GetYear();
				wxSet.MoveLast();
				if (yr2 > 0)
					yr2 = max(yr2, wxSet.m_ObsDate.GetYear());
				else
					yr2 = wxSet.m_ObsDate.GetYear();
				if (HasUserTables(m_pClim->m_pDB))
				{
					CRecordset userVals(m_pClim->m_pDB);
					CString strSQL;
					strSQL.Format("SELECT * FROM ffpUserVals WHERE [StationID] ='%s'", station);
					userVals.m_strSort = _T("[ObsDate]");
					userVals.Open(CRecordset::dynaset, strSQL);
					if (!userVals.IsBOF() && !userVals.IsEOF())//has records
					{	//set years for spinners
						CDBVariant var;
						userVals.MoveFirst();
						userVals.GetFieldValue("ObsDate", var);
						COleDateTime dt(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
							var.m_pdate->minute, var.m_pdate->second);
						//dt = var.m_boolVal;
						if (yr1 > 0)
							yr1 = min(yr1, dt.GetYear());
						else
							yr1 = dt.GetYear();
						userVals.MoveLast();
						userVals.GetFieldValue("ObsDate", var);
						COleDateTime dt2(var.m_pdate->year, var.m_pdate->month, var.m_pdate->day, var.m_pdate->hour,
							var.m_pdate->minute, var.m_pdate->second);
						if (yr2 > 0)
							yr2 = max(yr2, dt2.GetYear());
						else
							yr2 = dt2.GetYear();
					}
					userVals.Close();
				}
				CSIGStationSet ssSet(m_pClim->m_pDB);
				ssSet.m_strFilter.Format("[StationID] = '%6.6s'", station);
				ssSet.Open();
				if (isNFDRS2016(fmStr.GetLength() > 0 ? fmStr[0] : ssSet.m_NFDRSFM[0]))
				{
					using2016 = true;
					COleDateTime n2Start, n2End;
					bool valid2016 = m_pClim->m_fpSet->GetNFDRS2016Range(ssSet.m_StationID, &n2Start, &n2End);
					if (valid2016 && isValid2016)
					{
						yr1 = min(yr1, n2Start.GetYear());
						yr2 = max(yr2, n2End.GetYear());
					}
					else
					{
						isValid2016 = false;
						yr1 = yr2 = 0;
					}
				}
				ssSet.Close();
			}
		}
		CString gStr;
		if (!isValid2016 && using2016)
		{
			gStr.Format("No Suitable Hourly Data");
		}
		else if (yr1 > 0 && yr2 > 0)
		{
			gStr.Format("Data Years (%d - %d)", yr1, yr2);

		}
		else//invalid years
		{
			gStr.Format("No Weather Data");
		}
		dataYearsBtn.SetWindowTextA(gStr);
		m_spnSyear.SetRange(yr1, yr2);
		m_spnEyear.SetRange(yr1, yr2);
		char yrText1[16], yrText2[16];
		m_spnSyear.GetBuddy()->GetWindowText(yrText1, 15);
		m_spnEyear.GetBuddy()->GetWindowText(yrText2, 15);
		if (atoi(yrText1) < yr1 || atoi(yrText1) > yr2)
		{
			sprintf(temp, "%d", yr1);
			m_spnSyear.GetBuddy()->SetWindowText(temp);
		}
		if (atoi(yrText2) < yr1 || atoi(yrText2) > yr2)
		{
			sprintf(temp, "%d", yr2);
			m_spnEyear.GetBuddy()->SetWindowText(temp);
		}
//	}
	wxSet.Close();
	// changed 2012 FFP4.1 - if saved working set has valid years, use
	/*if (m_pSet && m_pSet->m_StartYear > 1900 && m_pSet->m_StartYear < 2100 && m_pSet->m_EndYear > 1900 && m_pSet->m_EndYear < 2100)
	{
	sprintf(temp, "%d", m_pSet->m_StartYear);
	m_spnSyear.GetBuddy()->SetWindowText(temp);
	sprintf(temp, "%d", m_pSet->m_EndYear);
	m_spnEyear.GetBuddy()->SetWindowText(temp);
	}*/
	if (yr1 > 0 && yr2 > 0)
		return true;
	return false;
}


void CWorkingSetDialog::OnBnClickedCheckDailyExtremes()
{
	// TODO: Add your control notification handler code here
}

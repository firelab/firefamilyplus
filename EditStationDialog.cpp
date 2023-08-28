// EditStationDialog.cpp : implementation file
//
 
#include "stdafx.h"
//#include "dibapi.h"
#include "fireplus.h"
//#include "datatbl.h"
//#include "pvnumeric.h"
//#include "pvbutton1.h"
#include "SIGStationSet.h"
#include "EditStationDialog.h"
#include "ObsAgySet.h"
#include "StnTypeSet.h"
#include "CountySet.h"
#include "StateSet.h"
#include ".\editstationdialog.h"
#include "SowThresholdsDlg.h"
#include "SowWetFlagOptionsDlg.h"
#include "wxSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CFireplusApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEditStationDialog dialog

CEditStationDialog::CEditStationDialog(CWnd* pParent /*=NULL*/,
									   CSIGStationSet *_station /*= NULL*/,
									   bool _isNew /*= false*/)
									   : CDialog(CEditStationDialog::IDD, pParent)
									   , m_strNESDID_ID(_T(""))
									   , m_strWRCC_ID(_T(""))
{
	//{{AFX_DATA_INIT(CEditStationDialog)
	name = _T("");
	stationID = _T("");
	obsUnit = _T("");
	//}}AFX_DATA_INIT
	station = _station;
	isNew = _isNew;
}

void CEditStationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditStationDialog)
	DDX_Control(pDX, IDC_COUNTYBOX, county);
	DDX_Control(pDX, IDC_STATIONID, stationIDControl);
	DDX_Control(pDX, IDC_STNTYPE, stnType);
	DDX_Control(pDX, IDC_AGENCY, agency);
	DDX_Control(pDX, IDC_USE88, use88);
	DDX_Control(pDX, IDC_STATE, state);
	DDX_Control(pDX, IDC_SLOPEPOSITION, slopePos);
	DDX_Control(pDX, IDC_SLOPECLASS, slopeClass);
	DDX_Control(pDX, IDC_REGION, region);
	DDX_Control(pDX, IDC_NFDRSFM, NFDRSFM);
	DDX_Control(pDX, IDC_HERBANNUAL, herbAnnual);
	DDX_Control(pDX, IDC_FM1EQ10, fm1eq10);
	DDX_Control(pDX, IDC_DECIDUOUS, deciduous);
	DDX_Control(pDX, IDC_CLIMATECLASS, climClass);
	DDX_Control(pDX, IDC_ASPECT, aspect);
	DDX_Control(pDX, IDC_FREEZEDATE, freezeDate);
	DDX_Control(pDX, IDC_GREENUPDATE, greenupDate);
	DDX_Text(pDX, IDC_NAME, name);
	DDV_MaxChars(pDX, name, 20);
	DDX_Text(pDX, IDC_STATIONID, stationID);
	DDX_Text(pDX, IDC_OBSUNIT, obsUnit);
	DDV_MaxChars(pDX, obsUnit, 20);
	//	DDX_Control(pDX, IDC_ELEVATION, elevation);
	//	DDX_Control(pDX, IDC_FM1000, fm1000);
	//	DDX_Control(pDX, IDC_KBDI, kbdi);
	//	DDX_Control(pDX, IDC_LAT, latitude);
	//	DDX_Control(pDX, IDC_LONG, longitude);
	//	DDX_Control(pDX, IDC_PRECIP, precip);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_GREENUPDATE, greenupDate);
	DDX_Control(pDX, IDC_FREEZEDATE, freezeDate);

	DDX_Control(pDX, IDC_EDITLAT, m_editLat);
	DDX_Control(pDX, IDC_EDITLONG, m_editLong);

	DDX_Control(pDX, IDC_SPINLAT, m_spinLat);
	DDX_Control(pDX, IDC_SPINLONG, m_spinLong);
	DDX_Control(pDX, IDC_SPINELEV, m_spinElev);
	DDX_Control(pDX, IDC_SPINSTART, m_spinKBDI);
	DDX_Control(pDX, IDC_SPINPRECIP, m_numSpinPrecip);
	DDX_Control(pDX, IDC_EDITPRECIP, m_editPrecip);
	DDX_Control(pDX, IDC_EDITFM1000, m_editFM1000);
	DDX_Control(pDX, IDC_EDITLAT, m_editLat);
	DDX_Control(pDX, IDC_EDITLONG, m_editLong);

	DDX_Control(pDX, IDC_SPINFM1000, m_numSpinFM1000);
	DDX_Control(pDX, IDC_DORMANTDATE, m_dormantDate);
	DDX_Control(pDX, IDC_USEDORMANT, m_useDormant);
	DDX_Control(pDX, IDC_USESTICK, m_useStick);
	DDX_Control(pDX, IDC_COMBO_REG_SCHED_OBS_HOUR, m_comboRegSchedObs);
	DDX_Control(pDX, IDC_EDIT_NESDIS_ID, m_editNESDID_ID);
	DDX_Text(pDX, IDC_EDIT_NESDIS_ID, m_strNESDID_ID);
	DDX_Control(pDX, IDC_EDIT_WRCC_ID, m_editWRCC_ID);
	DDX_Text(pDX, IDC_EDIT_WRCC_ID, m_strWRCC_ID);
	DDV_MaxChars(pDX, m_strWRCC_ID, 4);
	DDV_MaxChars(pDX, m_strNESDID_ID, 8);
	DDX_Control(pDX, IDC_SPIN_MAXSC_V, m_spinMaxSC_V);
	DDX_Control(pDX, IDC_SPIN_MAXSC_W, m_spinMaxSC_W);
	DDX_Control(pDX, IDC_SPIN_MAXSC_X, m_spinMaxSC_X);
	DDX_Control(pDX, IDC_SPIN_MAXSC_Y, m_spinMaxSC_Y);
	DDX_Control(pDX, IDC_SPIN_MAXSC_Z, m_spinMaxSC_Z);
	/*DDX_Control(pDX, IDC_CHECK_HUMID_V, m_btnHumid_V);
	DDX_Control(pDX, IDC_CHECK_HUMID_W, m_btnHumid_W);
	DDX_Control(pDX, IDC_CHECK_HUMID_X, m_btnHumid_X);
	DDX_Control(pDX, IDC_CHECK_HUMID_Y, m_btnHumid_Y);
	DDX_Control(pDX, IDC_CHECK_HUMID_Z, m_btnHumid_Z);*/
	DDX_Control(pDX, IDC_COMBO_TIMEZONE, m_comboTimeZone);
	DDX_Control(pDX, IDC_EDIT_KBDI_THRESHOLD, m_editKBDIThreshold);
	DDX_Control(pDX, IDC_SPIN_KBDI_THRESHOLD, m_spinKBDIThreshold);
	DDX_Control(pDX, IDC_EDIT_MXD, m_editMXD);
}

BEGIN_MESSAGE_MAP(CEditStationDialog, CDialog)
	//{{AFX_MSG_MAP(CEditStationDialog)
	ON_CBN_SELCHANGE(IDC_STATE, OnSelchangeState)
	ON_CBN_KILLFOCUS(IDC_CLIMATECLASS, OnKillfocusClimateclass)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_USEDORMANT, OnBnClickedUsedormant)
	ON_BN_CLICKED(IDC_USE88, &CEditStationDialog::OnBnClickedUse88)
	//ON_BN_CLICKED(IDC_BUTTON_SOW_THRESHOLDS, &CEditStationDialog::OnBnClickedButtonSowThresholds)
	//ON_BN_CLICKED(IDC_BUTTON_SOW_WETFLAG_OPTIONS, &CEditStationDialog::OnBnClickedButtonSowWetflagOptions)
	//ON_BN_CLICKED(IDC_BUTTON_RECALC_SOW_WETFLAG, &CEditStationDialog::OnBnClickedButtonRecalcSowWetflag)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditStationDialog message handlers

void CEditStationDialog::OnOK()
{
	// TODO: Add extra validation here
	if(FieldsOK())
	{
		SaveStation();
		CDialog::OnOK();
	}
}

BOOL CEditStationDialog::OnInitDialog()
{
	CString str, tStr;
	CDialog::OnInitDialog();
	freezeDate.SetFormat("M/d");
	greenupDate.SetFormat("M/d");
	m_dormantDate.SetFormat("M/d");
	m_spinLat.SetRangeAndDelta(-180.0, 180.0,0.01);
	m_spinLong.SetRangeAndDelta(-180.0, 180.0,0.01);

	m_spinLat.SetDecimalPlaces(5);
	m_spinLong.SetDecimalPlaces(5);

	m_spinLat.SetBuddy(&m_editLat);
	m_spinLong.SetBuddy(&m_editLong);

	m_spinElev.SetRange(0, 30000);
	m_spinKBDI.SetRange(0, 800);

	m_numSpinPrecip.SetDecimalPlaces (2);
	m_numSpinPrecip.SetTrimTrailingZeros (FALSE);
	m_numSpinPrecip.SetRangeAndDelta (0.0, 200.0, 0.1);
	m_numSpinPrecip.SetPos(0.0);
	m_numSpinPrecip.SetBuddy(&m_editPrecip);
	m_numSpinFM1000.SetDecimalPlaces (2);
	m_numSpinFM1000.SetTrimTrailingZeros (FALSE);
	m_numSpinFM1000.SetRangeAndDelta (0.0, 100.0, 1.0);
	m_numSpinFM1000.SetPos(0.0);
	m_numSpinFM1000.SetBuddy(&m_editFM1000);

	m_spinMaxSC_V.SetRange(1, 300);
	m_spinMaxSC_W.SetRange(1, 300);
	m_spinMaxSC_X.SetRange(1, 300);
	m_spinMaxSC_Y.SetRange(1, 300);
	m_spinMaxSC_Z.SetRange(1, 300);

	m_spinKBDIThreshold.SetRange(0, 800);
	//m_editNESDIS_ID.
	if(station)
	{
		if(!isNew)
		{
			//set string members
			stationID = station->m_StationID;
			name = station->m_Name;
			name.TrimLeft();
			name.TrimRight();
			obsUnit = station->m_ObsUnit;
			NFDRSFM.SelectString(-1, station->m_NFDRSFM);
			//state.SelectString(-1, station->m_State);
			aspect.SelectString(-1, station->m_Aspect);
			slopePos.SelectString(-1, station->m_PsnOnSlope);
			str.Format("%d", station->m_SlopeCls);
			slopeClass.SelectString(-1, str);
			str.Format("%d", station->m_ClimateCls);
			climClass.SelectString(-1, str);
			str.Format("%ld", station->m_FSRegion);
			region.SelectString(-1, str);
			//do the buttons
			use88.SetCheck(station->m_Use88);
			deciduous.SetCheck(station->m_Deciduous);
			herbAnnual.SetCheck(station->m_HerbAnnual);
			fm1eq10.SetCheck(station->m_FM1Eq10);
			//DO THE DATES
			freezeDate.SetTime(station->m_FreezeJulian);
			greenupDate.SetTime(station->m_GreenJulian);
			//do the numbers
			//better check for NULL...
			if(!station->IsFieldNull(&station->m_LonDegrees))
				m_spinLong.SetPos(station->m_LonDegrees);
			if(!station->IsFieldNull(&station->m_LatDegrees))
				m_spinLat.SetPos(station->m_LatDegrees);
			if(!station->IsFieldNull(&station->m_Elevation))
				m_spinElev.SetPos(station->m_Elevation);
			if(!station->IsFieldNull(&station->m_AvgPrecip))
				m_numSpinPrecip.SetPos(station->m_AvgPrecip);
			if(!station->IsFieldNull(&station->m_Start1000))
				m_numSpinFM1000.SetPos(station->m_Start1000);
			//if(!station->IsFieldNull(&station->m_StartGreenHerb))
			//greenHerb.SetValueInteger(station->m_StartGreenHerb);
			//if(!station->IsFieldNull(&station->m_StartGreenShrub))
			//greenShrub.SetValueInteger(station->m_StartGreenShrub);
			if(!station->IsFieldNull(&station->m_StartKBDI))
				m_spinKBDI.SetPos(station->m_StartKBDI);
			if(!station->IsFieldNull(&station->m_UseDormant))
				m_useDormant.SetCheck(station->m_UseDormant);
			else
				m_useDormant.SetCheck(0);
			if(!station->IsFieldNull(&station->m_DormantJulian))
				m_dormantDate.SetTime(station->m_DormantJulian);
			else //set the default dormant date
			{
				COleDateTime dTime;
				dTime.SetDateTime(2007, 9, 16,9,0,0);
				m_dormantDate.SetTime(dTime);
			}
			if(!station->IsFieldNull(&station->m_UseStick))
				m_useStick.SetCheck(station->m_UseStick);
			else
				m_useStick.SetCheck(1);
			//else
			if(!station->IsFieldNull(&station->m_NESDIS_ID))
				m_strNESDID_ID = station->m_NESDIS_ID;
			if(!station->IsFieldNull(&station->m_RegSchdObs))
			{
				str.Format("%d", station->m_RegSchdObs);
				m_comboRegSchedObs.SelectString(-1, str);
			}
			if(!station->IsFieldNull(&station->m_WRCC_ID))
				m_strWRCC_ID = station->m_WRCC_ID;

			//nfdr2016 specific
			if (!station->IsFieldNull(&station->m_SCM_V))
				m_spinMaxSC_V.SetPos(station->m_SCM_V);
			else
			{
				theApp.m_NFDRS2016.iSetFuelModel('V');
				m_spinMaxSC_V.SetPos(theApp.m_NFDRS2016.GetSCMax());
			}
			if (!station->IsFieldNull(&station->m_SCM_W))
				m_spinMaxSC_W.SetPos(station->m_SCM_W);
			else
			{
				theApp.m_NFDRS2016.iSetFuelModel('W');
				m_spinMaxSC_W.SetPos(theApp.m_NFDRS2016.GetSCMax());
			}
			if (!station->IsFieldNull(&station->m_SCM_X))
				m_spinMaxSC_X.SetPos(station->m_SCM_X);
			else
			{
				theApp.m_NFDRS2016.iSetFuelModel('X');
				m_spinMaxSC_X.SetPos(theApp.m_NFDRS2016.GetSCMax());
			}
			if (!station->IsFieldNull(&station->m_SCM_Y))
				m_spinMaxSC_Y.SetPos(station->m_SCM_Y);
			else
			{
				theApp.m_NFDRS2016.iSetFuelModel('Y');
				m_spinMaxSC_Y.SetPos(theApp.m_NFDRS2016.GetSCMax());
			}
			if (!station->IsFieldNull(&station->m_SCM_Z))
				m_spinMaxSC_Z.SetPos(station->m_SCM_Z);
			else
			{
				theApp.m_NFDRS2016.iSetFuelModel('Z');
				m_spinMaxSC_Z.SetPos(theApp.m_NFDRS2016.GetSCMax());
			}

			/*if (!station->IsFieldNull(&station->m_MX_Humid_V))
				m_btnHumid_V.SetCheck(station->m_MX_Humid_V);
			else
			{
				theApp.m_NFDRS2016.iSetFuelModel('V');
				m_btnHumid_V.SetCheck(theApp.m_NFDRS2016.GetMxdHumid());
			}
			if (!station->IsFieldNull(&station->m_MX_Humid_W))
				m_btnHumid_W.SetCheck(station->m_MX_Humid_W);
			else
			{
				theApp.m_NFDRS2016.iSetFuelModel('W');
				m_btnHumid_W.SetCheck(theApp.m_NFDRS2016.GetMxdHumid());
			}
			if (!station->IsFieldNull(&station->m_MX_Humid_X))
				m_btnHumid_X.SetCheck(station->m_MX_Humid_X);
			else
			{
				theApp.m_NFDRS2016.iSetFuelModel('X');
				m_btnHumid_X.SetCheck(theApp.m_NFDRS2016.GetMxdHumid());
			}
			if (!station->IsFieldNull(&station->m_MX_Humid_Y))
				m_btnHumid_Y.SetCheck(station->m_MX_Humid_Y);
			else
			{
				theApp.m_NFDRS2016.iSetFuelModel('Y');
				m_btnHumid_Y.SetCheck(theApp.m_NFDRS2016.GetMxdHumid());
			}
			if (!station->IsFieldNull(&station->m_MX_Humid_Z))
				m_btnHumid_Z.SetCheck(station->m_MX_Humid_Z);
			else
			{
				theApp.m_NFDRS2016.iSetFuelModel('Z');
				m_btnHumid_Z.SetCheck(theApp.m_NFDRS2016.GetMxdHumid());
			}*/
			if (!station->IsFieldNull(&station->m_KBDIThreshold))
				m_spinKBDIThreshold.SetPos(station->m_KBDIThreshold);
			if (!station->IsFieldNull(&station->m_timeZoneOffset))
			{
				int tzLoc;
				for (int t = 0; t < m_comboTimeZone.GetCount(); t++)
				{
					tzLoc = m_comboTimeZone.GetItemData(t);
					if (tzLoc == station->m_timeZoneOffset)
					{
						m_comboTimeZone.SetCurSel(t);
						break;
					}
				}
			}
			if (!station->IsFieldNull(&station->m_MXD_Override))
			{
				CString tmpMXD;
				tmpMXD.Format("%d", station->m_MXD_Override);
				m_editMXD.SetWindowTextA(tmpMXD);
			}

		}
		else
		{
			//nfdr2016 specific defaults for SCM_? and MX_Humid_?
			theApp.m_NFDRS2016.iSetFuelModel('V');
			m_spinMaxSC_V.SetPos(theApp.m_NFDRS2016.GetSCMax());
			//m_btnHumid_V.SetCheck(theApp.m_NFDRS2016.GetMxdHumid());
			theApp.m_NFDRS2016.iSetFuelModel('W');
			m_spinMaxSC_W.SetPos(theApp.m_NFDRS2016.GetSCMax());
			//m_btnHumid_W.SetCheck(theApp.m_NFDRS2016.GetMxdHumid());
			theApp.m_NFDRS2016.iSetFuelModel('X');
			m_spinMaxSC_X.SetPos(theApp.m_NFDRS2016.GetSCMax());
			//m_btnHumid_X.SetCheck(theApp.m_NFDRS2016.GetMxdHumid());
			theApp.m_NFDRS2016.iSetFuelModel('Y');
			m_spinMaxSC_Y.SetPos(theApp.m_NFDRS2016.GetSCMax());
			//m_btnHumid_Y.SetCheck(theApp.m_NFDRS2016.GetMxdHumid());
			theApp.m_NFDRS2016.iSetFuelModel('Z');
			m_spinMaxSC_Z.SetPos(theApp.m_NFDRS2016.GetSCMax());
			//m_btnHumid_Z.SetCheck(theApp.m_NFDRS2016.GetMxdHumid());
		}
		//set comboboxes with lookups
		//observation agency
		CObsAgySet agencySet(station->m_pDatabase);
		agencySet.m_strSort = "[WxAgy]";
		agencySet.Open();
		int loc = 0;
		while(!agencySet.IsEOF())
		{
			str.Format("%ld - %s", agencySet.m_WxAgy, agencySet.m_AgencyDescr);
			agency.AddString(str);
			if(!isNew)
			{
				if(agencySet.m_WxAgy == station->m_ObsAgy)
					agency.SetCurSel(loc);
			}
			loc++;
			agencySet.MoveNext();
		}
		agencySet.Close();
		//station types
		CStnTypeSet stnTypeSet(station->m_pDatabase);
		stnTypeSet.m_strSort = "[StnType]";
		stnTypeSet.Open();
		loc = 0;
		while(!stnTypeSet.IsEOF())
		{
			str.Format("%ld - %s", stnTypeSet.m_StnType, stnTypeSet.m_TypeDescr);
			stnType.AddString(str);
			if(!isNew)
			{
				if(stnTypeSet.m_StnType == station->m_StnType)
					stnType.SetCurSel(loc);
			}
			loc++;
			stnTypeSet.MoveNext();
		}
		stnTypeSet.Close();
		//states
		CStateSet stateSet(station->m_pDatabase);
		stateSet.m_strSort = "[State]";
		stateSet.Open();
		loc = 0;
		while(!stateSet.IsEOF())
		{
			str.Format("%s", stateSet.m_State);
			state.AddString(str);
			if(!isNew)
			{
				if(stateSet.m_State == station->m_State)
					state.SetCurSel(loc);
			}
			loc++;
			stateSet.MoveNext();
		}
		stateSet.Close();
		//counties
		if(!isNew)
		{
			AddCounties();
			if(!station->IsFieldNull(&station->m_County))
			{
				CString find;
				find.Format("%03ld", station->m_County);
				county.SelectString(-1, find);
			}
		}
		AddTimeZones();
		if (!isNew)
		{
			if (!station->IsFieldNull(&station->m_timeZoneOffset))
			{
				for (int i = 0; i < m_comboTimeZone.GetCount(); i++)
				{
					int tz = m_comboTimeZone.GetItemData(i);
					if (station->m_timeZoneOffset == tz)
					{
						m_comboTimeZone.SetCurSel(i);
						break;
					}
				}
			}
		}
	}

	//m_editWRCC_ID.SetLimitText((UINT)4);
	m_dormantDate.EnableWindow(m_useDormant.GetCheck());
	UpdateData(false);
	if(!isNew)
		stationIDControl.SetReadOnly(true);
	fm1eq10.EnableWindow(use88.GetCheck());
	deciduous.EnableWindow(use88.GetCheck());
	CEdit* pEditControl = (CEdit*)GetDlgItem(IDC_EDIT_WRCC_ID);
	if(pEditControl)
	{
		pEditControl->SetLimitText(4);
	}
	pEditControl = (CEdit*)GetDlgItem(IDC_EDIT_NESDIS_ID);
	if(pEditControl)
	{
		pEditControl->SetLimitText(8);
	}
	pEditControl = (CEdit*)GetDlgItem(IDC_NAME);
	if(pEditControl)
	{
		pEditControl->SetLimitText(20);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CEditStationDialog::FieldsOK()
{
	//check mandatory fields
	if(!UpdateData(true))
		return false;
	bool ret = true;
	CString str, errMsg;
	errMsg = "";
	//stationID
	if(isNew)
	{
		if(stationID.GetLength() != 6)
		{
			errMsg.Format("ERROR: StationID must contain 6 digits.\n");
			ret = false;
		}
		else
		{
			for(int i = 0; i < 6; i++)
			{
				if(!isdigit(stationID[i]))
				{
					errMsg.Format("ERROR: StationID must contain 6 digits.\n"
						"No blanks or characters.");
					ret = false;
					break;
				}
			}
			if(ret)
			{
				//make sure unique
				CSIGStationSet stas(station->m_pDatabase);
				stas.m_strFilter.Format("[StationID] = '%6.6s'", stationID);
				stas.Open();
				if(!stas.IsEOF())
				{
					errMsg.Format("ERROR: StationID %s already exists!\n",
						stationID);
					ret = false;
				}
				stas.Close();
			}
		}
		//stationID OK!
	}
	if(ret)
	{//make sure name isn't blank
		name.TrimRight();
		name.TrimLeft();
		if(name.GetLength() == 0)
		{
			errMsg.Format("ERROR: Station name can not be blank.\n");
			ret = false;
		}
	}
	if(errMsg.GetLength() > 0)
	{
		AfxMessageBox(errMsg, MB_OK);
		return false;
	}
	return true;
}

void CEditStationDialog::SaveStation()
{
	CString str1, str2;
	CWaitCursor wait;
	if(station)
	{
		if(isNew)
		{
			station->AddNew();
			station->m_StationID = stationID;
		}
		else
			station->Edit();
		//set string members
		station->m_StationID = stationID;
		station->m_Name = name;
		station->m_ObsUnit = obsUnit;
		agency.GetWindowText(str1);
		str2.Format("%2.2s", str1);
		station->m_ObsAgy = atoi(str2);
		stnType.GetWindowText(str1);
		str2.Format("%2.2s", str1);
		station->m_StnType = atoi(str2);
		NFDRSFM.GetWindowText(str1);
		str2.Format("%1.1s", str1);
		station->m_NFDRSFM = str2;
		state.GetWindowText(str1);
		str2.Format("%2.2s", str1);
		station->m_State = str2;
		aspect.GetWindowText(str1);
		str2.Format("%1.1s", str1);
		station->m_Aspect = str2;
		slopePos.GetWindowText(str1);
		str2.Format("%1.1s", str1);
		station->m_PsnOnSlope = str2;
		slopeClass.GetWindowText(str1);
		str2.Format("%1.1s", str1);
		station->m_SlopeCls = atoi(str2);
		climClass.GetWindowText(str1);
		str2.Format("%1.1s", str1);
		station->m_ClimateCls = atoi(str2);
		region.GetWindowText(str1);
		str2.Format("%2.2s", str1);
		CString cStr;
		county.GetWindowText(cStr);
		cStr.TrimRight();
		if(cStr.GetLength() > 0)
			station->m_County = atol(cStr);
		station->m_FSRegion = atoi(str2);
		station->m_Use88 = use88.GetCheck();
		station->m_Deciduous = deciduous.GetCheck();
		station->m_HerbAnnual = herbAnnual.GetCheck();
		station->m_FM1Eq10 = fm1eq10.GetCheck();
		freezeDate.GetTime(station->m_FreezeJulian);
		greenupDate.GetTime(station->m_GreenJulian);
		station->m_LonDegrees = (double)m_spinLong.GetPos();//longitude.GetValueInteger();
		station->m_LatDegrees = (double)m_spinLat.GetPos();//latitude.GetValueInteger();
		station->m_Elevation = (short)m_spinElev.GetPos();//elevation.GetValueInteger();
		station->m_AvgPrecip = (float)m_numSpinPrecip.GetPos();
		station->m_Start1000 = (float)m_numSpinFM1000.GetPos();
		//		station->m_StartGreenHerb = greenHerb.GetValueInteger();
		//station->m_StartGreenShrub = greenShrub.GetValueInteger();
		station->m_StartKBDI = (short)m_spinKBDI.GetPos();//kbdi.GetValueInteger();
		station->m_UseDormant = m_useDormant.GetCheck();
		m_dormantDate.GetTime(station->m_DormantJulian);
		station->m_UseStick = m_useStick.GetCheck();

		m_comboRegSchedObs.GetWindowText(str1);
		str2.Format("%2.2s", str1);
		int oldHour = station->m_RegSchdObs;
		station->m_RegSchdObs = atoi(str2);
		station->m_NESDIS_ID = m_strNESDID_ID;
		station->m_WRCC_ID = m_strWRCC_ID;
		station->m_SCM_V = m_spinMaxSC_V.GetPos();
		//station->m_MX_Humid_V = m_btnHumid_V.GetCheck();
		station->m_SCM_W = m_spinMaxSC_W.GetPos();
		//station->m_MX_Humid_W = m_btnHumid_W.GetCheck();
		station->m_SCM_X = m_spinMaxSC_X.GetPos();
		//station->m_MX_Humid_X = m_btnHumid_X.GetCheck();
		station->m_SCM_Y = m_spinMaxSC_Y.GetPos();
		//station->m_MX_Humid_Y = m_btnHumid_Y.GetCheck();
		station->m_SCM_Z = m_spinMaxSC_Z.GetPos();
		//station->m_MX_Humid_Z = m_btnHumid_Z.GetCheck();
		m_editKBDIThreshold.GetWindowTextA(str1);
		station->m_KBDIThreshold = atoi(str1);
		int sel = m_comboTimeZone.GetCurSel();
		if (sel >= 0)
			station->m_timeZoneOffset = m_comboTimeZone.GetItemData(sel);
		CString mxdStr;
		m_editMXD.GetWindowTextA(mxdStr);
		if (mxdStr.GetLength() > 0)
			station->m_MXD_Override = atoi(mxdStr);
		else
			station->SetFieldNull(&station->m_MXD_Override);
		//commit the changes
		station->Update();

		if (station->m_RegSchdObs != oldHour)
		{
			/* update DailyObs value in weather records */
			CWxSet wxset(station->m_pDatabase);
			//CCancelDialog cd;
			//CString tempstr;
			//tempstr.Format("Updating DailyObs field for Wx records...");
			//cd.SetStatusString(tempstr);

			wxset.m_strFilter.Format("[StationID]='%s'",station->m_StationID);
			wxset.Open();
			wxset.setDailyObs(station->m_RegSchdObs);

			wxset.Close();
		}

	}
}
void CEditStationDialog::AddTimeZones()
{
	m_comboTimeZone.ResetContent();
	for (auto it = theApp.m_timeZones.m_timezones.begin(); it != theApp.m_timeZones.m_timezones.end(); ++it)
	{
		CTimeZone tz = (*it);
		CString tzStr;
		tzStr.Format("UTC %d %s-%s", tz.m_offset, tz.m_abbrv.c_str(), tz.m_name.c_str());
		int loc = m_comboTimeZone.AddString(tzStr);
		m_comboTimeZone.SetItemData(loc, tz.m_offset);
	}
}

void CEditStationDialog::AddCounties()
{
	county.ResetContent();
	CString sStr;
	state.GetWindowText(sStr);
	sStr.TrimRight();
	if(sStr.GetLength() > 0)
	{
		CCountySet countySet(station->m_pDatabase);
		countySet.m_strFilter.Format("[State] = '%s'", sStr);
		countySet.m_strSort.Format("[CountyID]");
		countySet.Open();
		while(!countySet.IsEOF())
		{
			sStr.Format("%03ld %s", countySet.m_CountyID, countySet.m_CountyName);
			county.AddString(sStr);
			countySet.MoveNext();
		}
		countySet.Close();
	}
}

void CEditStationDialog::OnSelchangeState()
{
	// TODO: Add your control notification handler code here
	AddCounties();
}

void CEditStationDialog::OnKillfocusClimateclass()
{
	// TODO: Add your control notification handler code here
	int c = climClass.GetCurSel();
	if(c >= 0)
	{
		CString str;
		climClass.GetWindowText(str);
		int d = atoi(str);
		double e = 5.0 * d + 5.0;
		//fm1000.SetValueReal(e);
		m_numSpinFM1000.SetPos(e);
		//fm1000.Invalidate();
		UpdateWindow();//Data();
	}
}

void CEditStationDialog::OnBnClickedUsedormant()
{
	m_dormantDate.EnableWindow(m_useDormant.GetCheck());
}

void CEditStationDialog::OnBnClickedUse88()
{
	//UpdateData();
	fm1eq10.EnableWindow(use88.GetCheck());
	deciduous.EnableWindow(use88.GetCheck());
}

/*void CEditStationDialog::OnBnClickedButtonSowThresholds()
{
UpdateData();
CSowThresholdsSet sowSet(station->m_pDatabase);
sowSet.m_strFilter.Format("[SIG_Station] = '%s'", station->m_StationID);
sowSet.Open();
if(sowSet.IsBOF() || sowSet.IsEOF())
{
sowSet.AddDefaultForStation(station->m_StationID);
}
CSowThresholdsDlg std(this, &sowSet);
std.DoModal();
sowSet.Close();
}

void CEditStationDialog::OnBnClickedButtonSowWetflagOptions()
{
UpdateData();
CSowOptionsSet sowSet(station->m_pDatabase);
sowSet.m_strFilter.Format("[SIG_Station] = '%s'", station->m_StationID);
sowSet.Open();
if(sowSet.IsBOF() || sowSet.IsEOF())
{
sowSet.AddNew();
sowSet.m_SIG_Station = station->m_StationID;
sowSet.m_SOW_Usage = 0;
sowSet.m_WetFlag_Usage = 0;
sowSet.Update();
sowSet.Requery();
}
CSowWetFlagOptionsDlg std(this, &sowSet);
std.DoModal();
sowSet.Close();
}

void CEditStationDialog::OnBnClickedButtonRecalcSowWetflag()
{
CWxSet wxSet(station->m_pDatabase);
wxSet.m_strFilter.Format("[StationID] = '%6.6s' and [ObsType] = 'R'", station->m_StationID);
wxSet.Open();
if(wxSet.IsBOF() && wxSet.IsEOF())//no RAWS records
{
AfxMessageBox("Station needs hourly weather to calculate SR_SOW and SR_WetFlag\nSolar Radiation field should also be populated");
return;
}
wxSet.m_strFilter.Format("[StationID] = '%6.6s'", station->m_StationID);
wxSet.Requery();
long nUpdates = wxSet.CalcSOW();
CString msg;
msg.Format("SR_SOW/SR_WetFlag calculation complete. Updated %ld records.", nUpdates);
AfxMessageBox(msg);
}*/
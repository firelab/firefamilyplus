// CFW21ExportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "WxSet.h"

#include "CFW21ExportDlg.h"
#include "afxdialogex.h"

#include "unordered_map"
// CFW21ExportDlg dialog
#include "SIGStationSet.h"

extern CFireplusApp theApp;


enum FW21_TIME_FORMAT { TF_BASIC_UTC, TF_BASIC_OFFET, TF_EXTENDED_UTC, TF_EXTENDED_OFFSET, TF_END };
static const CString FW21_TimeFormats[TF_END] =
{
	"Basic UTC",
	"Basic UTC Offest",
	"Extended UTC",
	"Extended UTC Offset",
};

IMPLEMENT_DYNAMIC(CFW21ExportDlg, CDialogEx)

CFW21ExportDlg::CFW21ExportDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FW21_EXPORT, pParent)
{
	m_strDestFileName = "";
}

CFW21ExportDlg::~CFW21ExportDlg()
{
}

void CFW21ExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DEST_FILE, m_editDestFile);
	DDX_Control(pDX, IDC_CHECK_GUSTS, m_btnWindGusts);
	DDX_Control(pDX, IDC_COMBO_TIME_FORMAT, m_comboTimeFormat);
}


BEGIN_MESSAGE_MAP(CFW21ExportDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DEST_FILE, &CFW21ExportDlg::OnBnClickedButtonDestFile)
END_MESSAGE_MAP()


// CFW21ExportDlg message handlers


void CFW21ExportDlg::OnBnClickedButtonDestFile()
{
	CFileDialog fd(FALSE, "fw21", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Fire Weather Files (*.fw21)|*.fw21|All Files (*.*)|*.*||", this);
	if (fd.DoModal() == IDOK)
	{
		m_strDestFileName = fd.GetPathName();
		PathSetDlgItemPath(m_hWnd, IDC_EDIT_DEST_FILE, m_strDestFileName);
	}
	theApp.SetUserDir(fd.GetPathName());
}


BOOL CFW21ExportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	for (int tf = TF_BASIC_UTC; tf < TF_END; tf++)
	{
		int loc = m_comboTimeFormat.AddString(FW21_TimeFormats[tf]);
		m_comboTimeFormat.SetItemData(loc, tf);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

using namespace std;

void CFW21ExportDlg::OnOK()
{
	if (m_strDestFileName.GetLength() <= 0)
	{
		AfxMessageBox("A destination file must be selected");
		return;
	}

	FW21_TIME_FORMAT tf = TF_BASIC_UTC;
	int sel = m_comboTimeFormat.GetCurSel();
	if (sel >= 0)
		tf = (FW21_TIME_FORMAT)m_comboTimeFormat.GetItemData(sel);
	//bool useOffset = false;
	bool addGusts = false;
	//if (m_radioOffset.GetCheck())
	//	useOffset = true;
	if (m_btnWindGusts.GetCheck())
		addGusts = true;

	//output csv Header
	CString header = "DateTime, Temperature(F), RelativeHumidity(%), Precipitation(in), WindSpeed(mph), WindAzimuth(degrees), SolarRadiation(W/m2), SnowFlag";
	CString gustHeader = ", GustSpeed(mph), GustAzimuth(degrees)";

	FILE *out = NULL;
	errno_t err = fopen_s(&out, m_strDestFileName, "wt");
	if (err != 0)
	{
		CString msg;
		msg.Format("Error creating %s as output", m_strDestFileName);
		AfxMessageBox(msg);
	}
	CWaitCursor wait;

	fprintf_s(out, "%s", header);
	if(addGusts)
		fprintf_s(out, "%s", gustHeader);
	fprintf_s(out, "\n");
	//now cycle through the records and dump fields to the file
	records->MoveFirst();
	CString dateStr, tmpStr;
	COleDateTime utcTime;
	COleDateTimeSpan offsetSpan;
	CSIGStationSet staSet(records->m_pDatabase);
	int offset;
	staSet.m_strFilter.Format("[StationID] = '%6.6s'", records->m_StationID);
	staSet.Open();
	offset = staSet.m_timeZoneOffset;
	staSet.Close();
	offsetSpan.SetDateTimeSpan(0, offset, 0, 0);
	CString strTemp, strRH, strPcp, strWS, strWAzi, strSR, strSnow, strGS, strGAzi;
	while (!records->IsEOF())
	{
		switch (tf)
		{
		case TF_EXTENDED_OFFSET:
			dateStr = records->m_ObsDate.Format("%Y-%m-%dT%H:%M:%S");
			tmpStr.Format("%+03d:00", offset);
			dateStr = dateStr + tmpStr;
			break;
		case TF_EXTENDED_UTC:
			utcTime = records->m_ObsDate - offsetSpan;
			dateStr = utcTime.Format("%Y-%m-%dT%H:%M:%SZ");
			break;
		case TF_BASIC_OFFET:
			dateStr = records->m_ObsDate.Format("%Y%m%dT%H%M%S");
			tmpStr.Format("%+03d:00", offset);
			dateStr = dateStr + tmpStr;
			break;
			break;
		case TF_BASIC_UTC:
		default:
			utcTime = records->m_ObsDate - offsetSpan;
			dateStr = utcTime.Format("%Y%m%dT%H%M%SZ");
			break;
		}
		/*if (!useOffset)
		{
			utcTime = records->m_ObsDate - offsetSpan;
			dateStr = utcTime.Format("%Y-%m-%dT%H:%M:%SZ");
			//tmpStr.Format("%d", offset);
			//dateStr = dateStr
		}
		else
		{
			dateStr = records->m_ObsDate.Format("%Y-%m-%dT%H:%M:%S");
			tmpStr.Format("%+03d:00", offset);
			dateStr = dateStr + tmpStr;
		}*/
		if (records->IsFieldNull(&records->m_Temp))
			strTemp = "";
		else
			strTemp.Format("%ld", records->m_Temp);
		if (records->IsFieldNull(&records->m_RH))
			strRH = "";
		else
			strRH.Format("%ld", max(records->m_RH, 1));
		if (records->IsFieldNull(&records->m_HourlyPrecip))
			strPcp = "";
		else
			strPcp.Format("%0.3f", records->m_HourlyPrecip);
		if (records->IsFieldNull(&records->m_WS))
			strWS = "";
		else
			strWS.Format("%ld", records->m_WS);
		if (records->IsFieldNull(&records->m_WAzimuth))
			strWAzi = "";
		else
			strWAzi.Format("%ld", records->m_WAzimuth);
		if (records->IsFieldNull(&records->m_SolarRadiation))
			strSR = "";
		else
			strSR.Format("%ld", records->m_SolarRadiation);
		if (records->IsFieldNull(&records->m_SnowFlag))
			strSnow = "";
		else
			strSnow.Format("%ld", records->m_SnowFlag);
		fprintf_s(out, "%s,%s,%s,%s,%s,%s,%s,%s", dateStr, strTemp, strRH, strPcp, strWS, strWAzi, strSR, strSnow);
		if (addGusts)
		{
			if (records->IsFieldNull(&records->m_GustSpd))
				strGS = "";
			else
				strGS.Format("%ld", records->m_GustSpd);
			if (records->IsFieldNull(&records->m_GustDir))
				strGAzi = "";
			else
				strGAzi.Format("%ld", records->m_GustDir);
			fprintf_s(out, ",%s,%s", strGS, strGAzi);
		}
		fprintf_s(out, "\n");
		records->MoveNext();
	}
	fclose(out);
	AfxMessageBox("FW21 export completed successfully");
	CDialogEx::OnOK();
}

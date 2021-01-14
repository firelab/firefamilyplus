// WthrPrintDialog.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "WthrPrintDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWthrPrintDialog dialog
//const int prnFields = 19;
char *fireColNames[firePrnFields] = 
{
	"Date      ",
	"Time ",
	"Acres     ",
	"Cause",
	"Name               ",
	"FireNum ",
};

char *fireColFormats[firePrnFields] = 
{
	"%10.10s",
	"%5.5s",
	"%10.1f",
	"%5ld",
	"%20.20s",
	"%8.8s",
};

int fireWidths[firePrnFields] = 
{
	10,
	5,
	10,
	5,
	20,
	8,
};

char *colNames[prnFields] = 
{
	"Sta.  ",
	"Date      ",
	"Time ",
	"Type",
	"SOW",
	"Temp",
	"RH ",
	"PcpAmt",
	"PcpDur",
	"W Spd",
	"W Dir",
	"W Azi",
	"MaxTmp",
	"MinTmp",
	"MaxRH",
	"MinRH",
	"Sea",
	"MC10  ",
	"GrHrb",
	"GrShrb",
	"WetFlag",
	"SolRad",
	"G Spd",
	"G Dir",
	"HrlyPcp",
	"SR_SOW",
	"SR_WF",
	"SnowFlag",
	"FM1",
	"FM10",
	"FM100",
	"FM1000",
	"FuelTemp",
};

char *colFormats[prnFields] = 
{
	"%6.6s",
	"%10.10s",
	"%5.5s",
	"%4.4s",
	"%3d",
	"%4d",
	"%3d",
	"%6.2f",
	"%6d",
	"%5d",
	"%5d",
	"%5d",
	"%6d",
	"%6d",
	"%5d",
	"%5d",
	"%3d",
	"%6.2f",
	"%5d",
	"%6d",
	"%7d",
	"%6d",
	"%5d",
	"%5d",
	"%6.2f",
	"%3d",
	"%7d",
	"%8d",
	"%.2f",
	"%.2f",
	"%.2f",
	"%.2f",
	"%.2f",
};

int widths[prnFields] = 
{
	6,
	10,
	5,
	4,
	3,
	4,
	3,
	6,
	6,
	5,
	5,
	5,
	6,
	6,
	5,
	5,
	3,
	6,
	5,
	6,
	7,
	6,
	5,
	5,
	6,
	5,
	7,
	8,
	8,
	8,
	8,
	8,
	8,
};


CWthrPrintDialog::CWthrPrintDialog(CWnd* pParent, CWxSet *_wxSet, CFireEditSet *_fireSet)
	: CDialog(CWthrPrintDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWthrPrintDialog)
	headers = FALSE;
	pageNums = FALSE;
	recNums = FALSE;
	//}}AFX_DATA_INIT
	pFont = NULL;
	retRec = -1;
	wxSet = _wxSet;
	fireSet = _fireSet;
	if(wxSet)
	{
		CRecordsetStatus recStatus;
		wxSet->GetStatus(recStatus);
		retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
		//retRec = wxSet->GetAbsolutePosition();
	}
	else if(fireSet)
	{
		CRecordsetStatus recStatus;
		fireSet->GetStatus(recStatus);
		retRec = recStatus.m_lCurrentRecord;//records->GetAbsolutePosition();
		//retRec = fireSet->GetAbsolutePosition();
	}
}


void CWthrPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWthrPrintDialog)
	DDX_Control(pDX, IDC_SELECTEDLIST, selectedList);
	DDX_Control(pDX, IDC_FIELDSLIST, fieldsList);
	DDX_Check(pDX, IDC_HEADERS, headers);
	DDX_Check(pDX, IDC_PAGENUMS, pageNums);
	DDX_Check(pDX, IDC_RECNUMS, recNums);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWthrPrintDialog, CDialog)
	//{{AFX_MSG_MAP(CWthrPrintDialog)
	ON_BN_CLICKED(IDC_SELECTALL, OnSelectall)
	ON_BN_CLICKED(IDC_REMOVEALL, OnRemoveall)
	ON_BN_CLICKED(IDC_MOVELEFT, OnMoveleft)
	ON_BN_CLICKED(IDC_MOVERIGHT, OnMoveright)
	ON_LBN_DBLCLK(IDC_FIELDSLIST, OnDblclkFieldslist)
	ON_LBN_DBLCLK(IDC_SELECTEDLIST, OnDblclkSelectedlist)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_UP, OnUp)
	ON_BN_CLICKED(IDC_DOWN, OnDown)
	ON_BN_CLICKED(IDC_TOP, OnTop)
	ON_BN_CLICKED(IDC_BOTTOM, OnBottom)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWthrPrintDialog message handlers

BOOL CWthrPrintDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int loc;
	if(wxSet)
	{
		for(int f = 0; f < prnFields; f++)
		{
			loc = fieldsList.AddString(colNames[f]);
			fieldsList.SetItemData(loc, f);
		}
	}
	else if(fireSet)
	{
		for(int f = 0; f < firePrnFields; f++)
		{
			loc = fieldsList.AddString(fireColNames[f]);
			fieldsList.SetItemData(loc, f);
		}
	}
	upButton.AutoLoad(IDC_UP, this);
	downButton.AutoLoad(IDC_DOWN, this);
	topButton.AutoLoad(IDC_TOP, this);
	bottomButton.AutoLoad(IDC_BOTTOM, this);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWthrPrintDialog::OnSelectall() 
{
	while(fieldsList.GetCount() > 0)
	{
		fieldsList.SetCurSel(0);
		OnDblclkFieldslist();
	}
}

void CWthrPrintDialog::OnRemoveall() 
{
	while(selectedList.GetCount() > 0)
	{
		selectedList.SetCurSel(0);
		OnDblclkSelectedlist();
	}	
}

void CWthrPrintDialog::OnUp() 
{
	CWaitCursor wait;
	if(selectedList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = selectedList.GetCurSel();
		if(cur > 0)
		{
			selectedList.GetText(cur, str);
			int val = selectedList.GetItemData(cur);
			selectedList.DeleteString(cur);
			cur--;
			int loc = selectedList.InsertString(cur, str);
			selectedList.SetItemData(loc, val);
			selectedList.SetCurSel(loc);
		}
	}
}

void CWthrPrintDialog::OnTop() 
{
	CWaitCursor wait;
	if(selectedList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = selectedList.GetCurSel();
		if(cur > 0)
		{
			selectedList.GetText(cur, str);
			int val = selectedList.GetItemData(cur);
			selectedList.DeleteString(cur);
			int loc = selectedList.InsertString(0, str);
			selectedList.SetItemData(loc, val);
			selectedList.SetCurSel(loc);
		}
	}
}

void CWthrPrintDialog::OnDown() 
{
	CWaitCursor wait;
	if(selectedList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = selectedList.GetCurSel();
		if(cur < selectedList.GetCount() - 1)
		{
			selectedList.GetText(cur, str);
			int val = selectedList.GetItemData(cur);
			selectedList.DeleteString(cur);
			cur++;
			int loc = selectedList.InsertString(cur, str);
			selectedList.SetItemData(loc, val);
			selectedList.SetCurSel(loc);
		}
	}
}

void CWthrPrintDialog::OnBottom() 
{
	CWaitCursor wait;
	if(selectedList.GetCurSel() != LB_ERR)
	{
		CString str;
		int cur = selectedList.GetCurSel();
		if(cur < selectedList.GetCount() - 1)
		{
			selectedList.GetText(cur, str);
			int val = selectedList.GetItemData(cur);
			selectedList.DeleteString(cur);
			int loc = selectedList.InsertString(-1, str);
			selectedList.SetItemData(loc, val);
			selectedList.SetCurSel(loc);
		}
	}
}

void CWthrPrintDialog::OnMoveleft() 
{
	if(selectedList.GetCurSel() != LB_ERR)
		OnDblclkSelectedlist();
}

void CWthrPrintDialog::OnMoveright() 
{
	if(fieldsList.GetCurSel() != LB_ERR)
		OnDblclkFieldslist();
}

void CWthrPrintDialog::OnDblclkFieldslist() 
{
	CString str;
	int cur = fieldsList.GetCurSel();
	fieldsList.GetText(cur, str);
	int val = fieldsList.GetItemData(cur);
	int loc = selectedList.AddString(str);
	selectedList.SetItemData(loc, val);
	fieldsList.DeleteString(cur);
}

void CWthrPrintDialog::OnDblclkSelectedlist() 
{
	CString str;
	int cur = selectedList.GetCurSel();
	selectedList.GetText(cur, str);
	int val = selectedList.GetItemData(cur);
	int loc = fieldsList.AddString(str);
	fieldsList.SetItemData(loc, val);
	selectedList.DeleteString(cur);
}

void CWthrPrintDialog::OnOK() 
{
	if(selectedList.GetCount() <= 0)
	{
		AfxMessageBox("Error: At least one field must be selected.");
		return;
	}
	UpdateData();
	
	Print();

	if(wxSet && retRec >= 0)
		wxSet->SetAbsolutePosition(retRec + 1);
	else if(fireSet && retRec >= 0)
		fireSet->SetAbsolutePosition(retRec + 1);
	CDialog::OnOK();
}

void CWthrPrintDialog::Print()
{
   CDC dc;
    CPrintDialog printDlg(FALSE);

    if (printDlg.DoModal() == IDCANCEL)         // Get printer settings from user
        return;

    dc.Attach(printDlg.GetPrinterDC());         // Attach a printer DC
    dc.m_bPrinting = TRUE;

    CString strTitle;                           // Get the application title
    //strTitle.LoadString(AFX_IDS_APP_TITLE);
    strTitle.Format("Weather Data");

    DOCINFO di;                                 // Initialise print document details
    ::ZeroMemory (&di, sizeof (DOCINFO));
    di.cbSize = sizeof (DOCINFO);
    di.lpszDocName = strTitle;

    BOOL bPrintingOK = dc.StartDoc(&di);        // Begin a new print job

    // Get the printing extents and store in the m_rectDraw field of a 
    // CPrintInfo object
    CPrintInfo Info;
    Info.m_rectDraw.SetRect(0,0, 
                            dc.GetDeviceCaps(HORZRES), 
                            dc.GetDeviceCaps(VERTRES));

    OnBeginPrinting(&dc, &Info);                // Call your "Init printing" funtion
   /// for (UINT page = Info.GetMinPage(); 
    //     page <= Info.GetMaxPage() && bPrintingOK; 
    //     page++)
	UINT page = 0;
	while(bPrintingOK && Info.m_bContinuePrinting == TRUE)
    {
        dc.StartPage(); 
		page++;
		// begin new page
        Info.m_nCurPage = page;
        OnPrint(&dc, &Info);                    // Call your "Print page" function
        bPrintingOK = (dc.EndPage() > 0);       // end page
    }
    OnEndPrinting(&dc, &Info);                  // Call your "Clean up" funtion

    if (bPrintingOK)
        dc.EndDoc();                            // end a print job
    else
        dc.AbortDoc();                          // abort job.

    dc.Detach();                                // detach the printer DC

}

void CWthrPrintDialog::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	//allocate recources, figure doc length and write it to pInfo
	int wid = 0;
	if(recNums == TRUE)
		wid += 7;
	for(int f = 0; f < selectedList.GetCount(); f++)
	{
		int loc = selectedList.GetItemData(f);
		if(wxSet)
			wid += widths[loc] + 1;
		else
			wid += fireWidths[loc];
	}
	CString m = "";
	//CSize sz;
	for(int f = 0; f < wid; f++)
		m += "M";
	int points = 80;
	pFont = new CFont();
	///int h = 60, w = 80;
	pFont->CreatePointFont(points, "courier new", pDC);
	//pFont->CreateFont( pInfo->m_rectDraw.Height() / h, pInfo->m_rectDraw.Width() / w, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 
		//OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, FIXED_PITCH | TMPF_TRUETYPE | FF_MODERN, "terminal"); 
	oldFont = pDC->SelectObject(pFont);
	pDC->SetTextAlign(TA_LEFT | TA_TOP);
	//ok, need to make sure text will fit on a page....
	//check width
	sz = pDC->GetTextExtent(m);
	sz.cy += 4;
	sz.cx += 4;
	pInfo->m_rectDraw.left += sz.cx / wid;
	pInfo->m_rectDraw.right -= sz.cx / wid;
/*	while(sz.cx > pInfo->m_rectDraw.Width() && points > 10)//w < 1000)
	{
	//	h += 5;
	//	w += 10;
		points -= 10;
		pDC->SelectObject(oldFont);
		pFont->DeleteObject();
		delete pFont;
		pFont = new CFont();
		pFont->CreatePointFont(points, "terminal", pDC);
		//pFont->CreateFont( pInfo->m_rectDraw.Height() / h, pInfo->m_rectDraw.Width() / w, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 
		//OUT_TT_PRECIS, CLIP_TT_ALWAYS, DEFAULT_QUALITY, FIXED_PITCH | TMPF_TRUETYPE | FF_MODERN, "terminal");
		pDC->SelectObject(pFont);
		sz = pDC->GetTextExtent(m);
		sz.cy += 4;
		sz.cx += 4;
	}*/
	pInfo->m_rectDraw.top += sz.cy;
	if(headers)
		pInfo->m_rectDraw.top += sz.cy;
	if(pageNums == TRUE)
		pInfo->m_rectDraw.bottom -= 2 * sz.cy;
	//ok got font and adjusted print rect
	recPos = 0;
}

void CWthrPrintDialog::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CString out = "";
	int x = 4;

	if(headers == TRUE)
	{
		if(recNums)
			out += "Record ";
		for(int i = 0; i < selectedList.GetCount(); i++)
		{
			int loc = selectedList.GetItemData(i);
			if(wxSet)
				out += colNames[loc];
			else
				out += fireColNames[loc];
			out += " ";
		}
		pDC->TextOut(pInfo->m_rectDraw.left, x, out);
		CString line = "";
		for(int i = 0; i < out.GetLength(); i++)
			line += "-";
		x += sz.cy;
		pDC->TextOut(pInfo->m_rectDraw.left, x, line);
		x += sz.cy;
	}
	if(recPos == 0)
	{
		if(wxSet)
			wxSet->MoveFirst();
		else
			fireSet->MoveFirst();
		recPos++;
	}
	if(wxSet)
	{
		while(!wxSet->IsEOF() && x + sz.cy < pInfo->m_rectDraw.bottom)
		{
			out = "";
			if(recNums)
				out.Format("%6ld ", recPos);
			for(int f = 0; f < selectedList.GetCount(); f++)
			{
				CString temp, fmt;
				int loc = selectedList.GetItemData(f);
				fmt.Format("%s", colFormats[loc]);
				switch(loc)
				{
				case 0:
					temp.Format(fmt, wxSet->m_StationID);
					break;
				case 1:
					if(!wxSet->IsFieldNull(&wxSet->m_ObsDate))
						temp.Format("%02d/%02d/%4d", wxSet->m_ObsDate.GetMonth(),
						wxSet->m_ObsDate.GetDay(), wxSet->m_ObsDate.GetYear());
					else
						temp.Format(fmt, "");
					break;
				case 2:
					if(!wxSet->IsFieldNull(&wxSet->m_ObsDate))
						temp.Format("%02d:%02d", wxSet->m_ObsDate.GetHour(),
						wxSet->m_ObsDate.GetMinute());
					else
						temp.Format(fmt, "");
					break;
				case 3:
					if(!wxSet->IsFieldNull(&wxSet->m_ObsType))
						temp.Format(fmt, wxSet->m_ObsType);
					else
						temp.Format(fmt, "");
					break;
				case 4:
					if(!wxSet->IsFieldNull(&wxSet->m_SOW))
						temp.Format(fmt, wxSet->m_SOW);
					else
						temp = " ";
					break;
				case 5:
					if(!wxSet->IsFieldNull(&wxSet->m_Temp))
						temp.Format(fmt, wxSet->m_Temp);
					else
						temp = "   ";
					break;
				case 6:
					if(!wxSet->IsFieldNull(&wxSet->m_RH))
						temp.Format(fmt, max(wxSet->m_RH, 1));
					else
						temp = "   ";
					break;
				case 7:
					if(!wxSet->IsFieldNull(&wxSet->m_PPTAMT))
						temp.Format(fmt, wxSet->m_PPTAMT);
					else
						temp = "     ";
					break;
				case 8:
					if(!wxSet->IsFieldNull(&wxSet->m_PPTDUR))
						temp.Format(fmt, wxSet->m_PPTDUR);
					else
						temp = "  ";
					break;
				case 9:
					if(!wxSet->IsFieldNull(&wxSet->m_WS))
						temp.Format(fmt, wxSet->m_WS);
					else
						temp = "   ";
					break;
				case 10:
					if(!wxSet->IsFieldNull(&wxSet->m_WDir))
						temp.Format(fmt, wxSet->m_WDir);
					else
						temp = " ";
					break;
				case 11:
					if(!wxSet->IsFieldNull(&wxSet->m_WAzimuth))
						temp.Format(fmt, wxSet->m_WAzimuth);
					else
						temp = " ";
					break;
				case 12:
					if(!wxSet->IsFieldNull(&wxSet->m_TmpMax))
						temp.Format(fmt, wxSet->m_TmpMax);
					else
						temp = "   ";
					break;
				case 13:
					if(!wxSet->IsFieldNull(&wxSet->m_TmpMin))
						temp.Format(fmt, wxSet->m_TmpMin);
					else
						temp = "   ";
					break;
				case 14:
					if(!wxSet->IsFieldNull(&wxSet->m_RHMax))
						temp.Format(fmt, wxSet->m_RHMax);
					else
						temp = "   ";
					break;
				case 15:
					if(!wxSet->IsFieldNull(&wxSet->m_RHMin))
						temp.Format(fmt, max(wxSet->m_RHMin, 1));
					else
						temp = "   ";
					break;
				case 16:
					if(!wxSet->IsFieldNull(&wxSet->m_Season))
						temp.Format(fmt, wxSet->m_Season);
					else
						temp = " ";
					break;
				case 17:
					if(!wxSet->IsFieldNull(&wxSet->m_OMC10))
						temp.Format(fmt, wxSet->m_OMC10);
					else
						temp = "      ";
					break;
				case 18:
					if(!wxSet->IsFieldNull(&wxSet->m_GreenHerb))
						temp.Format(fmt, wxSet->m_GreenHerb);
					else
						temp = "    ";
					break;
				case 19:
					if(!wxSet->IsFieldNull(&wxSet->m_GreenShrub))
						temp.Format(fmt, wxSet->m_GreenShrub);
					else
						temp = "    ";
					break;
				case 20:
					if(!wxSet->IsFieldNull(&wxSet->m_WetFlag))
						temp.Format(fmt, wxSet->m_WetFlag);
					else
						temp = "       ";
					break;
				case 21:
					if(!wxSet->IsFieldNull(&wxSet->m_SolarRadiation))
						temp.Format(fmt, wxSet->m_SolarRadiation);
					else
						temp = "      ";
					break;
				case 22:
					if(!wxSet->IsFieldNull(&wxSet->m_GustSpd))
						temp.Format(fmt, wxSet->m_GustSpd);
					else
						temp = "     ";
					break;
				case 23:
					if(!wxSet->IsFieldNull(&wxSet->m_GustDir))
						temp.Format(fmt, wxSet->m_GustDir);
					else
						temp = "     ";
					break;
				case 24:
					if(!wxSet->IsFieldNull(&wxSet->m_HourlyPrecip))
						temp.Format(fmt, wxSet->m_HourlyPrecip);
					else
						temp = "      ";
					break;
				case 25:
					if(!wxSet->IsFieldNull(&wxSet->m_SR_SOW))
						temp.Format(fmt, wxSet->m_SR_SOW);
					else
						temp = "      ";
					break;
				case 26:
					if(!wxSet->IsFieldNull(&wxSet->m_SR_WetFlag))
						temp.Format(fmt, wxSet->m_SR_WetFlag);
					else
						temp = "      ";
					break;
				case 27:
					if (!wxSet->IsFieldNull(&wxSet->m_SnowFlag))
						temp.Format(fmt, wxSet->m_SnowFlag);
					else
						temp = "      ";
					break;
				case 28:
					if (!wxSet->IsFieldNull(&wxSet->m_FM1))
						temp.Format(fmt, wxSet->m_FM1);
					else
						temp = "      ";
					break;
				case 29:
					if (!wxSet->IsFieldNull(&wxSet->m_FM10))
						temp.Format(fmt, wxSet->m_FM10);
					else
						temp = "      ";
					break;
				case 30:
					if (!wxSet->IsFieldNull(&wxSet->m_FM100))
						temp.Format(fmt, wxSet->m_FM100);
					else
						temp = "      ";
					break;
				case 31:
					if (!wxSet->IsFieldNull(&wxSet->m_FM1000))
						temp.Format(fmt, wxSet->m_FM1000);
					else
						temp = "      ";
					break;
				case 32:
					if (!wxSet->IsFieldNull(&wxSet->m_FuelTemperature))
						temp.Format(fmt, wxSet->m_FM1);
					else
						temp = "      ";
					break;
				default:
					temp = "";
				}
				out += temp;
				out += " ";
			}
			//pDC->MoveTo(x, pInfo->m_rectDraw.left);
			pDC->TextOut(pInfo->m_rectDraw.left, x, out);
			x += sz.cy;
			wxSet->MoveNext();
			recPos++;
		}
	}
	else//fireSet
	{
		while(!fireSet->IsEOF() && x + sz.cy < pInfo->m_rectDraw.bottom)
		{
			out = "";
			if(recNums)
				out.Format("%6ld ", recPos);
			for(int f = 0; f < selectedList.GetCount(); f++)
			{
				CString temp, fmt;
				int loc = selectedList.GetItemData(f);
				fmt.Format("%s", fireColFormats[loc]);
				switch(loc)
				{
				case 0:
					if(!fireSet->IsFieldNull(&fireSet->m_Discovery))
						temp.Format("%02d/%02d/%4d", fireSet->m_Discovery.GetMonth(),
						fireSet->m_Discovery.GetDay(), fireSet->m_Discovery.GetYear());
					else
						temp.Format(fmt, "");
					break;
				case 1:
					if(!fireSet->IsFieldNull(&fireSet->m_Discovery))
						temp.Format("%02d:%02d", fireSet->m_Discovery.GetHour(),
						fireSet->m_Discovery.GetMinute());
					else
						temp.Format(fmt, "");
					break;
				case 2:
					if(!fireSet->IsFieldNull(&fireSet->m_TotalAcres))
						temp.Format(fmt, fireSet->m_TotalAcres);
					else
						temp = "          ";
					break;
				case 3:
					if(!fireSet->IsFieldNull(&fireSet->m_StatisticalCause))
						temp.Format(fmt, fireSet->m_StatisticalCause);
					else
						temp = "     ";
					break;
				case 4:
					if(!fireSet->IsFieldNull(&fireSet->m_FireName))
						temp.Format(fmt, fireSet->m_FireName.Trim());
					else
						temp.Format(fmt, " ");
					break;
				case 5:
					if(!fireSet->IsFieldNull(&fireSet->m_FireNumber))
						temp.Format(fmt, fireSet->m_FireNumber);
					else
						temp.Format(fmt, " ");
					break;
				default:
					temp = "";
				}
				out += temp;
				out += " ";
			}
			//pDC->MoveTo(x, pInfo->m_rectDraw.left);
			pDC->TextOut(pInfo->m_rectDraw.left, x, out);
			x += sz.cy;
			fireSet->MoveNext();
			recPos++;
		}
	}
	if(pageNums == TRUE)
	{
		CString pNum;
		pNum.Format("%d", pInfo->m_nCurPage);
		pDC->SetTextAlign(TA_TOP | TA_CENTER);
		pDC->TextOut(pInfo->m_rectDraw.left + pInfo->m_rectDraw.Width() / 2, 
			pInfo->m_rectDraw.bottom, pNum);
		pDC->SetTextAlign(TA_LEFT | TA_TOP);
	}
	if(wxSet)
	{
		if(wxSet->IsEOF())
			pInfo->m_bContinuePrinting = FALSE;
	}
	if(fireSet)
	{
		if(fireSet->IsEOF())
			pInfo->m_bContinuePrinting = FALSE;
	}
}

void CWthrPrintDialog::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	//free up allocated resources
	pDC->SelectObject(oldFont);
	pFont->DeleteObject();
	delete pFont;
}


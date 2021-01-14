// LFIPropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "LFIPropertySheet.h"


// CLFIPropertySheet

IMPLEMENT_DYNAMIC(CLFIPropertySheet, CPropertySheet)

CLFIPropertySheet::CLFIPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage, CDatabase *_pDB, CString _sigStationID)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), lfiSet(_pDB)
{
	pDB = _pDB;
	lfiSet.Open();
	lfiSet.m_strFilter.Format("[SIG_Station] = '%s'", _sigStationID);
	lfiSet.Requery();
	if(lfiSet.IsEOF())
	{
		CLFISet defSet(pDB);
		defSet.m_strFilter.Format("[SIG_Station] = '######'");
		defSet.Open();
		if(!defSet.IsEOF())
		{
			lfiSet.AddNew();
			lfiSet.m_SIG_Station = _sigStationID;
			lfiSet.m_LFIdaysAvg = defSet.m_LFIdaysAvg;
			lfiSet.m_UseVPDAvg = defSet.m_UseVPDAvg;
			lfiSet.m_TMinMin = defSet.m_TMinMin;
			lfiSet.m_TMinMax = defSet.m_TMinMax;
			lfiSet.m_VPDMin = defSet.m_VPDMin;
			lfiSet.m_VPDMax = defSet.m_VPDMax;
			lfiSet.m_DaylenMin = defSet.m_DaylenMin;
			lfiSet.m_DaylenMax = defSet.m_DaylenMax;
			lfiSet.m_PcpDays = defSet.m_PcpDays;
			lfiSet.m_HerbDaysAvg = defSet.m_HerbDaysAvg;
			lfiSet.m_HerbUseVPDAvg = defSet.m_HerbUseVPDAvg;
			lfiSet.m_HerbTMinMin = defSet.m_HerbTMinMin;
			lfiSet.m_HerbTMinMax = defSet.m_HerbTMinMax;
			lfiSet.m_HerbVPDMin = defSet.m_HerbVPDMin;
			lfiSet.m_HerbVPDMax = defSet.m_HerbVPDMax;
			lfiSet.m_HerbDaylenMin = defSet.m_HerbDaylenMin;
			lfiSet.m_HerbDaylenMax = defSet.m_HerbDaylenMax;
			lfiSet.m_HerbPcpDays = defSet.m_HerbPcpDays;
			lfiSet.m_WoodyDaysAvg = defSet.m_WoodyDaysAvg;
			lfiSet.m_WoodyUseVPDAvg = defSet.m_WoodyUseVPDAvg;
			lfiSet.m_WoodyTMinMin = defSet.m_WoodyTMinMin;
			lfiSet.m_WoodyTMinMax = defSet.m_WoodyTMinMax;
			lfiSet.m_WoodyVPDMin = defSet.m_WoodyVPDMin;
			lfiSet.m_WoodyVPDMax = defSet.m_WoodyVPDMax;
			lfiSet.m_WoodyDaylenMin = defSet.m_WoodyDaylenMin;
			lfiSet.m_WoodyDaylenMax = defSet.m_WoodyDaylenMax;
			lfiSet.m_WoodyPcpDays = defSet.m_WoodyPcpDays;
			lfiSet.m_HerbMaxGSI = defSet.m_HerbMaxGSI;
			lfiSet.m_HerbGreenup = defSet.m_HerbGreenup;
			lfiSet.m_HerbMax = defSet.m_HerbMax;
			lfiSet.m_HerbMin = defSet.m_HerbMin;
			lfiSet.m_WoodyMaxGSI = defSet.m_WoodyMaxGSI;
			lfiSet.m_WoodyGreenup = defSet.m_WoodyGreenup;
			lfiSet.m_WoodyMax = defSet.m_WoodyMax;
			lfiSet.m_WoodyMin = defSet.m_WoodyMin;
			lfiSet.Update();
		}
		else
		{
			lfiSet.AddNew();
			lfiSet.m_SIG_Station = _sigStationID;
			lfiSet.m_LFIdaysAvg = 21;
			lfiSet.m_TMinMin = -2.0;
			lfiSet.m_TMinMax = 5.0;
			lfiSet.m_VPDMin = 900.0;
			lfiSet.m_VPDMax = 4100.0;
			lfiSet.m_DaylenMin = 36000.0;
			lfiSet.m_DaylenMax = 39600.0;
			lfiSet.m_UseVPDAvg = TRUE;
			lfiSet.m_PcpDays = 30;
			//herb
			lfiSet.m_HerbDaysAvg = 21;
			lfiSet.m_HerbTMinMin = -2.0;
			lfiSet.m_HerbTMinMax = 5.0;
			lfiSet.m_HerbVPDMin = 900.0;
			lfiSet.m_HerbVPDMax = 4100.0;
			lfiSet.m_HerbDaylenMin = 36000.0;
			lfiSet.m_HerbDaylenMax = 39600.0;
			lfiSet.m_HerbUseVPDAvg = TRUE;
			lfiSet.m_HerbMaxGSI = 1.0;
			lfiSet.m_HerbGreenup = 0.5;
			lfiSet.m_HerbMax = 250.0;
			lfiSet.m_HerbMin = 30;
			lfiSet.m_HerbPcpDays = 30;
			//Woody
			lfiSet.m_WoodyDaysAvg = 21;
			lfiSet.m_WoodyTMinMin = -2.0;
			lfiSet.m_WoodyTMinMax = 5.0;
			lfiSet.m_WoodyVPDMin = 900.0;
			lfiSet.m_WoodyVPDMax = 4100.0;
			lfiSet.m_WoodyDaylenMin = 36000.0;
			lfiSet.m_WoodyDaylenMax = 39600.0;
			lfiSet.m_WoodyUseVPDAvg = TRUE;
			lfiSet.m_WoodyMaxGSI = 1.0;
			lfiSet.m_WoodyGreenup = 0.5;
			lfiSet.m_WoodyMax = 200.0;
			lfiSet.m_WoodyMin = 50;
			lfiSet.m_WoodyPcpDays = 30;
			lfiSet.Update();
		}
		defSet.Close();
		lfiSet.Requery();
	}
	generalPage.pLfiSet = &lfiSet;
	AddPage(&generalPage);
	herbPage.pLfiSet = &lfiSet;
	AddPage(&herbPage);
	woodyPage.pLfiSet = &lfiSet;
	AddPage(&woodyPage);
}

CLFIPropertySheet::~CLFIPropertySheet()
{
	lfiSet.Close();
}


BEGIN_MESSAGE_MAP(CLFIPropertySheet, CPropertySheet)
END_MESSAGE_MAP()


// CLFIPropertySheet message handlers

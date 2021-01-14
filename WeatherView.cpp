// WeatherView.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "WeatherView.h"
#include "WthrPrintDialog.h"
#include "DelRecsDialog.h"
#include "WxExportDialog.h"
#include "SIGStationSet.h"

CWxDataSource::CWxDataSource(void)
{
	records = NULL;
	totalRecs = 0;
	virRec = 0;
}

CWxDataSource::~CWxDataSource(void)
{
}

long CWxDataSource::GetNumRows()
{
	return totalRecs;
}

int CWxDataSource::GetNumCols()
{
	return 32;
}

int CWxDataSource::SetCell(int col,long row,CUGCell *cell)
{
	if(col < 0 || row < 0)
		return UG_NA;
	//write the cells new contents to the database
	if(row >= totalRecs)
		return UG_NA;
	if(!records)
		return UG_NA;
	if(virRec != row)
	{
		records->Move(row - virRec);
		virRec = row;
	}
	records->Edit();
	CString str = cell->GetText();
	str = str.Trim();
	switch(col)
	{
	case 0:
	case 1:
	case 2:
		break;
	case 3:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_SOW);
		else
			records->m_SOW = cell->GetNumber();
		break;
	case 4:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_Temp);
		else
			records->m_Temp = cell->GetNumber();;
		break;
	case 5:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_RH);
		else
			records->m_RH = max(cell->GetNumber(), 1);
		break;
	case 6:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_PPTAMT);
		else
			records->m_PPTAMT = cell->GetNumber();;
		break;
	case 7:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_PPTDUR);
		else
			records->m_PPTDUR = cell->GetNumber();;
		break;
	case 8:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_WS);
		else
			records->m_WS = cell->GetNumber();;
		break;
	case 9:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_WDir);
		else
			records->m_WDir = cell->GetNumber();;
		break;
	case 10:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_WAzimuth);
		else
			records->m_WAzimuth = cell->GetNumber();;
		break;
	case 11:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_TmpMax);
		else
			records->m_TmpMax = cell->GetNumber();;
		break;
	case 12:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_TmpMin);
		else
			records->m_TmpMin = cell->GetNumber();;
		break;
	case 13:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_RHMax);
		else
			records->m_RHMax = cell->GetNumber();;
		break;
	case 14:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_RHMin);
		else
			records->m_RHMin = max(cell->GetNumber(), 1);
		break;
	case 15:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_WetFlag);
		else
			records->m_WetFlag = cell->GetNumber();;
		break;
	case 16:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_SolarRadiation);
		else
			records->m_SolarRadiation = cell->GetNumber();;
		break;
	case 17:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_GustDir);
		else
			records->m_GustDir = cell->GetNumber();;
		break;
	case 18:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_GustSpd);
		else
			records->m_GustSpd = cell->GetNumber();;
		break;
	case 19:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_HourlyPrecip);
		else
			records->m_HourlyPrecip = cell->GetNumber();;
		break;
	case 20:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_OMC10);
		else
			records->m_OMC10 = cell->GetNumber();;
		break;
	case 21:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_Season);
		else
			records->m_Season = cell->GetNumber();;
		break;
	case 22:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_GreenHerb);
		else
			records->m_GreenHerb = cell->GetNumber();;
		break;
	case 23:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_GreenShrub);
		else
			records->m_GreenShrub = cell->GetNumber();;
		break;
	case 24:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_SR_SOW);
		else
			records->m_SR_SOW = cell->GetNumber();;
		break;
	case 25:
		if(str.GetLength() <= 0)
			records->SetFieldNull(&records->m_SR_WetFlag);
		else
			records->m_SR_WetFlag = cell->GetNumber();;
		break;
	case 26:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_SnowFlag);
		else
			records->m_SnowFlag = cell->GetNumber();;
		break;
	case 27:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FM1);
		else
			records->m_FM1 = cell->GetNumber();;
		break;
	case 28:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FM10);
		else
			records->m_FM10 = cell->GetNumber();;
		break;
	case 29:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FM100);
		else
			records->m_FM100 = cell->GetNumber();;
		break;
	case 30:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FM1000);
		else
			records->m_FM1000 = cell->GetNumber();;
		break;
	case 31:
		if (str.GetLength() <= 0)
			records->SetFieldNull(&records->m_FuelTemperature);
		else
			records->m_FuelTemperature = cell->GetNumber();;
		break;
	}

	/* 09/2012 add functionality to set DailyObs field */
	//another wonderful case where can create > 1 daily obs per day
	//so Stu comments this out
	/*CSIGStationSet stnSet(records->m_pDatabase);
	stnSet.Open();

	// look for station record
	stnSet.m_strFilter.Format("[StationID] = '%6.6s'", records->m_StationID);
	stnSet.Requery();

	int regHour = 13;

	if(! stnSet.IsEOF())
	{
		regHour = stnSet.m_RegSchdObs;
	}
	stnSet.Close();

	records->m_DailyObs = 0;
	if (records->m_ObsType.CompareNoCase("O") == 0)
		records->m_DailyObs = 1;
	else
		if ((records->m_ObsType.CompareNoCase("R") == 0)
			&& (records->m_Hour == regHour)
			&& (! records->IsFieldNull(&records->m_SOW)))
				records->m_DailyObs = 1;
	*/
	records->Update();

	return UG_SUCCESS;
}

int	CWxDataSource::GetCell(int col,long row,CUGCell *cell)
{
	if(!records)
		return UG_SUCCESS;
	CString str;
	COLORREF backColor = RGB(250, 250, 250);
	COLORREF backColorDailyObs = RGB(230, 230, 230);
	COLORREF backColorBad = RGB(250, 210, 210);
	if(row < 0)
	{
		switch(col)
		{
		case 0:
			cell->SetText("StationID");
			return UG_SUCCESS;
		case 1:
			cell->SetText("ObsDate");
			return UG_SUCCESS;
		case 2:
			cell->SetText("Type");
			return UG_SUCCESS;
		case 3:
			cell->SetText("SOW");
			return UG_SUCCESS;
		case 4:
			cell->SetText("Temp(F)");
			return UG_SUCCESS;
		case 5:
			cell->SetText("RH");
			return UG_SUCCESS;
		case 6:
			cell->SetText("24hr Precip");
			return UG_SUCCESS;
		case 7:
			cell->SetText("Duration");
			return UG_SUCCESS;
		case 8:
			cell->SetText("Wind Speed");
			return UG_SUCCESS;
		case 9:
			cell->SetText("Direction");
			return UG_SUCCESS;
		case 10:
			cell->SetText("Azimuth");
			return UG_SUCCESS;
		case 11:
			cell->SetText("Max Temp");
			return UG_SUCCESS;
		case 12:
			cell->SetText("Min Temp");
			return UG_SUCCESS;
		case 13:
			cell->SetText("Max RH");
			return UG_SUCCESS;
		case 14:
			cell->SetText("Min RH");
			return UG_SUCCESS;
		case 15:
			cell->SetText("WetFlag");
			return UG_SUCCESS;
		case 16:
			cell->SetText("SolarRad");
			return UG_SUCCESS;
		case 17:
			cell->SetText("Gust Dir");
			return UG_SUCCESS;
		case 18:
			cell->SetText("Gust Speed");
			return UG_SUCCESS;
		case 19:
			cell->SetText("Hourly Precip");
			return UG_SUCCESS;
		case 20:
			cell->SetText("OMC 10");
			return UG_SUCCESS;
		case 21:
			cell->SetText("Season");
			return UG_SUCCESS;
		case 22:
			cell->SetText("GreenHerb");
			return UG_SUCCESS;
		case 23:
			cell->SetText("GreenShrub");
			return UG_SUCCESS;
		case 24:
			cell->SetText("SR_SOW");
			return UG_SUCCESS;
		case 25:
			cell->SetText("SR_WetFlag");
			return UG_SUCCESS;
		case 26:
			cell->SetText("SnowFlag");
			return UG_SUCCESS;
		case 27:
			cell->SetText("1 Hour DFM");
			return UG_SUCCESS;
		case 28:
			cell->SetText("10 Hour DFM");
			return UG_SUCCESS;
		case 29:
			cell->SetText("100 Hour DFM");
			return UG_SUCCESS;
		case 30:
			cell->SetText("1000 Hour DFM");
			return UG_SUCCESS;
		case 31:
			cell->SetText("FuelTemperature");
			return UG_SUCCESS;
		default:
			cell->SetText("");
			return UG_SUCCESS;
		}
	}
	if(records->IsBOF() && records->IsEOF())
		return UG_SUCCESS;
	if(row == 0)
		records->MoveFirst();
	else if(row == totalRecs - 1)
		records->MoveLast();
	else if(row != virRec)
		records->Move(row - virRec);
	COLORREF selColor;
	if (col > 0 && records->m_DailyObs == 1)
		selColor = backColorDailyObs;
	else
		selColor = backColor;
	switch(col)
	{
	case -1:
		cell->SetNumber(row + 1);
		break;
	case 0:
		if(!records->IsFieldNull(&records->m_StationID))
			cell->SetText(records->m_StationID);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 1:
		if(!records->IsFieldNull(&records->m_ObsDate))
		{  // if (!records->m_ObsType.CompareNoCase("o"))
		   //    str = records->m_ObsDate.Format("%m/%d/%y");
		   // else
			   str = records->m_ObsDate.Format("%m/%d/%y %H:%M");
			cell->SetText(str);
		}
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 2:
		if(!records->IsFieldNull(&records->m_ObsType))
			cell->SetText(records->m_ObsType);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 3:
		if(!records->IsFieldNull(&records->m_SOW))
			cell->SetNumber(records->m_SOW);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 4:
		if (!records->IsFieldNull(&records->m_Temp))
		{
			cell->SetNumber(records->m_Temp);
			cell->SetBackColor(selColor);
		}
		else
		{
			cell->SetText("");
			cell->SetBackColor(backColorBad);
		}
		break;
	case 5:
		if(!records->IsFieldNull(&records->m_RH))
		{
			cell->SetNumber(max(records->m_RH, 1));
			cell->SetBackColor(selColor);
		}
		else
		{
			cell->SetText("");
			cell->SetBackColor(backColorBad);
		}
		break;
	case 6:
		if(!records->IsFieldNull(&records->m_PPTAMT))
		{
			cell->SetNumber(records->m_PPTAMT);
			cell->SetBackColor(selColor);
		}
		else
		{
			cell->SetText("");
			cell->SetBackColor(backColorBad);
		}
		break;
	case 7:
		if(!records->IsFieldNull(&records->m_PPTDUR))
			cell->SetNumber(records->m_PPTDUR);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 8:
		if(!records->IsFieldNull(&records->m_WS))
		{
			cell->SetNumber(records->m_WS);
			cell->SetBackColor(selColor);
		}
		else
		{
			cell->SetText("");
			cell->SetBackColor(backColorBad);
		}
		break;
	case 9:
		if(!records->IsFieldNull(&records->m_WDir))
			cell->SetNumber(records->m_WDir);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 10:
		if(!records->IsFieldNull(&records->m_WAzimuth))
			cell->SetNumber(records->m_WAzimuth);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 11:
		if(!records->IsFieldNull(&records->m_TmpMax))
		{
			cell->SetNumber(records->m_TmpMax);
			cell->SetBackColor(selColor);
		}
		else
		{
			cell->SetText("");
			cell->SetBackColor(backColorBad);
		}
		break;
	case 12:
		if(!records->IsFieldNull(&records->m_TmpMin))
		{
			cell->SetNumber(records->m_TmpMin);
			cell->SetBackColor(selColor);
		}
		else
		{
			cell->SetText("");
			cell->SetBackColor(backColorBad);
		}
		break;
	case 13:
		if(!records->IsFieldNull(&records->m_RHMax))
			cell->SetNumber(records->m_RHMax);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 14:
		if(!records->IsFieldNull(&records->m_RHMin))
		{
			cell->SetNumber(max(records->m_RHMin, 1));
			cell->SetBackColor(selColor);
		}
		else
		{
			cell->SetText("");
			cell->SetBackColor(backColorBad);
		}
		break;
	case 15:
		if(!records->IsFieldNull(&records->m_WetFlag))
			cell->SetNumber(records->m_WetFlag);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 16:
		if (!records->IsFieldNull(&records->m_SolarRadiation))
		{
			cell->SetNumber(records->m_SolarRadiation);
			cell->SetBackColor(selColor);
		}
		else
		{
			cell->SetText("");
			cell->SetBackColor(backColorBad);
		}
		break;
	case 17:
		if(!records->IsFieldNull(&records->m_GustDir))
			cell->SetNumber(records->m_GustDir);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 18:
		if(!records->IsFieldNull(&records->m_GustSpd))
			cell->SetNumber(records->m_GustSpd);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 19:
		if (!records->IsFieldNull(&records->m_HourlyPrecip))
		{
			cell->SetNumber(records->m_HourlyPrecip);
			cell->SetBackColor(selColor);
		}
		else
		{
			cell->SetText("");
			cell->SetBackColor(backColorBad);
		}
		break;
	case 20:
		if(!records->IsFieldNull(&records->m_OMC10))
			cell->SetNumber(records->m_OMC10);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 21:
		if(!records->IsFieldNull(&records->m_Season))
			cell->SetNumber(records->m_Season);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 22:
		if(!records->IsFieldNull(&records->m_GreenHerb))
			cell->SetNumber(records->m_GreenHerb);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 23:
		if(!records->IsFieldNull(&records->m_GreenShrub))
			cell->SetNumber(records->m_GreenShrub);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 24:
		if(!records->IsFieldNull(&records->m_SR_SOW))
			cell->SetNumber(records->m_SR_SOW);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 25:
		if(!records->IsFieldNull(&records->m_SR_WetFlag))
			cell->SetNumber(records->m_SR_WetFlag);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 26:
		if (!records->IsFieldNull(&records->m_SnowFlag))
		{
			cell->SetNumber(records->m_SnowFlag);
			cell->SetBackColor(selColor);
		}
		else
		{
			cell->SetText("");
			cell->SetBackColor(backColorBad);
		}
		break;
	case 27:
		if (!records->IsFieldNull(&records->m_FM1))
			cell->SetNumber(records->m_FM1);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 28:
		if (!records->IsFieldNull(&records->m_FM10))
			cell->SetNumber(records->m_FM10);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 29:
		if (!records->IsFieldNull(&records->m_FM100))
			cell->SetNumber(records->m_FM100);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 30:
		if (!records->IsFieldNull(&records->m_FM1000))
			cell->SetNumber(records->m_FM1000);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	case 31:
		if (!records->IsFieldNull(&records->m_FuelTemperature))
			cell->SetNumber(records->m_FuelTemperature);
		else
			cell->SetText("");
		cell->SetBackColor(selColor);
		break;
	}
	virRec = row;
	return UG_SUCCESS;
}

void CWxDataSource::SetRecords(CWxSet *_records)
{
	records = _records;
	totalRecs = 0;
	virRec = 0;
	if(!records->IsBOF() && !records->IsEOF())
	{
		records->MoveFirst();
		while(!records->IsEOF())
		{
			totalRecs++;
			records->MoveNext();
		}
		if(totalRecs > 0)
			records->MoveFirst();
	}
}

void CWxDataSource::SortAscending(int col)
{
	switch(col)
	{
	case 0:
		records->m_strSort = "[StationID],[ObsDate]";
		break;
	case 1:
		records->m_strSort = "[ObsDate],[StationID]";
		break;
	case 2:
		records->m_strSort = "[ObsType],[ObsDate],[StationID]";
		break;
	case 3:
		records->m_strSort = "[SOW],[ObsDate],[StationID]";
		break;
	case 4:
		records->m_strSort = "[Temp],[ObsDate],[StationID]";
		break;
	case 5:
		records->m_strSort = "[RH],[ObsDate],[StationID]";
		break;
	case 6:
		records->m_strSort = "[PPTAMT],[ObsDate],[StationID]";
		break;
	case 7:
		records->m_strSort = "[PPTDUR],[ObsDate],[StationID]";
		break;
	case 8:
		records->m_strSort = "[WS],[ObsDate],[StationID]";
		break;
	case 9:
		records->m_strSort = "[WDir],[ObsDate],[StationID]";
		break;
	case 10:
		records->m_strSort = "[CORCT2],[ObsDate],[StationID]";
		break;
	case 11:
		records->m_strSort = "[TmpMax],[ObsDate],[StationID]";
		break;
	case 12:
		records->m_strSort = "[TmpMin],[ObsDate],[StationID]";
		break;
	case 13:
		records->m_strSort = "[RHMax],[ObsDate],[StationID]";
		break;
	case 14:
		records->m_strSort = "[RHMin],[ObsDate],[StationID]";
		break;
	case 15:
		records->m_strSort = "[WetFlag],[ObsDate],[StationID]";
		break;
	case 16:
		records->m_strSort = "[SolarRadiation],[ObsDate],[StationID]";
		break;
	case 17:
		records->m_strSort = "[GustDir],[ObsDate],[StationID]";
		break;
	case 18:
		records->m_strSort = "[GustSpd],[ObsDate],[StationID]";
		break;
	case 19:
		records->m_strSort = "[HourlyPrecip],[ObsDate],[StationID]";
		break;
	case 20:
		records->m_strSort = "[OMC10],[ObsDate],[StationID]";
		break;
	case 21:
		records->m_strSort = "[Season],[ObsDate],[StationID]";
		break;
	case 22:
		records->m_strSort = "[GreenHerb],[ObsDate],[StationID]";
		break;
	case 23:
		records->m_strSort = "[GreenShrub],[ObsDate],[StationID]";
		break;
	case 24:
		records->m_strSort = "[SR_SOW],[ObsDate],[StationID]";
		break;
	case 25:
		records->m_strSort = "[SR_WetFlag],[ObsDate],[StationID]";
		break;
	case 26:
		records->m_strSort = "[SnowFlag],[ObsDate],[StationID]";
		break;
	case 27:
		records->m_strSort = "[FM_1],[ObsDate],[StationID]";
		break;
	case 28:
		records->m_strSort = "[FM_10],[ObsDate],[StationID]";
		break;
	case 29:
		records->m_strSort = "[FM_100],[ObsDate],[StationID]";
		break;
	case 30:
		records->m_strSort = "[FM_1000],[ObsDate],[StationID]";
		break;
	case 31:
		records->m_strSort = "[FuelTemperature],[ObsDate],[StationID]";
		break;
	}
	records->Requery();
	virRec = 0;
}

void CWxDataSource::SortDescending(int col)
{
	switch(col)
	{
	case 0:
		records->m_strSort = "[StationID] DESC,[ObsDate]";
		break;
	case 1:
		records->m_strSort = "[ObsDate] DESC,[StationID]";
		break;
	case 2:
		records->m_strSort = "[ObsType] DESC,[ObsDate],[StationID]";
		break;
	case 3:
		records->m_strSort = "[SOW] DESC,[ObsDate],[StationID]";
		break;
	case 4:
		records->m_strSort = "[Temp] DESC,[ObsDate],[StationID]";
		break;
	case 5:
		records->m_strSort = "[RH] DESC,[ObsDate],[StationID]";
		break;
	case 6:
		records->m_strSort = "[PPTAMT] DESC,[ObsDate],[StationID]";
		break;
	case 7:
		records->m_strSort = "[PPTDUR] DESC,[ObsDate],[StationID]";
		break;
	case 8:
		records->m_strSort = "[WS] DESC,[ObsDate],[StationID]";
		break;
	case 9:
		records->m_strSort = "[WDir] DESC,[ObsDate],[StationID]";
		break;
	case 10:
		records->m_strSort = "[CORCT2] DESC,[ObsDate],[StationID]";
		break;
	case 11:
		records->m_strSort = "[TmpMax] DESC,[ObsDate],[StationID]";
		break;
	case 12:
		records->m_strSort = "[TmpMin] DESC,[ObsDate],[StationID]";
		break;
	case 13:
		records->m_strSort = "[RHMax] DESC,[ObsDate],[StationID]";
		break;
	case 14:
		records->m_strSort = "[RHMin] DESC,[ObsDate],[StationID]";
		break;
	case 15:
		records->m_strSort = "[WetFlag] DESC,[ObsDate],[StationID]";
		break;
	case 16:
		records->m_strSort = "[SolarRadiation] DESC,[ObsDate],[StationID]";
		break;
	case 17:
		records->m_strSort = "[GustDir] DESC,[ObsDate],[StationID]";
		break;
	case 18:
		records->m_strSort = "[GustSpd] DESC,[ObsDate],[StationID]";
		break;
	case 19:
		records->m_strSort = "[HourlyPrecip] DESC,[ObsDate],[StationID]";
		break;
	case 20:
		records->m_strSort = "[OMC10] DESC,[ObsDate],[StationID]";
		break;
	case 21:
		records->m_strSort = "[Season] DESC,[ObsDate],[StationID]";
		break;
	case 22:
		records->m_strSort = "[GreenHerb] DESC,[ObsDate],[StationID]";
		break;
	case 23:
		records->m_strSort = "[GreenShrub] DESC,[ObsDate],[StationID]";
		break;
	case 24:
		records->m_strSort = "[SR_SOW] DESC,[ObsDate],[StationID]";
		break;
	case 25:
		records->m_strSort = "[SR_WetFlag] DESC,[ObsDate],[StationID]";
		break;
	case 26:
		records->m_strSort = "[SnowFlag] DESC,[ObsDate],[StationID]";
		break;
	case 27:
		records->m_strSort = "[FM_1] DESC,[ObsDate],[StationID]";
		break;
	case 28:
		records->m_strSort = "[FM_10] DESC,[ObsDate],[StationID]";
		break;
	case 29:
		records->m_strSort = "[FM_100] DESC,[ObsDate],[StationID]";
		break;
	case 30:
		records->m_strSort = "[FM_1000] DESC,[ObsDate],[StationID]";
		break;
	case 31:
		records->m_strSort = "[FuelTemperature] DESC,[ObsDate],[StationID]";
		break;
	}
	records->Requery();
	virRec = 0;
}

WxCUG::WxCUG()
{
}

WxCUG::~WxCUG()
{
}

void WxCUG::OnSetup()
{
	CRect rect;
	GetClientRect(&rect);
	SetCurrentCellMode(2);
	SetNumberCols(32);
	SetUniformRowHeight(TRUE);
	SetDoubleBufferMode(TRUE);
	SetVScrollMode(UG_SCROLLTRACKING);
	int wid = rect.Width() / 25;
	SetColWidth(-1, wid * 4);
	SetColWidth(0, wid * 3);
	SetColWidth(1, wid * 4.5);
	SetColWidth(2, wid * 1.8);
	SetColWidth(3, wid * 1.8);
	SetColWidth(4, wid * 3);
	SetColWidth(5, wid * 2);
	SetColWidth(6, wid * 3.7);
	SetColWidth(7, wid * 3);
	SetColWidth(8, wid * 4.2);
	SetColWidth(9, wid * 4);
	SetColWidth(10, wid * 3);
	SetColWidth(11, wid * 4);
	SetColWidth(12, wid * 4);
	SetColWidth(13, wid * 3);
	SetColWidth(14, wid * 3);
	SetColWidth(15, wid * 2.8);
	SetColWidth(16, wid * 3.5);
	SetColWidth(17, wid * 5);
	SetColWidth(18, wid * 5);
	SetColWidth(19, wid * 4);
	SetColWidth(20, wid * 3);
	SetColWidth(21, wid * 3);
	SetColWidth(22, wid * 4);
	SetColWidth(23, wid * 4);
	SetColWidth(24, wid * 2.8);
	SetColWidth(25, wid * 2.8);
	SetColWidth(26, wid * 2.8);
	SetColWidth(27, wid * 2.8);
	SetColWidth(28, wid * 2.8);
	SetColWidth(29, wid * 2.8);
	SetColWidth(30, wid * 2.8);
	SetColWidth(31, wid * 2.8);

	CUGCell cell;
	GetColDefault( 0, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 0, &cell );
	GetColDefault( 1, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 1, &cell );
	GetColDefault( 2, &cell );
	cell.SetReadOnly(TRUE);
	SetColDefault( 2, &cell );
	GetColDefault( 3, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 3, &cell );
	GetColDefault( 4, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 4, &cell );
	GetColDefault( 5, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 5, &cell );
	GetColDefault( 6, &cell );
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 6, &cell );
	GetColDefault( 7, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 7, &cell );
	GetColDefault( 8, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 8, &cell );
	GetColDefault( 9, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 9, &cell );
	GetColDefault( 10, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 10, &cell );
	GetColDefault( 11, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 11, &cell );
	GetColDefault( 12, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 12, &cell );
	GetColDefault( 13, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 13, &cell );
	GetColDefault( 14, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 14, &cell );
	GetColDefault( 15, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 15, &cell );
	GetColDefault( 16, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 16, &cell );
	GetColDefault( 17, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 17, &cell );
	GetColDefault( 18, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 18, &cell );

	GetColDefault( 19, &cell );
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 19, &cell );

    GetColDefault( 20, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 20, &cell );
    GetColDefault( 21, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 21, &cell );

    GetColDefault( 22, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 22, &cell );

	GetColDefault( 23, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 23, &cell );

	GetColDefault( 24, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 24, &cell );

	GetColDefault( 25, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 25, &cell );

	GetColDefault( 26, &cell );
	cell.SetNumberDecimals(0);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault( 26, &cell );

	GetColDefault(27, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(27, &cell);
	GetColDefault(28, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(28, &cell);
	GetColDefault(29, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(29, &cell);
	GetColDefault(30, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(30, &cell);
	GetColDefault(31, &cell);
	cell.SetNumberDecimals(2);
	//cell.SetParam(USE_COXNUMBER);
	SetColDefault(31, &cell);
	LockColumns(3);

	int index = AddDataSource(&m_data);
	SetDefDataSource(index);
	SetGridUsingDataSource(index);

	EnableMenu(TRUE);
}

void WxCUG::OnDrawFocusRect(CDC *dc,RECT *rect)
{
	//DrawExcelFocusRect(dc,rect);

	rect->bottom --;
	rect->right --;
	dc->DrawFocusRect(rect);
}

COLORREF WxCUG::OnGetDefBackColor(int section)
{
	if(section == UG_GRID)
		return GetSysColor(COLOR_BTNFACE);
	else
		return GetSysColor(COLOR_BTNFACE);
}

void WxCUG::OnMenuCommand(int col,long row,int section,int item)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 0)
	{
		if (item == ID_SORT_SORTASCENDING)
		{
			m_data.SortAscending(col);//(records);
			GotoRow(0);
			RedrawAll();
		}
		else if(item == ID_SORT_SORTDESCENDING)
		{
			m_data.SortDescending(col);
			GotoRow(0);
			RedrawAll();
		}
	}
}

int  WxCUG::OnMenuStart(int col,long row,int section)
{
	if (section == UG_GRID || section == UG_TOPHEADING && col >= 0)
	{
		//****** Empty the Menu!!
		EmptyMenu();

		//******* Add the Menu Items
		AddMenuItem(ID_SORT_SORTASCENDING,"Sort &Ascending");
		AddMenuItem(ID_SORT_SORTDESCENDING, "Sort &Descending");
	}
	return TRUE;
}

void WxCUG::OnDClicked(int col,long row,RECT *rect,POINT *point,BOOL processed)
{
	UNREFERENCED_PARAMETER(col);
	UNREFERENCED_PARAMETER(row);
	UNREFERENCED_PARAMETER(*rect);
	UNREFERENCED_PARAMETER(*point);
	UNREFERENCED_PARAMETER(processed);
	StartEdit();
}

void WxCUG::OnCharDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);
	StartEdit( *vcKey );
}

void WxCUG::OnKeyDown(UINT *vcKey,BOOL processed)
{
	UNREFERENCED_PARAMETER(processed);

	if ( *vcKey == VK_TAB )
	{
		int col = GetCurrentCol();
		long row = GetCurrentRow();

		if ( GetKeyState( VK_SHIFT ) < 0 )
		{
			col --;
			if ( col < 0 )
			{
				col = GetNumberCols() - 1;
				row--;
			}
		}
		else
		{
			col ++;
			if ( col >= GetNumberCols())
			{
				col = 0;
				row++;
			}
		}
		GotoCell( col, row );
	}
}

int WxCUG::OnEditVerify(int col, long row,CWnd *edit,UINT *vcKey)
{
	// change space to backspace
    if (vcKey)
	   if (*vcKey==32)
          *vcKey = 8;

	return CUGCtrl::OnEditVerify(col,row,edit,vcKey);
}

int WxCUG::OnEditStart(int col, long row,CWnd **edit)
{
	CUGCell cell;
	GetCellIndirect( col, row, &cell );
	DWORD dwStyle = 0;
	if ( cell.GetParam() == USE_COXNUMBER )
	{
		if ( ! m_cutNumeric.GetSafeHwnd())
		{
			m_cutNumeric.Create( WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), this, COXMASK_ID );
			m_cutNumeric.m_ctrl = this;
		}
		m_cutNumeric.SetDecimalSeparator( '.' );
		m_cutNumeric.SetGroupSeparator( ',' );
		m_cutNumeric.SetShowLeadingZero(FALSE);
		if(col == 6)//precip
			m_cutNumeric.SetFractionalDigitCount(2);
		else if(col == 19)
			m_cutNumeric.SetFractionalDigitCount(2);
		else
			m_cutNumeric.SetFractionalDigitCount(0);
		switch(col)
		{
		case 3:
		case 9:
		case 15:
		case 24:
		case 25:
		case 26:
			m_cutNumeric.SetDecimalDigitCount(1);
			break;
		case 4:
		case 5:
		case 6:
		case 8:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 16:
		case 17:
		case 18:
			m_cutNumeric.SetDecimalDigitCount(3);
			break;
		case 7:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
			m_cutNumeric.SetDecimalDigitCount(2);
			break;
		}
		*edit = &m_cutNumeric;
	}
	return TRUE;
}

// CWeatherView

IMPLEMENT_DYNCREATE(CWeatherView, CFormView)

CWeatherView::CWeatherView()
	: CFormView(CWeatherView::IDD)
{
	records = NULL;
}

CWeatherView::~CWeatherView()
{
}

void CWeatherView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWeatherView, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(ID_DATA_EXPORT, &CWeatherView::OnDataExport)
	ON_COMMAND(ID_DATA_PRINT, &CWeatherView::OnDataPrint)
	ON_COMMAND(ID_DATA_DELETERECORDS, &CWeatherView::OnDataDeleterecords)
	ON_UPDATE_COMMAND_UI(ID_DATA_DELETERECORDS, &CWeatherView::OnUpdateDataDeleterecords)
	ON_UPDATE_COMMAND_UI(ID_DATA_EXPORT, &CWeatherView::OnUpdateDataExport)
	ON_UPDATE_COMMAND_UI(ID_DATA_PRINT, &CWeatherView::OnUpdateDataPrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &CWeatherView::OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CWeatherView::OnUpdateFileSaveAs)
END_MESSAGE_MAP()

// CWeatherView diagnostics

#ifdef _DEBUG
void CWeatherView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CWeatherView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// CWeatherView message handlers

void CWeatherView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	CWaitCursor wait;
	if(m_grid.m_hWnd)
	{
		//CRect trect;
		//int a = trect.Height() + 1, b = cy - 2 - trect.Height();
		//if(b <= 0)
			//b = 1;
		m_grid.MoveWindow(1, 1, cx - 2, cy - 2);
	}
}

void CWeatherView::OnInitialUpdate()
{
	virRec = 0;
	totalRecs = 0;
	if(records)
		m_grid.m_data.SetRecords(records);
	CFormView::OnInitialUpdate();
	//m_grid.BestFit(1, m_grid.GetNumberCols() - 1, 1, UG_BESTFIT_TOPHEADINGS);
	rbCol = 1;
	if(records)
		records->MoveFirst();
	//m_grid.AttachGrid(this, IDC_GRID);
	CRect rect;
	GetClientRect(&rect);
	OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
}

void CWeatherView::OnDataExport()
{
	CWxExportDialog wed(this, records);
	wed.DoModal();
	m_grid.RedrawAll();
}

void CWeatherView::OnDataPrint()
{
	CWthrPrintDialog wpd(this, records);
	wpd.DoModal();
	m_grid.RedrawAll();
}

void CWeatherView::OnDataDeleterecords()
{
	CDelRecsDialog drd(this, records, m_grid.m_data.GetNumRows(), m_grid.GetCurrentRow() + 1, FALSE);
	if(drd.DoModal() == IDOK)
	{
		long start, end;
		start = drd.m_start;//startCtl.GetValueInteger();
		end = drd.m_end;//endCtl.GetValueInteger();
		if(end < start)
		{
			//AfxMessageBox("ERROR: End record can not be less than start record.");
			return;
		}
		if(end > m_grid.m_data.GetNumRows())
			end = m_grid.m_data.GetNumRows();
		records->SetAbsolutePosition(start);
		records->Delete();
		for(int r = 0; r < end - start && !records->IsEOF(); r++)
		{
			records->MoveNext();
			if(!records->IsEOF())
				records->Delete();
		}

		//reset table contents
		m_grid.SetNumberRows(0);
		CWaitCursor wait;
		totalRecs = 0;
		virRec = 0;
		m_grid.m_data.SetRecords(records);
		int index = m_grid.GetDefDataSource();
		m_grid.SetGridUsingDataSource(index);
	}
	m_grid.RedrawAll();
}

void CWeatherView::OnUpdateDataDeleterecords(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_grid.GetNumberRows() > 0) ? 1 : 0);
}

void CWeatherView::OnUpdateDataExport(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_grid.GetNumberRows() > 0) ? 1 : 0);
}

void CWeatherView::OnUpdateDataPrint(CCmdUI *pCmdUI)
{
	pCmdUI->Enable((m_grid.GetNumberRows() > 0) ? 1 : 0);
}

void CWeatherView::SetRecords(CWxSet *_records)
{
	records = _records;
	virRec = 0;
	totalRecs = 0;
	if(records)
		m_grid.m_data.SetRecords(records);
	//CFormView::OnInitialUpdate();

	rbCol = 1;
	if(records)
		records->MoveFirst();
	m_grid.AttachGrid(this, IDC_GRID);
	m_grid.BestFit(0, m_grid.GetNumberCols() - 1, 2, UG_BESTFIT_TOPHEADINGS);

	CRect rect;
	GetClientRect(&rect);
	OnSize(SIZE_RESTORED, rect.Width(), rect.Height());
}

void CWeatherView::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}

void CWeatherView::OnUpdateFileSaveAs(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
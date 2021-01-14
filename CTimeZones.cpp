#include "CTimeZones.h"

using namespace std;

CTimeZone::CTimeZone()
{
	m_abbrv = "";
	m_name = "";
	m_offset = 0;
}
CTimeZone::CTimeZone(const CTimeZone& rhs)
{
	m_abbrv = rhs.m_abbrv;
	m_name = rhs.m_name;
	m_offset = rhs.m_offset;
}

CTimeZone::~CTimeZone()
{

}


CTimeZones::CTimeZones()
{
	//add the time zones we care about...
	CTimeZone thisTz;
	thisTz.m_name = "Eastern Standard Time";
	thisTz.m_abbrv = "EST";
	thisTz.m_offset = -5;
	m_timezones.push_back(thisTz);
	thisTz.m_name = "Central Standard Time";
	thisTz.m_abbrv = "CST";
	thisTz.m_offset = -6;
	m_timezones.push_back(thisTz);
	thisTz.m_name = "Mountain Standard Time";
	thisTz.m_abbrv = "MST";
	thisTz.m_offset = -7;
	m_timezones.push_back(thisTz);
	thisTz.m_name = "Pacific Standard Time";
	thisTz.m_abbrv = "PST";
	thisTz.m_offset = -8;
	m_timezones.push_back(thisTz);
	thisTz.m_name = "Hawaii-Aleutian Standard Time";
	thisTz.m_abbrv = "HST";
	thisTz.m_offset = -10;
	m_timezones.push_back(thisTz);
	thisTz.m_name = "Alaska Standard Time";
	thisTz.m_abbrv = "AKST";
	thisTz.m_offset = -9;
	m_timezones.push_back(thisTz);
	thisTz.m_name = "Atlantic Standard Time";
	thisTz.m_abbrv = "AST";
	thisTz.m_offset = -4;
	m_timezones.push_back(thisTz);
	thisTz.m_name = "Chamorro Standard Time";
	thisTz.m_abbrv = "CHST";
	thisTz.m_offset = 10;
	m_timezones.push_back(thisTz);
	thisTz.m_name = "Samoa Standard Time";
	thisTz.m_abbrv = "SST";
	thisTz.m_offset = -11;
	m_timezones.push_back(thisTz);

}

CTimeZones::~CTimeZones()
{

}

CTimeZone CTimeZones::GetTimeZone(std::string tzAbbrev)
{
	if (tzAbbrev.size() > 0)
	{
		//ideal world the correct abbreviation is there
		for (vector<CTimeZone>::iterator it = m_timezones.begin(); it != m_timezones.end(); ++it)
		{
			if (tzAbbrev.compare((*it).m_abbrv) == 0)
				return (*it);
		}
		//pick the best fit based on first character
		for (vector<CTimeZone>::iterator it = m_timezones.begin(); it != m_timezones.end(); ++it)
		{
			if (tzAbbrev[0] == (*it).m_abbrv[0])
				return (*it);
		}
	}
	//default to something!!!!
	return m_timezones[0];
}

CTimeZone CTimeZones::GetTimeZone(int offset)
{
	for (vector<CTimeZone>::iterator it = m_timezones.begin(); it != m_timezones.end(); ++it)
	{
		if ((*it).m_offset == offset)
			return (*it);
	}
	//failed, default to something!!!!
	return m_timezones[0];
}

CTimeZone CTimeZones::GetTimeZoneFromState(std::string state)
{//cludge to add a default timezone when NULL in WxStation
	if (state.size() > 0)
	{
		if (state == "AK")
			return GetTimeZone("AKST");
		else if (state == "HI")
			return GetTimeZone("HST");
		else if (state == "CA" || state == "WA" || state == "OR" || state == "NV")
			return GetTimeZone("PST");
		else if (state == "MT" || state == "AZ" || state == "WY" || state == "CO" || state == "UT" || state == "NM" || state == "SD"
			|| state == "ID")
			return GetTimeZone("MST");
		else if (state == "MN" || state == "IA" || state == "NE" || state == "KS" || state == "OK" || state == "TX" || state == "LA"
			|| state == "AR" || state == "MS" || state == "AL" || state == "IL" || state == "MO" || state == "WI" || state == "ND"
			|| state == "")
			return GetTimeZone("CST");
		else if (state == "GU")
			return GetTimeZone("CHST");
		else if (state == "PR")
			return GetTimeZone("AST");
	}
	return GetTimeZone("EST");
}

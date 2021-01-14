#pragma once
#include <string>
#include <vector>

class CTimeZone
{
public:
	CTimeZone();
	CTimeZone(const CTimeZone& rhs);
	~CTimeZone();
	std::string m_abbrv;
	std::string m_name;
	int m_offset;
};

class CTimeZones
{
public:
	CTimeZones();
	~CTimeZones();
	CTimeZone GetTimeZone(std::string tzAbbrev);
	CTimeZone GetTimeZone(int offset);
	CTimeZone GetTimeZoneFromState(std::string state);
//private:
	std::vector<CTimeZone> m_timezones;
};


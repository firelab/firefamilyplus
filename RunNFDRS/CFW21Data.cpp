#include "CFW21Data.h"
#include <fstream>
#include <vector>
#include "csv_readrow.h"
#include "utctime.h"

using namespace std;
using namespace utctime;

tm CFW21Data::ParseISO8061(const string input)
{
	tm thisTime = { 0 };
	//first, need to know if extended or basic ISO 8061 format, and if Zulu time or time zone offset, also if milliseconds are included(but we'll ignore them...)
	bool isExtended = false;
	bool isZulu = false;
	bool hasMillisecs = false;
	size_t tLoc = input.find('T');
	size_t found = input.find('-');
	if (found != string::npos && found < tLoc)
		isExtended = true;
	found = input.find('Z');
	if (found != string::npos)
		isZulu = true;
	found = input.find('.');
	if (found != string::npos)
		hasMillisecs = true;
	//then do appropriate sscanf!
	int nRead;
	int y = -1, M = -1, d = -1, h = -1, m = -1, s = -1, tzh = 0, tzm = 0;
	float ms = 0.0;
	if (isExtended)//has dashes separating fields...
	{
		if (isZulu)
		{
			if(hasMillisecs)
				nRead = sscanf_s(input.c_str(), "%d-%d-%dT%d:%d:%fZ", &y, &M, &d, &h, &m, &ms);
			else
				nRead = sscanf_s(input.c_str(), "%d-%d-%dT%d:%d:%dZ", &y, &M, &d, &h, &m, &s);
		}
		else
		{
			if (hasMillisecs)
				nRead = sscanf_s(input.c_str(), "%d-%d-%dT%d:%d:%f%d%d", &y, &M, &d, &h, &m, &ms, &tzh, &tzm);
			else
				nRead = sscanf_s(input.c_str(), "%d-%d-%dT%d:%d:%d%3d:%d", &y, &M, &d, &h, &m, &s, &tzh, &tzm);
		}
	}
	else//basic
	{
		if (isZulu)
		{
			if (hasMillisecs)
				nRead = sscanf_s(input.c_str(), "%4d%2d%2dT%2d%2d%fZ", &y, &M, &d, &h, &m, &ms);
			else
				nRead = sscanf_s(input.c_str(), "%4d%2d%2dT%2d%2d%2dZ", &y, &M, &d, &h, &m, &s);
		}
		else
		{
			if (hasMillisecs)
				nRead = sscanf_s(input.c_str(), "%4d%2d%2dT%2d%2d%f%3d%2d", &y, &M, &d, &h, &m, &ms, &tzh, &tzm);
			else
				nRead = sscanf_s(input.c_str(), "%4d%2d%2dT%2d%2d%2d%3d%2d", &y, &M, &d, &h, &m, &s, &tzh, &tzm);
		}
	}
	//now build the tm
	if (isZulu)//convert to local time
	{
		UTCTime zTime(y, M, d, h, m, s);
		thisTime = zTime.get_tm();
		tm_increment_hour(&thisTime, m_timeZoneOffset);
	}
	else
	{
		UTCTime lTime(y, M, d, h, m, s);
		thisTime = lTime.get_tm();
	}
	return thisTime;
}

FW21Record::FW21Record()
{
	memset(&m_dateTime, 0, sizeof(m_dateTime));
	m_temp = dNODATA;
	m_RH = dNODATA;
	m_pcp = dNODATA;
	m_windSpeed = dNODATA;
	m_windAzimuth = iNODATA;
	m_solarRadiation = dNODATA;
	m_snowFlag = iNODATA;
	m_gustSpeed = dNODATA;
	m_gustAzimuth = iNODATA;
}

FW21Record::FW21Record(const FW21Record& rhs)
{
	m_dateTime = rhs.m_dateTime;
	m_temp = rhs.m_temp;
	m_RH = rhs.m_RH;
	m_pcp = rhs.m_pcp;
	m_windSpeed = rhs.m_windSpeed;
	m_windAzimuth = rhs.m_windAzimuth;
	m_solarRadiation = rhs.m_solarRadiation;
	m_snowFlag = rhs.m_snowFlag;
	m_gustSpeed = rhs.m_gustSpeed;
	m_gustAzimuth = rhs.m_gustAzimuth;
}
FW21Record::~FW21Record()
{

}

NFDRSRec::NFDRSRec() : FW21Record()
{
	m_minTemp = dNODATA;
	m_maxTemp = dNODATA;
	m_minRH = dNODATA;
	m_pcp24 = dNODATA;
}

NFDRSRec::NFDRSRec(const NFDRSRec& rhs) : FW21Record(rhs)
{
	m_minTemp = rhs.m_minTemp;
	m_maxTemp = rhs.m_maxTemp;
	m_minRH = rhs.m_minRH;
	m_pcp24 = rhs.m_pcp24;
}

NFDRSRec::NFDRSRec(FW21Record rhs)
{
	SetDateTime(rhs.GetDateTime());
	SetTemp(rhs.GetTemp());
	SetRH(rhs.GetRH());
	SetPrecip(rhs.GetPrecip());
	SetWindSpeed(rhs.GetWindSpeed());
	SetSolarRadiation(rhs.GetSolarRadiation());
	SetSnowFlag(rhs.GetSnowFlag());
}

NFDRSRec::~NFDRSRec()
{

}

CFW21Data::CFW21Data()
{
	m_fileName = "";
	m_bTimeIsZulu = false;
	m_timeZoneOffset = 0;
}

CFW21Data::CFW21Data(const CFW21Data& rhs)
{//not implemented!!!!

}

CFW21Data::~CFW21Data()
{

}


int CFW21Data::LoadFile(const char *fw21FileName, int tzOffsetHours/* = 0*/)
{
	m_timeZoneOffset = tzOffsetHours;
	vector<string> vFieldNames = { "DateTime","Temperature(F)","RelativeHumidity(%)","Precipitation(in)",
		"WindSpeed(mph)","WindAzimuth(degrees)","SolarRadiation(W/m2)","SnowFlag","GustSpeed(mph)","GustAzimuth(degrees)" };
	m_fileName = fw21FileName;
	ifstream stream;
	stream.open(m_fileName);
	if (!stream.is_open())
	{
		printf("Error opening %s as input\n", m_fileName.c_str());
		return -1;
	}
	char buf[1024];
	//get the header line which contains FW12 fields
	int bufSize = 1024;
	stream.getline(buf, bufSize);
	string line = buf;
	vector<string> vFields = csv_read_row(line, ',');
	//get field Indexes
	int dtIdx, tmpIdx, rhIdx, pcpIdx, wsIdx, wdirIdx, srIdx, snowIdx, gsIdx, gdirIdx;
	dtIdx = getColIndex(vFieldNames[0], vFields);
	tmpIdx = getColIndex(vFieldNames[1], vFields);
	rhIdx = getColIndex(vFieldNames[2], vFields);
	pcpIdx = getColIndex(vFieldNames[3], vFields);
	wsIdx = getColIndex(vFieldNames[4], vFields);
	wdirIdx = getColIndex(vFieldNames[5], vFields);
	srIdx = getColIndex(vFieldNames[6], vFields);
	snowIdx = getColIndex(vFieldNames[7], vFields);
	gsIdx = getColIndex(vFieldNames[8], vFields);
	gdirIdx = getColIndex(vFieldNames[9], vFields);

	//basic check for required fields
	if (dtIdx < 0 || tmpIdx < 0 || rhIdx < 0 || pcpIdx < 0 || wsIdx < 0 || wdirIdx < 0 || srIdx < 0 || snowIdx < 0)
	{
		if (dtIdx < 0)
			printf("Error, field %s not found in header\n", vFieldNames[0].c_str());
		if (tmpIdx < 0)
			printf("Error, field %s not found in header\n", vFieldNames[1].c_str());
		if (rhIdx < 0)
			printf("Error, field %s not found in header\n", vFieldNames[2].c_str());
		if (pcpIdx < 0)
			printf("Error, field %s not found in header\n", vFieldNames[3].c_str());
		if (wsIdx < 0)
			printf("Error, field %s not found in header\n", vFieldNames[4].c_str());
		if (wdirIdx < 0)
			printf("Error, field %s not found in header\n", vFieldNames[5].c_str());
		if (srIdx < 0)
			printf("Error, field %s not found in header\n", vFieldNames[6].c_str());
		if (snowIdx < 0)
			printf("Error, field %s not found in header\n", vFieldNames[7].c_str());
		printf("Header line is:\n%s\n", buf);
		stream.close();
		return -2;
	}
	//ok, ready to parse the data...
	bool firstRec = false;
	while (stream.good())
	{
		stream.getline(buf, bufSize);
		line = buf;
		vFields = csv_read_row(line, ',');
		if (vFields.size() < 8)
			continue;
		FW21Record thisRec;
		string dStr = vFields[dtIdx];
		if (firstRec)
		{
			//need to check for Zulu time
			if (dStr.find('Z') != string::npos)
				m_bTimeIsZulu = true;
			firstRec = false;
		}
		tm recTime = ParseISO8061(dStr);
		thisRec.SetDateTime(recTime);
		thisRec.SetTemp(atof(vFields[tmpIdx].c_str()));
		thisRec.SetRH(max(atof(vFields[rhIdx].c_str()), 1.0));
		thisRec.SetPrecip(atof(vFields[pcpIdx].c_str()));
		thisRec.SetWindSpeed(atof(vFields[wsIdx].c_str()));
		thisRec.SetWindAzimuth(atoi(vFields[wdirIdx].c_str()));
		thisRec.SetSolarRadiation(atof(vFields[srIdx].c_str()));
		thisRec.SetSnowFlag(atoi(vFields[snowIdx].c_str()));
		if(gsIdx >= 0)
			thisRec.SetGustSpeed(atof(vFields[gsIdx].c_str()));
		if(gdirIdx >= 0)
			thisRec.SetGustAzimuth(atoi(vFields[gdirIdx].c_str()));

		//sanity checks?
		if(thisRec.GetTemp() > -76 && thisRec.GetTemp() < 140 
			&& thisRec.GetRH() > 0 && thisRec.GetRH() <= 100
			&& thisRec.GetSolarRadiation() >= 0 && thisRec.GetSolarRadiation() <= 2000)
			m_recs.push_back(thisRec);
	}
	stream.close();
	return 0;
}

FW21Record CFW21Data::GetRec(size_t recNum)//zero based! valid: 0->GetNumRecs() - 1
{
	FW21Record ret;
	if (recNum >= 0 && recNum < m_recs.size())
		ret = m_recs[recNum];
	return ret;
}

const double SECS_PER_DAY = 86400.0;

NFDRSRec CFW21Data::GetNFDRSRec(size_t recNum)//zero based! valid: 0->GetNumRecs() - 1
{
	FW21Record rec, rec2;
	if (recNum < 0 || recNum >= m_recs.size())
	{
		NFDRSRec ret;
		return ret;
	}
	rec = GetRec(recNum);
	NFDRSRec goodRec(rec);
	tm trgTime = rec.GetDateTime();
	time_t trgTimet = mktime(&trgTime), thisTimet;
	double tMin = rec.GetTemp(), tMax = rec.GetTemp(), rhMin = rec.GetRH(), pcp = rec.GetPrecip();
	__int64 checkRec = recNum - 1;
	while (checkRec >= 0)
	{
		rec2 = GetRec(checkRec);
		tm thisTime = rec2.GetDateTime();
		thisTimet = mktime(&thisTime);
		if (difftime(trgTimet, thisTimet) >= SECS_PER_DAY)
			break;
		double thisTemp = rec2.GetTemp(), thisRH = rec2.GetRH(), thisPcp = rec2.GetPrecip();
		if (thisTemp != dNODATA)
		{
			if (thisTemp < tMin || tMin == dNODATA)
				tMin = thisTemp;
			if (thisTemp > tMax || tMax == dNODATA)
				tMax = thisTemp;
		}
		if (thisRH != dNODATA)
		{
			if (thisRH < rhMin || rhMin == dNODATA)
				rhMin = thisRH;
		}
		if (thisPcp != dNODATA || pcp == dNODATA)
			pcp += thisPcp;
		checkRec--;
	}
	goodRec.SetMinTemp(tMin);
	goodRec.SetMaxTemp(tMax);
	goodRec.SetMinRH(rhMin);
	goodRec.SetPcp24(pcp);
	return goodRec;
}

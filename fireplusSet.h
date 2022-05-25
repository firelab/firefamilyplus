// ffpDescription.h : Declaration of the CFireplusSet

#pragma once
#define DOY_NORMAL	1
#define DOY_EQUAL	2
#define DOY_FLIPPED	3
//const int BASISYEAR = 1993;
int MonthChtoInt(CString chMonth);
/*{
	if(strcmpi(chMonth, "January") == 0)
			return 1;
	if(strcmpi(chMonth, "February") == 0)
			return 2;
	if(strcmpi(chMonth, "March") == 0)
			return 3;
	if(strcmpi(chMonth, "April") == 0)
			return 4;
	if(strcmpi(chMonth, "May") == 0)
			return 5;
	if(strcmpi(chMonth, "June") == 0)
			return 6;
	if(strcmpi(chMonth, "July") == 0)
			return 7;
	if(strcmpi(chMonth, "August") == 0)
			return 8;
	if(strcmpi(chMonth, "September") == 0)
			return 9;
	if(strcmpi(chMonth, "October") == 0)
			return 10;
	if(strcmpi(chMonth, "November") == 0)
			return 11;
	return 12;
}*/

// code generated on Wednesday, September 28, 2005, 1:14 PM
class CTermDatesDialog;

class CFireplusSet : public CRecordset
{
public:
	CFireplusSet(CDatabase* pDatabase = NULL);
	~CFireplusSet();
	DECLARE_DYNAMIC(CFireplusSet)
	void BuildBaseQuery(CString& query, CString dateField, bool yearsOnly, bool inverted = false);
	void BuildBaseQuery2(CString& query, CString dateField, bool yearsOnly, int startYear, int endYear);
	void BuildBaseNFDRS2016Query(CString& query, CString dateField, bool yearsOnly, bool inverted = false);
	bool GetNFDRS2016Range(CString stnID, COleDateTime *start, COleDateTime *end);
	CTermDatesDialog *m_pTermDatesDialog;
// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_Description;	//Description of data base contents
	CString	m_SIG_Station;	//Active SIG or Station
	long	m_StartYear;	//Start Year of active working set
	long	m_EndYear;	//End Year of active working set
	CString	m_StartMonth;	//Start Month of working set
	CString	m_EndMonth;	//End month of working set
	long	m_StartDay;	//Start Day of month of working set
	long	m_EndDay;	//End day of month of working set
	CString	m_PeriodLength;	//Analysis period length
	BOOL	m_Use78;	//<Not Used>
	BOOL	m_Use88;	//<Not Used>
	BOOL	m_UseCanadian;	//Use the Canadian Fire Danger Model

// Overrides
	// Wizard generated virtual function overrides
	public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};



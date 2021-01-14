// FireSet.h : Declaration of the CFireSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:01 PM

class CFireSet : public CRecordset
{
public:
	CFireSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CFireSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_FireID;	//Local unique key
	CString	m_Year;	//Discovery year
	long	m_AgencyID;	//Protection Agency - Link to FireAgency table
	long	m_RegionID;	//Region, broad management unit- Link to FireRegions table
	long	m_UnitID;	//Report Unit (Forest, BLM District, NPS unit, etc.)- Link to FireUnits table
	long	m_SubunitID;	//Subunit - Link to FireSubunits table
	CString	m_FireNumber;	//Fire identifier from report system
	CString	m_State;	//State/Country Code
	long	m_County;	//FIPS county number
	double	m_TotalAcres;	//Total Fire Acres
	CString	m_SizeClass;	//Fire Size Class
	CString	m_FireName;	//Name of the Fire
	long	m_StatisticalCause;	//FS Statistical Cause code
	long	m_SpecificCause;	//FS Specific cause code
	long	m_GeneralCause;	//FS General Cause code
	long	m_ClassPeople;	//FS Class of People code
	CString	m_CauseNarr;	//Free text cause description
	CString	m_Township;	//Township code
	CString	m_Range;	//Range code
	long	m_Section;	//Section
	CString	m_SubSection;	//Subsection (smallest first)
	long	m_LatDD;	//Latitude Deg (+ for East, - for West)
	long	m_LatMM;	//Latitude minutes
	long	m_LatSS;	//Latitude seconds
	long	m_LonDD;	//Longitude Deg (+ for North, - for South)
	long	m_LonMM;	//Longitude minutes
	long	m_LonSS;	//Longitude seconds
	CString	m_Other;	//Other location descriptor
	long	m_WildNum;	//Wilderness ID
	CString	m_Notes;	//General location notes
	COleDateTime	m_Ignition;	//Date/time of ignition (ofetn estimated)
	COleDateTime	m_Discovery;	//Date/time of discovery
	COleDateTime	m_FirstAttack;	//Date/time of first attack
	COleDateTime	m_Reinforcement;	//Date/time ofreinforcment
	COleDateTime	m_DeclareWildlandFire;	//Date/time of declared wildland
	COleDateTime	m_Contain;	//Date/time of fire contained
	COleDateTime	m_StrategyMet;	//Date/time of strategy (usually control) met
	COleDateTime	m_FireOut;	//Date/time fire declared out

	// new ffp4 fields  05/2006
	CString		m_Slope;
	CString		m_Elevation;
	CString		m_Aspect;
	CString		m_FuelModel;
	CString		m_FireType;


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



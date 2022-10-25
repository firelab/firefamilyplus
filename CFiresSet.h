#pragma once
class CFireplusSet;

class CFiresSet :
    public CRecordset
{
public:
	CFiresSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CFiresSet)

	long	m_AgencyID;	//Protection Agency - Link to FireAgency table
	long	m_RegionID;	//Region, broad management unit- Link to FireRegions table
	long	m_UnitID;	//Report Unit (Forest, BLM District, NPS unit, etc.)- Link to FireUnits table
	long	m_SubunitID;	//Subunit - Link to FireSubunits table
	CString	m_FireNumber;	//Fire identifier from report system
	double	m_TotalAcres;	//Total Fire Acres
	CString	m_FireName;	//Name of the Fire
	long	m_StatisticalCause;	//FS Statistical Cause code
	double m_latitude;
	double m_longitude;
	COleDateTime	m_Discovery;	//Date/time of discovery
	COleDateTime	m_Contain;	//Date/time of fire contained
	COleDateTime	m_StrategyMet;	//Date/time of strategy (usually control) met
	CString m_FORID; // InFORMS ID
	CString m_IRWINID;
//overrides
public:
	virtual CString GetDefaultConnect();	// Default connection string

	virtual CString GetDefaultSQL(); 	// default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX support

	// Implementation
	bool FilterToWorkingSet(CFireplusSet* fpSet, int causeType, int moreCauseFlag, bool inverted = false);
	bool FilterToWorkingSet2(CFireplusSet* fpSet, int causeType, int moreCauseFlag);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};


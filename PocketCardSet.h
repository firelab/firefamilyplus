// PocketCardSet.h : Declaration of the CPocketCardSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:08 PM

class CPocketCardSet : public CRecordset
{
public:
	CPocketCardSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CPocketCardSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString	m_FireDangerArea;
	CString	m_AreaBitmap;
	CString	m_Index;
	CString	m_FuelModelName;
	long	m_Year1;
	long	m_Year2;
	CString	m_Fire1;
	CString	m_Fire2;
	CString	m_Fire3;
	double	m_Fire1Value;
	double	m_Fire2Value;
	double	m_Fire3Value;
	COleDateTime	m_Fire1Date;
	COleDateTime	m_Fire2Date;
	COleDateTime	m_Fire3Date;
	CString	m_Line1;
	CString	m_Line2;
	CString	m_Line3;
	long	m_WindSpeed;
	long	m_RH;
	long	m_Temp;
	long	m_LiveFM;
	CString	m_PastExperience;
	//CString	m_StationID;
	CString	m_SIG_Station;
	long	m_pcRyVal;
	long	m_pcGyVal;
	long	m_pcYVal;
	CString	m_ExtraVar;
	long	m_CP;
	BOOL	m_NWCGStd;
	CString	m_RespAgency;


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

	int OpenEnsureSIGStationRec(CString _stationSIG);
};



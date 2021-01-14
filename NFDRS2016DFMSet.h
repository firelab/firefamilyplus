
#pragma once



class CNFDRS2016DFMSet : public CRecordset
{
public:
	CNFDRS2016DFMSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CNFDRS2016DFMSet)

	CString	m_StationID;	//Station ID
	COleDateTime m_ObsDate;
	double m_FM1;
	double m_FM10;
	double m_FM100;
	double m_FM1000;
	double m_FuelTemperature;
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
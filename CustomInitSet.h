// CustomInitsSet.h : Declaration of the CFireUnitSet

#pragma once



class CCustomInitSet : public CRecordset
{
public:
	CCustomInitSet(CDatabase* pDatabase = NULL);

	DECLARE_DYNAMIC(CCustomInitSet)


	CString			m_SIG_Station;
    long	m_Year;
	COleDateTime	m_GreenJulian;
	long				m_StartKBDI;
	float			m_Start1000;
	float			m_FFMC;
	float			m_DMC;
	float			m_DC;
	BOOL			m_Enabled;
	COleDateTime    m_StartUpDay;
	
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


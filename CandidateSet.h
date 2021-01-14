// CandidateSet.h : Declaration of the CRunsSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:17 PM

class CCandidateSet : public CRecordset
{
public:
	CCandidateSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CCandidateSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	CString m_SIG_Station;
	CString m_MODEL;
	CString m_VarName;
	long m_StartYear;
	long m_EndYear;
	CString m_StartMonth;
	CString m_EndMonth;
	long m_StartDay;
	long m_EndDay;
	COleDateTime m_GreenJulian;
	COleDateTime m_FreezeJulian;
	CString m_FD_Type;
	double	m_FD_R2;
	double	m_FD_Chi2;
	double	m_FD_PVal;
	double	m_FD_PMin;
	double	m_FD_PMax;
	double	m_FD_Beta0;
	double	m_FD_Beta1;
	CString m_LFD_Acres;
	double	m_LFD_R2;
	double	m_LFD_Chi2;
	double	m_LFD_PVal;
	double	m_LFD_PMin;
	double	m_LFD_PMax;
	double	m_LFD_Beta0;
	double	m_LFD_Beta1;
	CString m_MFD_NumFires;
	double	m_MFD_R2;
	double	m_MFD_Chi2;
	double	m_MFD_PVal;
	double	m_MFD_PMin;
	double	m_MFD_PMax;
	double	m_MFD_Beta0;
	double	m_MFD_Beta1;
	CString	m_Comment;

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



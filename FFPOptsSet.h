// FFPOptsSet.h : Declaration of the CFFPOptsSet

#pragma once

class CFFPOptsSet : public CRecordset
{
public:
	CFFPOptsSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CFFPOptsSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	long	m_CP1;
	long m_CP1Color;
	long	m_CP2;
	long m_CP2Color;
	long m_RunSaveOn;
	long m_PctWxVarID;
	long m_MaxMissWx;
	long m_LastGfxFmt;
	long m_EventHours;
	long m_EventDays;
	long m_OverlayYear1;
	long m_OverlayYear2;
	long m_OverlayYear3;
	long m_OverlayColor1;
	long m_OverlayColor2;
	long m_OverlayColor3;
	long m_OverlayWidth1;
	long m_OverlayWidth2;
	long m_OverlayWidth3;
	long m_OverlayStyle1;
	long m_OverlayStyle2;
	long m_OverlayStyle3;
	long m_MergeColor1;
	long m_MergeColor2;
	long m_MergeColor3;
	long m_MergeColor4;
	long m_MergeStyle1;
	long m_MergeStyle2;
	long m_MergeStyle3;
	long m_MergeStyle4;
	long m_MergeWidth1;
	long m_MergeWidth2;
	long m_MergeWidth3;
	long m_MergeWidth4;
	long m_EnableFuture;
	long m_VarSortOrder;
	long m_DLExport;
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
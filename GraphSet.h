// GraphSet.h : Declaration of the CGraphSet

#pragma once

// code generated on Wednesday, September 28, 2005, 2:06 PM

class CGraphSet : public CRecordset
{
public:
	CGraphSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CGraphSet)

// Field/Param Data

// The string types below (if present) reflect the actual data type of the
// database field - CStringA for ANSI datatypes and CStringW for Unicode
// datatypes. This is to prevent the ODBC driver from performing potentially
// unnecessary conversions.  If you wish, you may change these members to
// CString types and the ODBC driver will perform all necessary conversions.
// (Note: You must use an ODBC driver version that is version 3.5 or greater
// to support both Unicode and these conversions).

	BOOL	m_Horizontal_Lines;	//Draw Horizontal Grid water marks
	BOOL	m_Vertical_Lines;	//Draw vertical grid water marks
	BOOL	m_Range_Tics;	//Draw range indicators on bargraph
	BOOL	m_CP_Tics;	//Draw Critical percentile indicator on bargraph
	long	m_Line_Width;	//Width of line for line graph
	CString	m_H_Line_Style;	//Linestyle of horizontal water mark
	CString	m_V_Line_Style;	//Linestyle of vertical water mark
	long	m_Line_Color;	//Color of line on line graph
	BOOL	m_InitBargraph;
	BOOL	m_Mins;
	long	m_MinsColor;
	long	m_MinsWidth;
	BOOL	m_Maxs;
	long	m_MaxsColor;
	long	m_MaxsWidth;
	BOOL	m_CP1;
	long	m_CP1Color;
	long	m_CP1Width;
	BOOL	m_CP2;
	long	m_CP2Color;
	long	m_CP2Width;
	long	m_Cause;
	BOOL	m_FD;
	BOOL	m_LFD;
	BOOL	m_MFD;
	long	m_LFDAcres;
	long	m_MFDFires;
	BOOL	m_AutoSave;
	long	m_AutoSaveWidth;
	long	m_AutoSaveHeight;
	long	m_AutoSaveFileType;
	CString	m_AutoSaveLocation;

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

	CString GetGraphicsExtension(void);
};



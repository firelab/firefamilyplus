// GraphSet.h : Implementation of the CGraphSet class

 

// CGraphSet implementation

// code generated on Wednesday, September 28, 2005, 2:06 PM

#include "stdafx.h"
#include "GraphSet.h"
#include ".\graphset.h"
IMPLEMENT_DYNAMIC(CGraphSet, CRecordset)

CGraphSet::CGraphSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_Horizontal_Lines = FALSE;
	m_Vertical_Lines = FALSE;
	m_Range_Tics = FALSE;
	m_CP_Tics = FALSE;
	m_Line_Width = 0;
	m_H_Line_Style = L"";
	m_V_Line_Style = L"";
	m_Line_Color = 0;
	m_InitBargraph = FALSE;
	m_Mins = FALSE;
	m_MinsColor = 0;
	m_MinsWidth = 0;
	m_Maxs = FALSE;
	m_MaxsColor = 0;
	m_MaxsWidth = 0;
	m_CP1 = FALSE;
	m_CP1Color = 0;
	m_CP1Width = 0;
	m_CP2 = FALSE;
	m_CP2Color = 0;
	m_CP2Width = 0;
	m_Cause = 0;
	m_FD = FALSE;
	m_LFD = FALSE;
	m_MFD = FALSE;
	m_LFDAcres = 0;
	m_MFDFires = 0;
	m_AutoSave = FALSE;
	m_AutoSaveWidth = 0;
	m_AutoSaveHeight = 0;
	m_AutoSaveFileType = 3;   // default PNG
	m_AutoSaveLocation = L"";
	m_nFields = 32;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CGraphSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CGraphSet::GetDefaultSQL()
{
	//return _T("SELECT * FROM [ffpGraphOptions]");
	return _T("[ffpGraphOptions]");
}

void CGraphSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Bool(pFX, _T("[Horizontal Lines]"), m_Horizontal_Lines);
	RFX_Bool(pFX, _T("[Vertical Lines]"), m_Vertical_Lines);
	RFX_Bool(pFX, _T("[Range Tics]"), m_Range_Tics);
	RFX_Bool(pFX, _T("[CP Tics]"), m_CP_Tics);
	RFX_Long (pFX, _T("[Line Width]"), m_Line_Width);
	RFX_Text(pFX, _T("[Horizontal Line Style]"), m_H_Line_Style);
	RFX_Text(pFX, _T("[Vertical Line Style]"), m_V_Line_Style);
	RFX_Long(pFX, _T("[LineColor]"), m_Line_Color);
	RFX_Bool(pFX, _T("[InitBargraph]"), m_InitBargraph);
	RFX_Bool(pFX, _T("[Mins]"), m_Mins);
	RFX_Long(pFX, _T("[MinsColor]"), m_MinsColor);
	RFX_Long (pFX, _T("[MinsWidth]"), m_MinsWidth);
	RFX_Bool(pFX, _T("[Maxs]"), m_Maxs);
	RFX_Long(pFX, _T("[MaxsColor]"), m_MaxsColor);
	RFX_Long (pFX, _T("[MaxsWidth]"), m_MaxsWidth);
	RFX_Bool(pFX, _T("[CP1]"), m_CP1);
	RFX_Long(pFX, _T("[CP1Color]"), m_CP1Color);
	RFX_Long (pFX, _T("[CP1Width]"), m_CP1Width);
	RFX_Bool(pFX, _T("[CP2]"), m_CP2);
	RFX_Long(pFX, _T("[CP2Color]"), m_CP2Color);
	RFX_Long (pFX, _T("[CP2Width]"), m_CP2Width);
	RFX_Long (pFX, _T("[Cause]"), m_Cause);
	RFX_Bool(pFX, _T("[FD]"), m_FD);
	RFX_Bool(pFX, _T("[LFD]"), m_LFD);
	RFX_Bool(pFX, _T("[MFD]"), m_MFD);
	RFX_Long (pFX, _T("[LFDAcres]"), m_LFDAcres);
	RFX_Long (pFX, _T("[MFDFires]"), m_MFDFires);
	RFX_Bool(pFX, _T("[AutoSave]"), m_AutoSave);
	RFX_Long (pFX, _T("[AutoSaveWidth]"), m_AutoSaveWidth);
	RFX_Long (pFX, _T("[AutoSaveHeight]"), m_AutoSaveHeight);
	RFX_Long (pFX, _T("[AutoSaveFileType]"), m_AutoSaveFileType);
	RFX_Text(pFX, _T("[AutoSaveLocation]"), m_AutoSaveLocation);

}
/////////////////////////////////////////////////////////////////////////////
// CGraphSet diagnostics

#ifdef _DEBUG
void CGraphSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CGraphSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG



CString CGraphSet::GetGraphicsExtension(void)
{
	CString ret = "BMP";
	switch(this->m_AutoSaveFileType)
	{
	case 0:
		ret = "BMP";
		break;
	case 1:
		ret = "GIF";
		break;
	case 2:
		ret= "JPG";
		break;
	case 3:
		ret = "PNG";
		break;
	default:
		ret = "PNG";
	}
	return ret;
}

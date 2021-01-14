// FFPOptsSet.cpp : Implementation of the CFFPOptsSet class

// CFFPOptsSet implementation
 
#include "stdafx.h"
#include "FFPOptsSet.h"
IMPLEMENT_DYNAMIC(CFFPOptsSet, CRecordset)

CFFPOptsSet::CFFPOptsSet(CDatabase* pdb)
	: CRecordset(pdb)
{
	m_CP1 = 90;
	m_CP1Color = 0;
	m_CP2 = 80;
	m_CP1Color = 0;
	m_RunSaveOn = 0;
	m_PctWxVarID = 1;
	m_MaxMissWx = 0;
	m_LastGfxFmt = 0;
	m_EventHours = 1;
	m_EventDays = 1;
	m_OverlayYear1 = 0;
	m_OverlayYear2 = 0;
	m_OverlayYear3 = 0;
	m_OverlayColor1 = 0;
	m_OverlayColor2 = 0;
	m_OverlayColor3 = 0;
	m_OverlayWidth1 = 0;
	m_OverlayWidth2 = 0;
	m_OverlayWidth3 = 0;
	m_OverlayStyle1 = 0;
	m_OverlayStyle2 = 0;
	m_OverlayStyle3 = 0;
	m_MergeColor1 = 0;
	m_MergeColor2 = 0;
	m_MergeColor3 = 0;
	m_MergeColor4 = 0;
	m_MergeStyle1 = 0;
	m_MergeStyle2 = 0;
	m_MergeStyle3 = 0;
	m_MergeStyle4 = 0;
	m_MergeWidth1 = 0;
	m_MergeWidth2 = 0;
	m_MergeWidth3 = 0;
	m_MergeWidth4 = 0;
	m_EnableFuture = 0;
	m_VarSortOrder = 0;
	m_DLExport = 0;
	m_nFields = 37;
	m_nDefaultType = dynaset;
}
//#error Security Issue: The connection string may contain a password
// The connection string below may contain plain text passwords and/or
// other sensitive information. Please remove the #error after reviewing
// the connection string for any security related issues. You may want to
// store the password in some other form or use a different user authentication.
CString CFFPOptsSet::GetDefaultConnect()
{
	return _T("");//_T("DBQ=F:\\Source\\FFP4\\Data\\r1_short.mdb;DefaultDir=F:\\Source\\FFP4\\Data;Driver={Microsoft Access Driver (*.mdb)};DriverId=281;FIL=MS Access;FILEDSN=C:\\Program Files\\Common Files\\ODBC\\Data Sources\\ffp.dsn;MaxBufferSize=2048;MaxScanRows=8;PageTimeout=5;SafeTransactions=0;Threads=3;UID=admin;UserCommitSync=Yes;");
}

CString CFFPOptsSet::GetDefaultSQL()
{
	return _T("[ffpOpts]");
}

void CFFPOptsSet::DoFieldExchange(CFieldExchange* pFX)
{
	pFX->SetFieldType(CFieldExchange::outputColumn);
// Macros such as RFX_Text() and RFX_Long () are dependent on the
// type of the member variable, not the type of the field in the database.
// ODBC will try to automatically convert the column value to the requested type
	RFX_Long (pFX, _T("[CP1]"), m_CP1);
	RFX_Long(pFX, _T("[CP1Color]"), m_CP1Color);
	RFX_Long (pFX, _T("[CP2]"), m_CP2);
	RFX_Long(pFX, _T("[CP2Color]"), m_CP2Color);
	RFX_Long (pFX, _T("[RunSaveOn]"), m_RunSaveOn);
	RFX_Long (pFX, _T("[PctWxVarID]"), m_PctWxVarID);
	RFX_Long (pFX, _T("[MaxMissWx]"), m_MaxMissWx);
	RFX_Long (pFX, _T("[LastGfxFmt]"), m_LastGfxFmt);
	RFX_Long (pFX, _T("[EventHours]"), m_EventHours);
	RFX_Long (pFX, _T("[EventDays]"), m_EventDays);
	RFX_Long (pFX, _T("[OverlayYear1]"), m_OverlayYear1);
	RFX_Long (pFX, _T("[OverlayYear2]"), m_OverlayYear2);
	RFX_Long (pFX, _T("[OverlayYear3]"), m_OverlayYear3);
	RFX_Long(pFX, _T("[OverlayColor1]"), m_OverlayColor1);
	RFX_Long(pFX, _T("[OverlayColor2]"), m_OverlayColor2);
	RFX_Long(pFX, _T("[OverlayColor3]"), m_OverlayColor3);
	RFX_Long (pFX, _T("[OverlayWidth1]"), m_OverlayWidth1);
	RFX_Long (pFX, _T("[OverlayWidth2]"), m_OverlayWidth2);
	RFX_Long (pFX, _T("[OverlayWidth3]"), m_OverlayWidth3);
	RFX_Long (pFX, _T("[OverlayStyle1]"), m_OverlayStyle1);
	RFX_Long (pFX, _T("[OverlayStyle2]"), m_OverlayStyle2);
	RFX_Long (pFX, _T("[OverlayStyle3]"), m_OverlayStyle3);
	RFX_Long(pFX, _T("[MergeColor1]"), m_MergeColor1);
	RFX_Long(pFX, _T("[MergeColor2]"), m_MergeColor2);
	RFX_Long(pFX, _T("[MergeColor3]"), m_MergeColor3);
	RFX_Long(pFX, _T("[MergeColor4]"), m_MergeColor4);
	RFX_Long (pFX, _T("[MergeStyle1]"), m_MergeStyle1);
	RFX_Long (pFX, _T("[MergeStyle2]"), m_MergeStyle2);
	RFX_Long (pFX, _T("[MergeStyle3]"), m_MergeStyle3);
	RFX_Long (pFX, _T("[MergeStyle4]"), m_MergeStyle4);
	RFX_Long (pFX, _T("[MergeWidth1]"), m_MergeWidth1);
	RFX_Long (pFX, _T("[MergeWidth2]"), m_MergeWidth2);
	RFX_Long (pFX, _T("[MergeWidth3]"), m_MergeWidth3);
	RFX_Long (pFX, _T("[MergeWidth4]"), m_MergeWidth4);
	RFX_Long (pFX, _T("[EnableFuture]"), m_EnableFuture);
	RFX_Long (pFX, _T("[VarSortOrder]"), m_VarSortOrder);
	RFX_Long (pFX, _T("[DL_Export]"), m_DLExport);
}
/////////////////////////////////////////////////////////////////////////////
// CFFPOptsSet diagnostics

#ifdef _DEBUG
void CFFPOptsSet::AssertValid() const
{
	CRecordset::AssertValid();
}

void CFFPOptsSet::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG
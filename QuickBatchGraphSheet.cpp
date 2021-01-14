// QuickBatchGraphSheet.cpp : implementation file
//

#include "stdafx.h"
#include "fireplus.h"
#include "QuickBatchGraphSheet.h"


// CQuickBatchGraphSheet

IMPLEMENT_DYNAMIC(CQuickBatchGraphSheet, CPropertySheet)

/*CQuickBatchGraphSheet::CQuickBatchGraphSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{

}*/

CQuickBatchGraphSheet::CQuickBatchGraphSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage, CDatabase *_pDB)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), graphSet(_pDB)
{
	pCurGraphView = NULL;
	if(_pDB)
		graphSet.Open();
	gp.isQuick = bp.isQuick = lp.isQuick = fp.isQuick = pp.isQuick = true;
	gp.SetGraphSet(&graphSet);
	bp.SetGraphSet(&graphSet);
	lp.SetGraphSet(&graphSet);
	fp.SetGraphSet(&graphSet);
	asp.SetGraphSet(&graphSet);
	pp.m_pDB = _pDB;
	AddPage(&gp);
	AddPage(&bp);
	AddPage(&lp);
	AddPage(&fp);
	AddPage(&pp);
	AddPage(&asp);

}

CQuickBatchGraphSheet::~CQuickBatchGraphSheet()
{
}


BEGIN_MESSAGE_MAP(CQuickBatchGraphSheet, CPropertySheet)
	ON_BN_CLICKED(IDOK, OnOK)
END_MESSAGE_MAP()


// CQuickBatchGraphSheet message handlers
void CQuickBatchGraphSheet::ApplyToGraph()
{
	graphSet.Edit();
	if(gp.m_hWnd)
	{
		gp.UpdateData();
		gp.WriteData(NULL);
	}
	if(bp.m_hWnd)
		bp.WriteData(NULL);
	if(lp.m_hWnd)
		lp.WriteData(NULL);
	if(fp.m_hWnd)
		fp.WriteData(NULL);
	if(pp.m_hWnd)
		pp.WriteData(NULL);
	if (asp.m_hWnd)
		asp.WriteData();
	graphSet.Update();
}

void CQuickBatchGraphSheet::OnOK() 
{
	ApplyToGraph();
	EndDialog(IDOK);
}

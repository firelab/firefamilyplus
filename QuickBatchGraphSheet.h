#pragma once
#include "GraphSet.h"
#include "SplitterFrame.h"
#include "LineGraphProp.h"
#include "BargraphProp.h"
#include "GeneralGraphProp.h"
#include "GraphFiresPage.h"
#include "FDProjPage.h"
//#include "MergedVarsPropPage.h"
#include "GraphAutoSaveProp.h"
#include "GraphView.h"



// CQuickBatchGraphSheet

class CQuickBatchGraphSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CQuickBatchGraphSheet)

public:
	//CQuickBatchGraphSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, CDatabase *_pDB);
	CQuickBatchGraphSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage, CDatabase *_pDB);
	virtual ~CQuickBatchGraphSheet();
	void ApplyToGraph();
	virtual void OnOK();
	CGraphSet graphSet;
	CGeneralGraphProp gp;
	CLineGraphProp lp;
	CBargraphProp bp;
	CGraphAutoSaveProp asp;
	CGraphFiresPage fp;
	CFDProjPage pp;
	CGraphView *pCurGraphView;
	
protected:
	DECLARE_MESSAGE_MAP()
};



#pragma once
#include "LfiSet.h"
#include "LFIGeneralPage.h"
#include "LFIHerbPage.h"
#include "LFIWoodyPage.h"


// CLFIPropertySheet

class CLFIPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CLFIPropertySheet)

public:
	CLFIPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, CDatabase *_pDB = NULL, CString _sigStationID = "######");
	virtual ~CLFIPropertySheet();
	CDatabase *pDB;
	CLFISet lfiSet;
	CLFIGeneralPage generalPage;
	CLFIHerbPage herbPage;
	CLFIWoodyPage woodyPage;
protected:
	DECLARE_MESSAGE_MAP()
};


